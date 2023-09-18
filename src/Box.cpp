#include "Box.h"

using namespace Constant;

Box::Box( double SigmaMax ) :
    Size( L ),
    HalfSize ( 0.5 * L )
{
    Volume = pow( L, D );
    sectorsPerSide = (int ) ( L / SigmaMax );
    numOfSectors = Packings::iPow( sectorsPerSide, D );
    SectorLength = (double ) L / sectorsPerSide;
    SectorLengthInverse = 1. / SectorLength;
    numOfNeighborSectors = ( Packings::iPow( 3, D ) - 1 ) / 2;
    s = new Sector[ numOfSectors ];

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
    for ( int l = 0; l < numOfSectors; l++ )
    { 
        // Assign coordinates to sectors
        int residue = l;
        for (int k = 0; k < D; k++)
        {
            int den = Packings::iPow( sectorsPerSide, D - k - 1 );
            s[l].coord.x[k] = residue / den;
            residue = residue % den;
        }

        // Neighbors
        Vector<int, D> v, w;
        for (int k = 0; k < D; k++)
            w.x[k] = 1;

        for (int i = 0; i < numOfNeighborSectors; i++)
        {
            for (int k = 0; k < D; k++)
            {
                v.x[k] = s[l].coord.x[k];
                v.x[k] += sectorsPerSide + w.x[k];
                v.x[k] %= sectorsPerSide;
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
            c = c * sectorsPerSide;
        n = n + SectorCoord.x[k] * c;
    }
    return n;
}

