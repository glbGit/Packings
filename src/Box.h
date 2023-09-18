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
    int                 numOfSectors;
    int                 sectorsPerSide;
    int                 numOfNeighborSectors;

};



namespace BoundaryConditions
{
    namespace Periodic
    {
        double Image( double );
        double Distance( double, double );
        double Distance_sq( double, double );
        double Distance_sq( Vector<double, D> &, Vector<double, D> & );
        double Distance( Vector<double, D> &, Vector<double, D> & );
        Vector<double, D> Relative( Vector<double, D>, Vector<double, D> );
    }

    namespace Wall
    {
    }

}

//-----------------------------------------------------------------------------
// Inline functions

inline double BoundaryConditions::Periodic::Image( double x )
{
    if ( x >= 0 && x < L )
        return x;
    else
    {
        if ( x < 0 )
        {
            do
            {
                x += L;
            } while ( x < 0 );
            return x;
        }
        else
        {
            do
            {
                x -= L;
            } while ( x > L );
            return x;
        }
    }
}

inline double BoundaryConditions::Periodic::Distance( double x, double y )
{
    double d = x - y;
    if ( d > L_H )
        d = d - L;
    else if ( d < -L_H )
        d = d + L;
    return d;
}

inline double BoundaryConditions::Periodic::Distance_sq( double x, double y )
{
    double d = Distance( x, y );
    return d * d;
}

inline double BoundaryConditions::Periodic::Distance_sq( Vector<double, D> & a, Vector<double, D> & b )
{
    double Sum = 0;
    for( int i = 0; i < D; i++ )
       Sum = Sum + Distance_sq( a.x[i], b.x[i] );
    
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
       Diff.x[i] = Distance( a.x[i], b.x[i] );

    return Diff;
}