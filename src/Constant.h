#pragma once

#include "pch.h"

//---------------------------------------------------------------------------------------------------------------
// Types

using ULL = unsigned long long;
using UL = unsigned long;

//---------------------------------------------------------------------------------------------------------------
// Constants

namespace Constant
{
    /* General */
    static constexpr int         D = 3;                                 // Dimensions 
    static constexpr double      Epsilon = 1.e-15;                      // Precision
    static constexpr double      Pi = 3.1415926535897932384626433;      // Pi

    /* Box */
    static constexpr double      L = 10;                                // Box size 
    static constexpr double      L_H = L / 2;                           // Box half side

    /* Packing */
    static constexpr UL          N = 1000;                              // Number of particles
    static constexpr ULL         NumOfPairs = ( N * (N - 1) ) / 2;      // Number of pairs
    static constexpr UL          MaxSteps = 1000;                       // Max iterations
    static constexpr double      T = 0.04;                              // Temperature
    static constexpr double      SwapChance = 0.2;                           // Swap probability
    static constexpr double      Delta_r = 0.05;                        // Max side displacement 
    static constexpr double      Phi_0 = 0.1;                           // Initial packing fraction
    static constexpr double      Phi_t = 0.7;                           // Target packing fraction
    static const bool            InitialRelaxation = false;             // If true, perform a very brief relaxation in a soft repulsive potential. 

    /* Diameter distribution */
    static constexpr int         DType = 2;                              // Type
    static constexpr double      DParameters[] = { 3 };                  // Parameters
    static constexpr double      Ratio = 0.45;                           // SigmaMin/SigmaMax

    /* Growth */ 
    static const double          a0 = Delta_r * 0.1;          

    /* Potential */
    static const double          b = 25; 
 
}

//---------------------------------------------------------------------------------------------------------------
// Flags

#define NULL_PTR                            0x01
#define DIR_NOT_FOUND                       0x02
#define EMPTY_LINE                          0x06
#define WRONG_INPUT_FORMAT                  0x03
#define INSUFFICIENT_DATA                   0x04
#define INVALID_INTERACTION                 0x05
#define INVALID_PARAMETER                   0x07
#define UNDEFINED_DISTRIBUTION              0x08
#define SECTOR_FAIL                         0x09
#define STEPS_OVERFLOW                      0xf0
#define RELOCATION_OVERFLOW                 0xf1
#define NO_INDEX                            -99

