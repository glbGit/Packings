#pragma once

#include "pch.h"
#include "Vector.h"

namespace Box
{
    namespace Variables
    {
        extern float L;                     // Box size
        extern float V;                     // Box volume
    }

    namespace BoundaryConditions
    {
        namespace Periodic 
        {
            double Image( double );
            double Distance_sq( double, double );
            double Distance_sq( Point &, Point & );
            double Distance_sq( Vector &, Vector & );
            double Distance( Vector &, Vector & );
            Vector Relative( Vector, Vector );
        }

        namespace Wall
        {
        }

    }

    class Sector
    {
    public:
        Sector();
        ~Sector();
        std::string toString();
        void MakeNeighbors();
        static int Find( int, int, int );
        static constexpr float s_length = 2;                      // Sector length (must be greater than sigma_max at any time).
        static const int _S_ = (int) ( Constant::L / s_length );          // Sector per side.
        static const unsigned N_S = _S_ * _S_ * _S_;     
        int x;
        int y;
        int z;
        std::vector<int> neighbor_list;
        std::list<int> member_list;
    };

}
