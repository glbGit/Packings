#pragma once

#include "pch.h"

#define _WINDOWS
#define _USE_MATH_DEFINES

//---------------------------------------------------------------------------------------------------------------
// Macros

#define u_rand ( (double) rand() / RAND_MAX )                  // Uniform-distributed random number in [0,1].
#define u_rand_in(A, B) ( u_rand * (B - A) + A )               // Uniform-distributed random number in [A,B].
#define to_cstr(val) std::to_string(val).c_str()

//---------------------------------------------------------------------------------------------------------------
// Functions


namespace Packings
{
    enum class On
    {
        Begin,
        End,
        Start,
        Exit
    };
    
    int iPow( int base, int exponent );
    unsigned long long Factorial( unsigned long long n );
    void Time( On _time );
    void Exit( int Code, std::vector<const char *> Message );
}