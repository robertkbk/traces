#if !defined(UTIL_H)
#define UTIL_H

#include <pthread.h>

#define lock(mutex)                         \
for (int i = 1;                             \
    i && !pthread_mutex_lock(&(mutex));     \
    i = (pthread_mutex_unlock(&(mutex)), 0))

#define $MaybeUnused __attribute__((unused))

#endif // UTIL_H
