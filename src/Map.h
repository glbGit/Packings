#pragma once

#include "pch.h"
#include "Particle.h"

using namespace Packings;

template <class T1, class T2>
struct Pair
{
    T1 i;
    T2 j;
};

template <class T_k, class T_v>
struct HashEntry
{
    T_k key;
    T_v value;
};

template<size_t size>
ULL Hash( int _i, int _j )
{
    ULL i = static_cast<ULL>(_i);
    ULL j = static_cast<ULL>(_j);
    if ( i == j || i >= size || j >= size || i < 0 || j < 0 ) 
        Exit( INVALID_INTERACTION, { "No interaction (", to_cstr(i), ",", to_cstr(j), ") exists." } );
    else if ( i > j ) 
    {
        i = _j;
        j = _i;
    }
    return j - i * ( i + 3 ) / 2 + i * size - 1; 
}