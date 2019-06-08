#ifndef TEST_WIN32_THREADS_H
#define TEST_WIN32_THREADS_H

#include <stdio.h>
#include <windows.h>
// for barrier intrinsics
#include <intrin.h>
#include "../../common/common.h"

// Macro to define compiler intrinsics
#define WRITE_BARRIER _WriteBarrier(); _mm_sfence();

struct ThreadInfo
{
    i32 id;
};

#define MAX_THREADS 16
i32 g_nextThread = 0;
ThreadInfo g_threads[MAX_THREADS];

struct Job
{
    i32 id;
    char* data;
};

#define MAX_JOBS 256
Job g_jobs[MAX_JOBS];

volatile i32 g_nextJobToIssue = 0;
volatile i32 g_nextJobToDo = 0;
volatile i32 g_jobsCompleted = 0;

Job* GetJobToDo();
void FinishJob(i32 jobId);

// Thread entry point
DWORD __stdcall ThreadMain(LPVOID lpThreadParameter)
{
    ThreadInfo handle = *(ThreadInfo*)lpThreadParameter;
    printf("Thread %d started\n", handle.id);
    for(;;)
    {
        Job* job = GetJobToDo();
        if (job == NULL) { continue; }

        printf("Thread %d doing job %d %s\n", handle.id, job->id, job->data);
        FinishJob(job->id);
    }
    //return COM_ERROR_NONE;
}

void AddJob(char* data)
{
    Job* job = &g_jobs[g_nextJobToIssue];
    job->id = g_nextJobToIssue;
    job->data = data;
    // do not increment job counter until
    // job has been prepared
    WRITE_BARRIER
    printf("issued job %d\n", g_nextJobToIssue);
    g_nextJobToIssue++;
}

Job* GetJobToDo()
{
    if (g_nextJobToDo < g_nextJobToIssue)
    {
        // this returns the incremented value
        // this function works as a memory fence
        LONG jobId = InterlockedIncrement((LONG volatile *)&g_nextJobToDo);
        // Although we just incremented next job, we got the result of the increment,
        // we actually want what it was before we incremented it, so -1:
        jobId -= 1;
        return &g_jobs[jobId];
    }
    Sleep(10);
    return NULL;
}

void FinishJob(i32 id)
{
    InterlockedIncrement((LONG volatile *)&g_jobsCompleted);
    printf("  Finished job %d! completed: %d\n", id, g_jobsCompleted);
}

void IssueJobs(i32 numJobs)
{
    printf("-- ISSUING JOBS --\n");
    for (i32 i = 0; i < numJobs; ++i)
    {
        AddJob("DATA");
    }
    printf("-- JOBS ISSUED --\n");
}

void WaitForJobs()
{
    printf("Waiting for %d jobs\n", g_nextJobToIssue);
    while (g_jobsCompleted < g_nextJobToIssue) { }
    printf("Finished on %d of %d!\n", g_jobsCompleted, g_nextJobToIssue);
}

void DoAllJobs()
{
    for (;;)
    {
        Job* job = GetJobToDo();
        if (job == NULL) { return; }
        printf("Doing job %d single threaded\n", job->id);
        FinishJob(job->id);
    }
}

void Test_Win32Threads()
{
    printf("=== Test threads ===\n");
    i32 numThreads = 4;
    i32 numJobs = 4;
    printf("Creating %d threads and %d jobs\n", numThreads, numJobs);
    for (i32 i = 0; i < numThreads; ++i)
    {
        ThreadInfo* info = &g_threads[g_nextThread++];
        info->id = i;
        DWORD threadId;
        HANDLE handle = CreateThread(0, 0, ThreadMain, info, 0, &threadId);
        CloseHandle(handle);
    }
    /*
    HANDLE CreateThread(
        LPSECURITY_ATTRIBUTES   lpThreadAttributes,
        SIZE_T                  dwStackSize,
        LPTHREAD_START_ROUTINE  lpStartAddress,
        __drv_aliasesMem LPVOID lpParameter,
        DWORD                   dwCreationFlags,
        LPDWORD                 lpThreadId
    );
     */
    IssueJobs(numJobs);
    if (numThreads == 0)
    {
        DoAllJobs();
    }
    WaitForJobs();
    #if 0
    printf("Main sleep(1000)\n");
    Sleep(1000);
    printf("Main resume\n");
    #endif
}

#endif // TEST_WIN32_THREADS_H
