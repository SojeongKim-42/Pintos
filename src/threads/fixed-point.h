#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H
#define F (1 << 14)
typedef int FP;


/* Operations of integer & fixed point numbers.

   We use 17.14 fixed-point number representation.
   f: 2^14
   x, y: fixed point numbers
   n: integer   */

/* n to fixed point */
FP to_fp(int n);

/* x to integer rounding toward zero (floor) */
int to_int(int x);

/* x to integer rounding to nearest (round) */
int to_int_round(int x);

/* add fixed point numbers x, y -> x+y */
FP add_fps(int x, int y);

/* subtract fixed point y from fixed point x -> x-y */
FP sub_fps(int x, int y);

/* add fixed point x and int n -> x+n */
FP add_mix(int x, int n);

/* subtract int n from fixed point y -> x-n */
FP sub_mix(int x, int n);

/* multiply fixed point x by fixed point y -> x*y */
FP multiply_fps(int x, int y);

/* multiply fixed point x by int n -> x*n */
FP multiply_mix(int x, int n);

/* divide fixed point x by fixed point y -> x/y*/
FP divide_fps(int x, int y);

/* divide fixed point x by int n -> x/n */
FP divide_mix(int x, int n);


FP 
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

FP
add_fps(int x, int y)
{
    return x + y;
}

FP
sub_fps(int x, int y)
{
    return x - y;
}

FP
add_mix(int x, int n)
{
    return x + n * F;
}

FP 
sub_mix(int x, int n)
{
    return x - n * F;
}

FP 
multiply_fps(int x, int y)
{
    return ((int64_t)x) * y / F;
}

FP 
multiply_mix(int x, int n)
{
    return x * n;
}

FP
divide_fps(int x, int y)
{
    return ((int64_t)x) * F / y;
}

FP 
divide_mix(int x, int n)
{
    return x / n;
}

#endif /* threads/fixed-point */