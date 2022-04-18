#if !defined(OPERATION_H)
#define OPERATION_H

#include "programData.h"

typedef void (*Operation)(ProgramData*, long, long, long);

void A(ProgramData*, long, long, long);
void B(ProgramData*, long, long, long);
void C(ProgramData*, long, long, long);

#endif // OPERATION_H


