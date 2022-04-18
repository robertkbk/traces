#include "operation.h"
#include "util.h"

#define M(a, b)     (*matrixAt(p, a, b))
#define m(a, b)     (*multiplierAt(p, a, b))
#define c(a, b, c)  (*subtrahendAt(p, a, b, c))

void A(ProgramData* p, long i, $MaybeUnused long j, long k)
{
    m(k, i) = M(k, i) / M(i, i);
}

void B(ProgramData* p, long i, long j, long k)
{
    c(i, j, k) = M(i, j) * m(k, i);
}

void C(ProgramData* p, long i, long j, long k)
{
    M(k, j) -= c(i, j, k);
}