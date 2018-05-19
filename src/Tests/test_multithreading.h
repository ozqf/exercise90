#pragma once

#include <windows.h>
// For compiler intrinisics, which are used in _WriteBarrier();
#include <intrin.h>

//TODO: Is this correct...?
#define WRITE_BARRIER _WriteBarrier(); _mm_sfence();
#define READ_BARRIER _ReadBarrier()

#define MAX_THREADS 6
static HANDLE g_threads[MAX_THREADS];
static int g_threadParams[MAX_THREADS];
static unsigned char g_numThreads = 0;

static HANDLE g_mutex;
static HANDLE g_semaphore;

struct Job
{

    char* stringToPrint;
};

#define MAX_JOBS 256
// Volatile: telling the compile that this variable may appear constant
// on the current thread of execution but that another thread may
// change it and always load its current value, so don't optimise!
static u32 volatile g_jobCount = 0;
static Job g_jobs[MAX_JOBS];

struct Win32ThreadInfo
{
    i32 threadIndex;
};

static void AddJob(char* stringToPrint)
{
    // ensure
    g_jobs[g_jobCount].stringToPrint = stringToPrint;
    // Make sure the string is assigned before the counter is incremented!
    WRITE_BARRIER
    g_jobCount++;
    // ReleaseSemaphore to wake a thread
}

/*
// Increment the semaphore
BOOL WINAPI ReleaseSemaphore(
  _In_      HANDLE hSemaphore,
  _In_      LONG   lReleaseCount,
  _Out_opt_ LPLONG lpPreviousCount
);
*/
DWORD WINAPI ThreadLoop(LPVOID lpParameter)
{
    for(;;)
    {
        ILLEGAL_CODE_PATH
        /*
        if (found work)
        {
            // take work
            // do work
            // commit work
        }
        else
        {
            // wait for work
        }
        */
    }
}

DWORD WINAPI Test_ThreadProc(LPVOID lpParameter)
{
    int param = *(int*)lpParameter;
    // for(;;)
    // {
    //     // Interlocked increment will also act as a barrier... read or write though?

    //     //InterlockedIncrement
    // }
    return 0;
}

DWORD WINAPI Test_ThreadProcSlow(LPVOID lpParameter)
{
    int param = *(int*)lpParameter;
    //printf("Hello from thread %d\n", param);
    int i = 0;
    // Please melt my CPU
    while (i++ < 200000000) { }
    printf("Thread %d done: %d\n", param, i);
    return 0;
}

/**
Notes on multithreading:
> pointers CANNOT be shared for params. the time the pointer is actually read is unpredictable.
    a for loop starting threads will result in most reading the final value or worse: the object
    may have fallen out of the stack and garbage will be read!

    Make sure no threads share input or output memory locations.
*/
void Test_StartTemporaryThreads()
{
    g_mutex = CreateMutex(0, FALSE, 0);
    g_semaphore = CreateSemaphore(0, 0, 9999, "Jobs");

    int numThreads = MAX_THREADS;
    // int param1 = 1;
    // int param2 = 2;
    printf("Start %d temporary threads test\n", numThreads);

    for (int i = 0; i < numThreads; ++i)
    {
        Win32ThreadInfo info = {};
        info.threadIndex = i;

        g_threadParams[i] = i;
        g_threads[i] = CreateThread(NULL, NULL, Test_ThreadProcSlow, &g_threadParams[i], NULL, NULL);
    }

    AddJob("Work A");
    printf("Threads started\n");

    printf("WaitForMultipleObjects\n");
    WaitForMultipleObjects(numThreads, g_threads, TRUE, 20000);

    for(int i = 0; i < numThreads; ++i)
    {
        CloseHandle(g_threads[i]);
        g_threads[i] = NULL;
    }

    printf("Threads Finished?\n");
}

void Test_MultiThreading()
{
    Test_StartTemporaryThreads();
}