#if !defined(PROGRAMDATA_H)
#define PROGRAMDATA_H

#include <stdio.h>

typedef struct __programData
{
    double*     matrix;
    double*     multipliers;
    double*     subtrahends;
    long        size;
} ProgramData;

int     programInit(ProgramData*, long);
void    programDestroy(ProgramData*);

int     programReadInput(ProgramData*, FILE*);
void    programWriteFile(ProgramData*, FILE*);
void    programPrint(ProgramData*);

double* matrixAt(ProgramData* p, long i, long j);
double* multiplierAt(ProgramData* p, long i, long j);
double* subtrahendAt(ProgramData* p, long i, long j, long k);

#endif // PROGRAMDATA_H
