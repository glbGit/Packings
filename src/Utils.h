#pragma once

#include "pch.h"

#define _USE_MATH_DEFINES

//---------------------------------------------------------------------------------------------
// Macros

#define RAND ( (double )rand() / RAND_MAX )                             // Uniform-distributed random number in [0,1].
#define RAND_IN(A, B) ( RAND * ((B) - (A)) + (A) )                      // Uniform-distributed random number in [A,B].

#define SQUARE(x) ( (x) * (x) )                                         // Square
#define ABS(x) ( (x) < 0 ? (x) * (-1) : (x) )                           // Absolute value
#define SGN(x) ( (x) < 0 ? -1 : 1 )                                     // Sign function
#define HVSD(x) ( (x) < 0 ? 0 : 1 )                                     // Heaviside function

#define to_cstr(val) std::to_string(val).c_str()

//---------------------------------------------------------------------------------------------
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
    void Exit( int code, std::vector<const char *> message );
}

// TODO
// Make assert functions
// Move math functions into Math.h 

