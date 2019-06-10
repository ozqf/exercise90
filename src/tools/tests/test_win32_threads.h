#ifndef TEST_WIN32_THREADS_H
#define TEST_WIN32_THREADS_H

#include <stdio.h>
#include <windows.h>
#include <sysinfoapi.h>
#include <intrin.h> // for barrier intrinsics
#include "../../common/common.h"

// Macro to define compiler intrinsics
#define WRITE_BARRIER _WriteBarrier(); _mm_sfence();
#define READ_BARRIER _ReadBarrier();

struct ThreadInfo
{
    i32 id;
};

#define MAX_THREADS 16
i32 g_nextThread = 0;
ThreadInfo g_threads[MAX_THREADS];

struct Job
{
    volatile i32 threadId;
    i32 id;
    char* data;
};

#define MAX_JOBS 256
Job g_jobs[MAX_JOBS];

volatile i32 g_nextJobToIssue = 0;
volatile i32 g_nextJobToDo = 0;
volatile i32 g_jobsCompleted = 0;

Job* GetJobToDo(i32 threadId);
void FinishJob(i32 jobId);
void FindJobIndexByAddress(Job* queryJob);

char* g_jobPayloads[] = {
    "DATA 1", "DATA 2", "DATA 3", "DATA 4", "DATA 5", "DATA 6",
    "DATA 7", "DATA 8", "DATA 9", "DATA 10", "DATA 11", "DATA 12",
    "DATA 13", "DATA 14", "DATA 15", "DATA 16", "DATA 17", "DATA 18",
};

// Thread entry point
DWORD __stdcall ThreadMain(LPVOID lpThreadParameter)
{
    ThreadInfo handle = *(ThreadInfo*)lpThreadParameter;
    printf("Thread %d started\n", handle.id);
    for(;;)
    {
        #if 1
        Job* job = GetJobToDo(handle.id);
        if (job == NULL) { continue; }
        if (job->data == NULL)
        {
            printf("THREAD %d GOT NULL DATA. Finding job...\n", handle.id);
            FindJobIndexByAddress(job);
            continue;
        }
        else
        {
            printf("Thread %d doing job %d %s\n", handle.id, job->id, job->data);
        }
        //Sleep(1);
        FinishJob(job->id);
        #endif
    }
    //return COM_ERROR_NONE;
}
#if 1
Job* GetJobToDo(i32 threadId)
{
    /*LONG InterlockedCompareExchange(
      LONG volatile *Destination,
      LONG          ExChange,
      LONG          Comperand
    );*/
	// The problem here is that two threads can compare the same values
    // at the same time. They will both pass the check, increment the job
    // counter, and one will grab a job that hasn't been issued yet!
    // The increments are thread safe, but the condition to increment
    // is NOT!
	// The InterlockedCompareExchange is used to check that another thread
	// hasn't sneaked in and grabbed the job at the same time
    if (g_nextJobToDo < g_nextJobToIssue)
    {
        //printf("THREAD %d checking for job: Todo %d < Issue %d\n",
        //    threadId, g_nextJobToDo, g_nextJobToIssue);
        Job* job = &g_jobs[g_nextJobToDo];
        i32 result = (i32)InterlockedCompareExchange(
            (volatile long*)&job->threadId, (long)threadId, -1);
        if (result != -1)
        {
            // we failed to acquire the job, abort.
            printf("!!CLASH!! Thread %d cannot acquire job owned by thread %d\n", threadId, job->threadId);
            return NULL;
        }
        //printf("Thread %d got job %d\n", threadId, g_nextJobToDo);
        LONG jobId = InterlockedIncrement((LONG volatile *)&g_nextJobToDo);
        return job;
    }
    return NULL;
}
#endif
void FinishJob(i32 id)
{
    i32 num = InterlockedIncrement((LONG volatile *)&g_jobsCompleted);
    printf("  Finished job %d! completed: %d\n", id, num);
}

void AddJob(char* data)
{
    printf("Adding job %d\n", g_nextJobToIssue);
    Job* job = &g_jobs[g_nextJobToIssue];
    job->id = g_nextJobToIssue;
    job->data = data;
    job->threadId = -1;
    // do not increment job counter until
    // job has been prepared
    WRITE_BARRIER
    //printf("  added job %d\n", g_nextJobToIssue);
    ++g_nextJobToIssue;
}

void CreateJobs(i32 numJobs)
{
    printf("-- CREATING JOBS --\n");
    for (i32 i = 0; i < numJobs; ++i)
    {
        AddJob(g_jobPayloads[i]);
        //Sleep(10);
    }
    printf("-- JOBS CREATED --\n");
}

void FindJobIndexByAddress(Job* queryJob)
{
    for (i32 i = 0; i < MAX_JOBS; ++i)
    {
        Job* job = &g_jobs[i];
        if (job == queryJob)
        {
            printf("  0x%X is job %d!\n", (u32)queryJob, i);
            return;
        }
    }
    printf("Failed to find job at addr %d\n", (u32)queryJob);
}

void WaitForJobs()
{
    printf("Waiting for %d jobs\n", g_nextJobToIssue);
    while (g_jobsCompleted < g_nextJobToIssue) { }
    printf("Finished on %d of %d!\n", g_jobsCompleted, g_nextJobToIssue);
    //Sleep(100);
}

void DoAllJobs()
{
    printf("Doing jobs single threaded\n");
    for (;;)
    {
        Job* job = GetJobToDo(0);
        if (job == NULL) { return; }
        FinishJob(job->id);
    }
}

void Test_JobQueue()
{
    printf("=== Test job queue ===\n");
    i32 numThreads = 4;
    i32 numJobs = 4;
    printf("Creating %d threads and %d jobs\n", numThreads, numJobs);
    for (i32 i = 0; i < numThreads; ++i)
    {
        ThreadInfo* info = &g_threads[g_nextThread++];
        info->id = i + 1;
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
    CreateJobs(numJobs);
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

////////////////////////////////////////////////////////////////////////////////////
// Concurrency test
////////////////////////////////////////////////////////////////////////////////////
#define USE_MUTEX

#define EVENT_CODE_NONE -1

//#define MAX_TEST_EVENTS 10
//i32 g_testEvents[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

#define MAX_EVENTS_QUEUED 256
i32 g_eventQueue[MAX_EVENTS_QUEUED];
i32 g_numEvents = 0;

HANDLE g_queueMutex;

void EventQueue_Push(i32 event)
{
    #ifdef USE_MUTEX
    WaitForSingleObject(g_queueMutex, INFINITE);
    #endif

    if (g_numEvents == MAX_EVENTS_QUEUED)
    {
        printf("EVENT QUEUE OVERFLOW");
    }
    else
    {
        g_eventQueue[g_numEvents] = event;
        g_numEvents++;
    }
    #ifdef USE_MUTEX
    ReleaseMutex(g_queueMutex);
    #endif
}

i32 EventQueue_Pop()
{
    #ifdef USE_MUTEX
    WaitForSingleObject(g_queueMutex, INFINITE);
    #endif
    i32 result;
    if (g_numEvents == 0)
    {
        result = EVENT_CODE_NONE;
    }
    else
    {
        result = g_eventQueue[0];
        // Shift queue down so that queue is FIFO
        for (i32 i = 1; i < g_numEvents; ++i)
        {
            g_eventQueue[i - 1] = g_eventQueue[i];
        }
        g_numEvents--;
    }
    #ifdef USE_MUTEX
    ReleaseMutex(g_queueMutex);
    #endif
    return result;
}

// Thread entry point
DWORD __stdcall AppThreadMain(LPVOID lpThreadParameter)
{
	printf("Begin application thread\n");
    ThreadInfo handle = *(ThreadInfo*)lpThreadParameter;
    i32 ticks = 0;
    //for (i32 i = 0; i < 10; ++i)
    for(;;)
    {
        //printf("Thread %d: %d\n", handle.id, i);
        //Sleep((i32)(COM_STDRandf32() * 500));
        i32 numEventsRead = 0;
        char buf[256];
		char* write = buf;
        COM_SET_ZERO(buf, 256);
		
		ticks++;
        i32 event;
        event = EventQueue_Pop();
		
        while (event != EVENT_CODE_NONE)
        {
            numEventsRead++;
			printf(".");
			i32 written = snprintf(write, 256, "%d, ", event);
			write += written;
            //printf("%d, ", event);
            event = EventQueue_Pop();
        }
        if (numEventsRead > 0)
        {
            printf("App read %d events (%s) on tick %d\n", numEventsRead, buf, ticks);
        }
    }
    //printf("  App thread done\n");
    //return 0;
}

void Test_RunMain()
{
	printf("Main thread - entering loop\n");
    i32 ticks = 0;
    while (ticks++ < 20)
    {
        // Check for messages from AppThread here
        i32 numEvents = (i32)(COM_STDRandf32() * 5);
        for (i32 i = 0; i < numEvents; ++i)
        {
            EventQueue_Push(ticks);
        }
        if (numEvents > 0)
        {
            printf("Main posted %d events on tick %d\n", numEvents, ticks);
        }
        Sleep((i32)(COM_STDRandf32() * 200));
    }
    printf("  Main done. Events remaining: %d\n", g_numEvents);
    //Sleep(2000);
}

void Test_Concurrency()
{
    printf("=== Test concurrency ===\n");

    g_queueMutex = CreateMutexA(0, 0, "EventQueue");

    ThreadInfo* info = &g_threads[g_nextThread++];
    info->id = 1;
    DWORD threadId;
    HANDLE handle = CreateThread(0, 0, AppThreadMain, info, 0, &threadId);
    CloseHandle(handle);
    #if 1
    Test_RunMain();
    #endif
    #if 0
    ThreadInfo* self = &g_threads[g_nextThread++];
    self->id = 2;
    threadId;
    AppThreadMain(self);
    #endif
}

void Test_Win32Threads()
{
    Test_Concurrency();
    
}

#endif // TEST_WIN32_THREADS_H
