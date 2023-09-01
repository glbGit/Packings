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

void Box::MakeNeighbors()
{
    for ( int l = 0; l < NumOfSectors; l++ )
    { 
        // Assign coordinates to sectors
        int residue = l;
        for (int k = 0; k < D; k++)
        {
            int den = Packings::iPow( SectorsPerSide, D - k - 1 );
            s[l].coord.x[k] = residue / den;
            residue = residue % den;
        }

        // Neighbors
        Vector<int, D> v, w;
        for (int k = 0; k < D; k++)
            w.x[k] = 1;

        for (int i = 0; i < NumOfNeighborSectors; i++)
        {
            for (int k = 0; k < D; k++)
            {
                v.x[k] = s[l].coord.x[k];
                v.x[k] += SectorsPerSide + w.x[k];
                v.x[k] %= SectorsPerSide;
            }
            s[l].neighbor_list.push_back( FindSector(v) );
            if ( Decrease( D - 1, w ) ) Packings::Exit( SECTOR_FAIL, { "Something went wrong while defining neighbors." } );
        }

    }
}

int Box::Decrease( int Layer, Vector<int, D> & SectorCoord )
{
    int top = 1, bottom = -1;
    if ( Layer < 0 || Layer >= D )
        return 1;
    SectorCoord.x[ Layer ] -= 1;
    if ( SectorCoord.x[ Layer ] < bottom )
    {
        SectorCoord.x[ Layer ] = top;
        return Decrease( Layer - 1, SectorCoord );
    }
    return 0;
}

int Box::FindSector( Vector<int, D> & SectorCoord )
{
    int n = 0;
    for ( int k = 0; k < D; k++ )
    {
        int c = 1;
        for ( int l = k; l < D - 1; l++ )
            c = c * SectorsPerSide;
        n = n + SectorCoord.x[k] * c;
    }
    return n;
}