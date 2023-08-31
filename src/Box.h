#pragma once

#include "pch.h"
#include "Vector.h"

using namespace Constant;

class Box
{

public:
    Box( double SigmaMax );
    ~Box();

    const double Size;
    const double HalfSize;
    double Volume;
    double SectorLength;
    double SectorLengthInverse;
    int NumOfSectors;
    int SectorsPerSide;
    int NumOfNeighborSectors;
    
    struct Sector
    {
        Sector();
        int x[D];
        std::vector<int> neighbor_list;
        std::list<int> member_list;
        void MakeNeighbors();
    };

    int             Decrease( int Layer, Vector<int, D> & SectorCoord );
    int             FindSector( Vector<int, D> & SectorCoord );

    Sector *        s;

};

namespace BoundaryConditions
{
    namespace Periodic
    {
        double Image( double );
        double Distance_sq( double, double );
        double Distance_sq( Vector<double, D> &, Vector<double, D> & );
        double Distance( Vector<double, D> &, Vector<double, D> & );
        Vector<double, D> Relative( Vector<double, D>, Vector<double, D> );
    }

    namespace Wall
    {
    }

}