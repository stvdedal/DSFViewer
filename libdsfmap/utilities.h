#ifndef UTILITIES_H_
#define UTILITIES_H_

template<typename T>
T mapval(T x, T x1, T x2, T v1, T v2)
{
    T dx = x2 - x1;
    T dv = v2 - v1;
    T d = dv / dx;
    return ((x - x1) * d) + v1;
}

#endif