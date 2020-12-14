#include <stddef.h>
#include <sys/time.h>
#include "mytime.h"

double Time()
{
    static struct timeval t;
    gettimeofday(&t, NULL);
    return ((double)t.tv_sec + (double)t.tv_usec * 1.0e-06);
}
