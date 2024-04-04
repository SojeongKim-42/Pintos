#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H
static const int F = (1 << 14);

/* Operations of integer & fixed point numbers.

   We use 17.14 fixed-point number representation.
   f: 2^14
   x, y: fixed point numbers
   n: integer   */

/* n to fixed point */
int to_fp(int n);

/* x to integer rounding toward zero (floor) */
int to_int(int x);

/* x to integer rounding to nearest (round) */
int to_int_round(int x);

/* add fixed point numbers x, y -> x+y */
int add_fps(int x, int y);

/* subtract fixed point y from fixed point x -> x-y */
int sub_fps(int x, int y);

/* add fixed point x and int n -> x+n */
int add_mix(int x, int n);

/* subtract int n from fixed point y -> x-n */
int sub_mix(int x, int n);

/* multiply fixed point x by fixed point y -> x*y */
int multiply_fps(int x, int y);

/* multiply fixed point x by int n -> x*n */
int multiply_mix(int x, int n);

/* divide fixed point x by fixed point y -> x/y*/
int divide_fps(int x, int y);

/* divide fixed point x by int n -> x/n */
int divide_mix(int x, int n);


// int 
// to_fp(int n)
// {
//     return n * F;
// }

// int 
// to_int(int x)
// {
//     return x / F;
// }

// int 
// to_int_round(int x)
// {
//     if (x >= 0)
//         return (x + F / 2) / F;
//     return (x - F / 2) / F;
// }

// int 
// add_fps(int x, int y)
// {
//     return x + y;
// }

// int 
// sub_fps(int x, int y)
// {
//     return x - y;
// }

// int 
// add_mix(int x, int n)
// {
//     return x + n * F;
// }

// int 
// sub_mix(int x, int n)
// {
//     return x - n * F;
// }

// int 
// multiply_fps(int x, int y)
// {
//     return ((int64_t)x) * y / F;
// }

// int 
// multiply_mix(int x, int n)
// {
//     return x * n;
// }

// int 
// divide_fps(int x, int y)
// {
//     return ((int64_t)x) * F / y;
// }

// int 
// divide_mix(int x, int n)
// {
//     return x / n;
// }

#endif /* threads/fixed-point */