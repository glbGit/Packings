#pragma once

#include "pch.h"
#include "Vector.h"

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
        int x[ Constant::D ];
        std::vector<int> neighbor_list;
        std::list<int> member_list;
    };

    int             FindSector( int x, int y, int z );

    Sector *        s;

};

namespace BoundaryConditions
{
    namespace Periodic
    {
        double Image(double);
        double Distance_sq(double, double);
        double Distance_sq(Point &, Point &);
        double Distance_sq(Vector &, Vector &);
        double Distance(Vector &, Vector &);
        Vector Relative(Vector, Vector);
    }

    namespace Wall
    {
    }

}