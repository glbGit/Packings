#include "Box.h"

using namespace Constant;

double BoundaryConditions::Periodic::Image( double x )
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

double BoundaryConditions::Periodic::Distance_sq( double x, double y )
{
    double d = x - y;
    if ( d > L_H )
        d = d - L;
    else if ( d < -L_H )
        d = d + L;
    return d * d;
}

double BoundaryConditions::Periodic::Distance_sq( Point<D> & a, Point<D> & b )
{
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    double dz = b.z - a.z;
    if ( dx > L_H )
        dx = dx - L;
    else if ( dx < -L_H )
        dx = dx + L;
    if ( dy > L_H )
        dy = dy - L;
    else if ( dy < -L_H )
        dy = dy + L;
    if ( dz > L_H )
        dz = dz - L;
    else if ( dz < -L_H )
        dz = dz + L;
    return dx * dx + dy * dy + dz * dz;
}

double BoundaryConditions::Periodic::Distance_sq( Vector<double, D> & a, Vector<double, D> & b )
{
    double dx = a.m_x - b.m_x;
    double dy = a.m_y - b.m_y;
    double dz = a.m_z - b.m_z;
    if ( dx > L_H )
        dx = dx - L;
    else if ( dx < -L_H )
        dx = dx + L;
    if ( dy > L_H )
        dy = dy - L;
    else if ( dy < -L_H )
        dy = dy + L;
    if ( dz > L_H )
        dz = dz - L;
    else if ( dz < -L_H )
        dz = dz + L;
    return dx * dx + dy * dy + dz * dz;
}

double BoundaryConditions::Periodic::Distance( Vector<double, D> & a, Vector<double, D> & b )
{
    double dx = a.m_x - b.m_x;
    double dy = a.m_y - b.m_y;
    double dz = a.m_z - b.m_z;
    if ( dx > L_H )
        dx = dx - L;
    else if ( dx < -L_H )
        dx = dx + L;
    if ( dy > L_H )
        dy = dy - L;
    else if ( dy < -L_H )
        dy = dy + L;
    if ( dz > L_H )
        dz = dz - L;
    else if ( dz < -L_H )
        dz = dz + L;
    return sqrt( dx * dx + dy * dy + dz * dz );
}

Vector<double, D> BoundaryConditions::Periodic::Relative( Vector<double, D> a, Vector<double, D> b ) // returns periodic Vector (a-b)
{
    double x = a.m_x - b.m_x;
    double y = a.m_y - b.m_y;
    double z = a.m_z - b.m_z;
    if ( x > L_H )
        while ( x > L_H )
            x -= L;
    else if ( x < -L_H ) 
        while ( x < -L_H )
            x += L;
    if ( y > L_H )
        while ( y > L_H )
            y -= L;
    else if ( y < -L_H )
        while ( y < -L_H )
            y += L;
    if ( z > L_H )
        while ( z > L_H )
            z -= L;
    else if ( z < -L_H )
        while ( z < -L_H )
            z += L;
    return Vector{ x, y, z };
}

void Box::Sector::MakeNeighbors()
{
}

int Box::FindSector( int x, int y, int z )
{
    int n = 0;
    for ( int k = 0; k < D; k++ )
    {
        int c = 1;
        for ( int l = k; l < D - 1; l++ )
            c = c * SectorsPerSide;
        n = n + s.x[k] * c;
    }
    return n;
}

Box::Box( double SigmaMax ) :
    Size( L ),
    HalfSize ( 0.5 * L )
{
    Volume = pow( L, D );
    SectorsPerSide = (int ) ( L / SigmaMax );
    NumOfSectors = Packings::iPow( SectorsPerSide, D );
    SectorLength = (double ) L / SectorsPerSide;
    SectorLengthInverse = 1. / SectorLength;
    NumOfNeighborSectors = ( Packings::iPow( 3, D ) - 1 ) / 2;
    s = new Sector[ NumOfSectors ];

}

Box::~Box()
{
    delete[] s;
}

Box::Sector::Sector()
{
}
