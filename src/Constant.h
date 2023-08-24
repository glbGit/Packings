#pragma once

#include "pch.h"

#define _WINDOWS
#define _USE_MATH_DEFINES

//---------------------------------------------------------------------------------------------------------------
// Macros

#define cutoff 1.e-15
#define Pi 3.1415926535897932384626433
#define u_rand ( (double) rand() / RAND_MAX )  // Uniform-distributed random number in [0,1].
#define to_cstr(val) std::to_string(val).c_str()

//---------------------------------------------------------------------------------------------------------------
// Types

using ULL = unsigned long long;
using UL = unsigned long;

//---------------------------------------------------------------------------------------------------------------
// Constants

namespace Constant
{
    /* Box*/
    static constexpr double      L = 10;                            // Box size 
    static constexpr double      V = L * L * L;                     // Box volume
    static constexpr double      L_H = L / 2;                       // Box half side
    static constexpr double      vol_c = Pi / 6. / V; 

    /* Packing */
    static const UL              N = 1000;                          // Number of particles
    static const ULL             NumOfPairs = ( N * (N - 1) ) / 2;  // Number of pairs
    static const UL              MAX_STEPS = 1000;                  // Max iterations
    static const double          T = 0.04;                          // Temperature
    static const double          PSwap = 0.2;                       // Swap probability
    static const double          delta_r = 0.05;                    // Max side displacement 
    static const double          phi_max = 0.2;                     // Max packing fraction        
    static const bool            InitialRelaxation = false;         // If true, perform a very brief relaxation in a soft repulsive potential. 

    /* Diameter distribution */
    static const double          Dmin = 0.230;                         // Min diameter
    static const double          Dmax = 0.642;                         // Max diameter
    static const double          A = 2*(Dmax*Dmax*Dmin*Dmin/(Dmax*Dmax-Dmin*Dmin)); 
    static const double          mu = 0.384812144;                                              
    static const double          sigma = 0.11809049;                                                       

    /* Growth */ 
    static const double          a0 = delta_r * 0.1;              
    static const double          a_min = a0 * Dmin;
    static const double          a_max = a0 * Dmax;

    /* Potential */
    static const double          b = 25; 
 
}

namespace Utils
{
    enum class On
    {
        Begin,
        End
    };

    /**
     * @brief Print date and time together with a message.
     * @param 0 Exit message; 
     * @param 1(any_integer) Welcome message. 
     */
    static void Time( On _time )
    {
        /* Time */
        char time_s[26];
        time_t rawtime;
	    struct tm timeinfo;
	    time(&rawtime);
#ifdef _LINUX
	    localtime_r( &rawtime, &timeinfo );
        asctime_r( &timeinfo, time_s );
#endif
#ifdef _WINDOWS
	    localtime_s( &timeinfo, &rawtime );
        asctime_s( time_s, &timeinfo );
#endif
        switch ( _time )
        {
        case On::Begin: 
            printf( "Running program...    %s\n", time_s );
            break;

        case On::End: 
            printf( "\nExiting program...    %s\n", time_s );
            break;

        default:
            break;
        }

    }

    /**
     * @brief Handle exception and exit.
     * @param Code Error identifier;
     * @param Message Message to display.
     */
    static void Exit( int Code, std::vector<const char *> Message )
    {
        printf( "\nError(%x): ", Code );
        for ( auto Block : Message ) printf( Block );
        printf("\n");
        exit( Code ); 
    }

}

//---------------------------------------------------------------------------------------------------------------
// Flags

#define NULL_PTR                0x01
#define DIR_NOT_FOUND           0x02
#define EMPTY_LINE              0x06
#define WRONG_INPUT_FORMAT      0x03
#define INSUFFICIENT_DATA       0x04
#define INVALID_INTERACTION     0x05
#define STEPS_OVERFLOW          0xf0
#define NO_INDEX                -99
