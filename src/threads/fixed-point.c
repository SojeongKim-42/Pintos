#include "threads/fixed-point.h"
#include <stdint.h>

int 
to_fp(int n)
{
    return n * F;
}

int 
to_int(int x)
{
    return x / F;
}

int 
to_int_round(int x)
{
    if (x >= 0)
        return (x + F / 2) / F;
    return (x - F / 2) / F;
}

int 
add_fps(int x, int y)
{
    return x + y;
}

int 
sub_fps(int x, int y)
{
    return x - y;
}

int 
add_mix(int x, int n)
{
    return x + n * F;
}

int 
sub_mix(int x, int n)
{
    return x - n * F;
}

int 
multiply_fps(int x, int y)
{
    return ((int64_t)x) * y / F;
}

int 
multiply_mix(int x, int n)
{
    return x * n;
}

int 
divide_fps(int x, int y)
{
    return ((int64_t)x) * F / y;
}

int 
divide_mix(int x, int n)
{
    return x / n;
}