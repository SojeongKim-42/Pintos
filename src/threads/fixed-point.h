#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H
typedef int Fpoint;
static const int F = (1 << 14);

/* Operations of integer & fixed point numbers.

   We use 17.14 fixed-point number representation.
   f: 2^14
   x, y: fixed point numbers
   n: integer   */

/* n to fixed point */
Fpoint to_fp(int n);

/* x to integer rounding toward zero (floor) */
Fpoint to_int(Fpoint x);

/* x to integer rounding to nearest (round) */
Fpoint to_int_round(Fpoint x);

/* add fixed poINT Numbers x, y -> x+y */
Fpoint add_fps(Fpoint x, Fpoint y);

/* subtract fixed poFpoint y from fixed poFpoint x -> x-y */
Fpoint sub_fps(Fpoint x, Fpoint y);

/* add fixed poFpoint x and int n -> x+n */
Fpoint add_mix(Fpoint x, int n);

/* subtract int n from fixed poFpoint y -> x-n */
Fpoint sub_mix(Fpoint x, int n);

/* multiply fixed poFpoint x by fixed poFpoint y -> x*y */
Fpoint multiply_fps(Fpoint x, Fpoint y);

/* multiply fixed poFpoint x by int n -> x*n */
Fpoint multiply_mix(Fpoint x, int n);

/* divide fixed poFpoint x by fixed poFpoint y -> x/y*/
Fpoint divide_fps(Fpoint x, Fpoint y);

/* divide fixed poFpoint x by int n -> x/n */
Fpoint divide_mix(Fpoint x, int n);


Fpoint 
to_fp(int n)
{
    return n * F;
}

Fpoint 
to_int(Fpoint x)
{
    return x / F;
}

Fpoint 
to_int_round(Fpoint x)
{
    if (x >= 0)
        return (x + F / 2) / F;
    return (x - F / 2) / F;
}

Fpoint 
add_fps(Fpoint x, Fpoint y)
{
    return x + y;
}

Fpoint 
sub_fps(Fpoint x, Fpoint y)
{
    return x - y;
}

Fpoint 
add_mix(Fpoint x, int n)
{
    return x + n * F;
}

Fpoint 
sub_mix(Fpoint x, int n)
{
    return x - n * F;
}

Fpoint 
multiply_fps(Fpoint x, Fpoint y)
{
    return ((int64_t)x) * y / F;
}

Fpoint 
multiply_mix(Fpoint x, int n)
{
    return x * n;
}

Fpoint 
divide_fps(Fpoint x, Fpoint y)
{
    return ((int64_t)x) * F / y;
}

Fpoint 
divide_mix(Fpoint x, int n)
{
    return x / n;
}

#endif /* threads/fixed-point */