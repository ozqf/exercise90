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
    char* data;
};

#define MAX_JOBS 16
Job g_jobs[MAX_JOBS];

volatile i32 g_nextJobToIssue = 0;
volatile i32 g_nextJobToDo = 0;

// Thread entry point
DWORD __stdcall ThreadMain(LPVOID lpThreadParameter)
{
    ThreadInfo handle = *(ThreadInfo*)lpThreadParameter;
    printf("Thread %d started\n", handle.id);
    for(;;)
    {
        if (g_nextJobToDo < g_nextJobToIssue)
        {
            // this returns the incremented value
            // this function works as a memory fence
            LONG jobId = InterlockedIncrement((LONG volatile *)&g_nextJobToDo);
            // Although we just incremented next job, we got the result of the increment,
            // we actually want what it was before we incremented it, so -1:
            jobId -= 1;
            printf("Thread %d: %s\n", handle.id, g_jobs[jobId].data);
        }
    }
    //return COM_ERROR_NONE;
}

void AddJob(char* data)
{
    Job* job = &g_jobs[g_nextJobToIssue];
    job->data = data;
    // do not increment job counter until
    // job has been prepared
    WRITE_BARRIER
    g_nextJobToIssue++;
}

void GetJob()
{
    

}

void FinishJob()
{

}

void IssueJobs()
{
    printf("Issuing jobs\n");
    AddJob("Job 1");
    AddJob("Job 2");
    AddJob("Job 3");
    AddJob("Job 4");
    AddJob("Job 5");
    AddJob("Job 6");
    AddJob("Job 7");
    AddJob("Job 8");
    AddJob("Job 9");
    AddJob("Job 10");
    AddJob("Job 11");
    AddJob("Job 12");
}

void WaitForJobs()
{
    printf("Waiting for %d jobs\n", g_nextJobToIssue);
}

void Test_Win32Threads()
{
    printf("Test threads\n");

    i32 numThreads = 8;
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
    IssueJobs();
    //WaitForJobs();
    printf("Main sleep(1000)\n");
    Sleep(1000);
}

#endif // TEST_WIN32_THREADS_H
