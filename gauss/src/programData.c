#include "programData.h"

#include <stdlib.h>
#include <stdio.h>

double* matrixAt(ProgramData* p, long i, long j)
{
    return p->matrix + (i * (p->size + 1)) + j;
}

double* multiplierAt(ProgramData* p, long i, long j)
{
    return p->multipliers + (i * p->size) + j;
}

double* subtrahendAt(ProgramData* p, long i, long j, long k)
{
    const long n = p->size;

    return p->subtrahends + (i * n * (n + 1)) + (j * n) + k;
}

int programInit(ProgramData* p, long n)
{
    if (p == NULL || n < 2)
        return 1;

    p->size         = n;
    p->matrix       = malloc(n * (n + 1) * sizeof(double));
    p->multipliers  = malloc(n * n * sizeof(double));
    p->subtrahends  = malloc(n * (n + 1) * n * sizeof(double));

    if (p->matrix == NULL || p->multipliers == NULL || p->subtrahends == NULL)
        return (programDestroy(p), 1);

    return 0;
}

void programDestroy(ProgramData* p)
{
    free(p->matrix);
    free(p->multipliers);
    free(p->subtrahends);
}

int programReadInput(ProgramData* p, FILE* f)
{
    if (f == NULL || p == NULL)
        return 1;

    for (int i = 0; i < p->size; i++) 
    {
        for (int j = 0; j < p->size; j++)
        {
            if (fscanf(f, "%lf", matrixAt(p, i, j)) != 1) 
            {
                programDestroy(p);
                return 1;
            }
        }
    }

    for (int i = 0; i < p->size; i++)
    {
        if (fscanf(f, "%lf", matrixAt(p, i, p->size)) != 1)
        {
            programDestroy(p);
            return 1;
        }
    }

    return 0;
}

void programWriteFile(ProgramData* p, FILE* f)
{
    if (p == NULL || f == NULL)
        return;

    for (int i = 0; i < p->size; i++)
    {
        for (int j = 0; j < p->size; j++)
        {
            fprintf(f, "%lf ", *matrixAt(p, i, j));
        }

        fputc('\n', f);
    }

    for (int i = 0; i < p->size; i++)
    {
        fprintf(f, "%lf ", *matrixAt(p, i, p->size));
    }
}

void programPrint(ProgramData* p)
{
    programWriteFile(p, stdout);
}
