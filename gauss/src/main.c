#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <err.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#include "programData.h"
#include "util.h"
#include "operation.h"

enum {
    THREAD_POOL_SIZE = 16
};

typedef sem_t       Semaphore; 
typedef pthread_t   PThread;

typedef struct __work
{
    Operation   operation;
    long        i;
    long        j;
    long        k;
} Work;

typedef struct __threadData
{
    PThread         threadId;
    ProgramData*    program;
} ThreadData;

Semaphore   freeThreads;
Semaphore   assignment;
Semaphore   received;
Semaphore   completed;
Work        work;
bool        run = true;

void*       threadWorker(void* arg);
ThreadData* initializeThreadPool(long n, ProgramData* p);
void        schedule(ProgramData* p);
void        destroyThreads(ThreadData* tdata, long n);

int main(int argc, char const *argv[])
{
    long size = 0;
    ProgramData program;
    FILE* in;
    FILE* out = stdout;
    
    if (argc < 2) 
        errx(EXIT_FAILURE, "usage: %s <in-file> [out-file]\n", argv[0]);

    if (argc >= 3)
    {
        out = fopen(argv[2], "w");

        if (out == NULL)
            err(EXIT_FAILURE, "fopen");
    }

    in = fopen(argv[1], "r");

    if (in == NULL)
        err(EXIT_FAILURE, "fopen");

    if (fscanf(in, "%ld", &size) != 1)
        errx(EXIT_FAILURE, "fscanf: invalid size parameter\n");

    if (size < 2)
        errx(EXIT_FAILURE, "size: invalid value of %ld\n", size);
    
    if (programInit(&program, size))
        errx(EXIT_FAILURE, "programInit: cannot initialize program\n");

    if (programReadInput(&program, in))
        errx(EXIT_FAILURE, "programReadInput: invalid input data\n");

    sem_init(&freeThreads,  true, 0);
    sem_init(&assignment,   true, 0);
    sem_init(&received,     true, 0);
    sem_init(&completed,    true, 0);

    ThreadData* threads = initializeThreadPool(THREAD_POOL_SIZE, &program);

    if (threads == NULL)
        err(EXIT_FAILURE, "initializeThreadPool");

    schedule(&program);

    destroyThreads(threads, THREAD_POOL_SIZE);

    programWriteFile(&program, out);
    fputc('\n', out);

    programDestroy(&program);
    sem_destroy(&freeThreads);
    sem_destroy(&assignment);
    sem_destroy(&received);
    sem_destroy(&completed);
    fclose(in);
    return EXIT_SUCCESS;
}

void* threadWorker(void* arg)
{
    ThreadData* thread = (ThreadData*) arg;
    Work w;

    while (run) 
    {
        sem_post(&freeThreads);
        
        sem_wait(&assignment);

        if (run == false)
            return NULL;
        
        w = work;
        sem_post(&received);

        w.operation(thread->program, w.i, w.j, w.k);
        sem_post(&completed);
    }

    return NULL;
}

ThreadData* initializeThreadPool(long n, ProgramData* p)
{
    ThreadData* tdata = NULL;
    ThreadData* it;

    tdata = malloc(n * sizeof(ThreadData));

    if (tdata == NULL)
        return NULL;

    for (it = tdata; it < tdata + n; it++) 
    {
        if (pthread_create(&it->threadId, NULL, threadWorker, it))
            goto cleanup;

        it->program = p;
    }
    
    return tdata;

    cleanup:
    for (ThreadData* t = tdata; t < it; t++)
        if (pthread_cancel(t->threadId))
            err(EXIT_FAILURE, "pthread_cancel");

    return NULL;
}

void assign(Operation o, long i, long j, long k) 
{
    work = (Work) {
        .operation = o,
        .i = i,
        .j = j,
        .k = k
    };
    
    sem_wait(&freeThreads);
    sem_post(&assignment);
    sem_wait(&received);
}

void awaitCompletion(long n)
{
    for (long i = 0; i < n; i++)
        sem_wait(&completed);
}

void schedule(ProgramData* p)
{
    long n = p->size;
    
    for (long i = 0; i < n - 1; i++)
    {    
        for (long k = i + 1; k < n; k++)
        {
            assign(A, i, -1, k);
            awaitCompletion(1);

            for (long j = i; j <= n; j++)
                assign(B, i, j, k);

            awaitCompletion(n - i + 1);

            for (long j = i; j <= n; j++)
                assign(C, i, j, k);
            
            awaitCompletion(n - i + 1);
        }
    }
}

void destroyThreads(ThreadData* threads, long n)
{
    run = false;

    for (long i = 0; i < n; i++)
        sem_post(&assignment);

    for (ThreadData* it = threads; it < threads + n; it++)
        pthread_join(it->threadId, NULL);
}
