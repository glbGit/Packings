#pragma once

#include "pch.h"
#include "Vector.h"

using namespace Constant;

class Box
{

public:
    Box( double SigmaMax );
    ~Box();
    
    struct Sector
    {
        Sector();
        Vector<int, D> coord;
        std::vector<int> neighbor_list;
        std::list<int> member_list;
    };
    
    void                MakeNeighbors();
    int                 Decrease( int Layer, Vector<int, D> & SectorCoord );
    int                 FindSector( Vector<int, D> & SectorCoord );

    Sector *            s;
    const double        Size;
    const double        HalfSize;
    double              Volume;
    double              SectorLength;
    double              SectorLengthInverse;
    int                 numSectors;
    int                 sectorsPerSide;
    int                 numNeighborSectors;

};

namespace BoundaryConditions
{
    namespace Periodic
    {
        double Distance_sq( Vector<double, D> &, Vector<double, D> & );
        double Distance( Vector<double, D> &, Vector<double, D> & );
        Vector<double, D> Relative( Vector<double, D>, Vector<double, D> );
    }

    namespace Wall
    {
    }

}

//---------------------------------------------------------------------------------------------
// Macros

#define IMAGE(x) ( (x) + ( HVSD(-(x)) - (int )( (x) / L ) ) * L )

#define DIST(x) ( \
\
    ( (x) > L_H )  ? \
    ABS( (x) - L ) : \
    ( (x) < -L_H ) ? \
    ABS( (x) + L ) : \
    ABS(x) )

#define DIST_SQ(x) ( \
\
    ( (x) > L_H )  ? \
    SQUARE( (x) - L ) : \
    ( (x) < -L_H ) ? \
    SQUARE( (x) + L ) : \
    SQUARE(x) )

//---------------------------------------------------------------------------------------------
// Inline functions

inline double BoundaryConditions::Periodic::Distance_sq( Vector<double, D> & a, Vector<double, D> & b )
{
    double Sum = 0;
    for( int i = 0; i < D; i++ )
       Sum = Sum + DIST_SQ( a.x[i] - b.x[i] );
    
    return Sum;
}

inline double BoundaryConditions::Periodic::Distance( Vector<double, D> & a, Vector<double, D> & b )
{
    return sqrt( Distance_sq( a, b ) );
}

inline Vector<double, D> BoundaryConditions::Periodic::Relative( Vector<double, D> a, Vector<double, D> b ) 
{
    Vector<double, D> Diff;
    for( int i = 0; i < D; i++ )
       Diff.x[i] = DIST( a.x[i] - b.x[i] );

    return Diff;
}