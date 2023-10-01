#include "Box.h"

using namespace Constant;

Box::Box( double SigmaMax ) :
    Size( L ),
    HalfSize ( 0.5 * L )
{
    Volume = pow( L, D );
    sectorsPerSide = static_cast<int>( L / SigmaMax );
    numSectors = Packings::iPow( sectorsPerSide, D );
    SectorLength = static_cast<double>(L) / sectorsPerSide;
    SectorLengthInverse = 1. / SectorLength;
    numNeighborSectors = ( Packings::iPow( 3, D ) - 1 ) / 2;
    s = new Sector[ numSectors ];

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
    for ( int l = 0; l < numSectors; l++ )
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

        for (int i = 0; i < numNeighborSectors; i++)
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

int Box::Decrease( int layer, Vector<int, D> & sectorCoord )
{
    int top = 1, bottom = -1;
    if ( layer < 0 || layer >= D )
        return 1;
    sectorCoord.x[ layer ] -= 1;
    if ( sectorCoord.x[ layer ] < bottom )
    {
        sectorCoord.x[ layer ] = top;
        return Decrease( layer - 1, sectorCoord );
    }
    return 0;
}

int Box::FindSector( Vector<int, D> & sectorCoord )
{
    int n = 0;
    for ( int k = 0; k < D; k++ )
    {
        int c = 1;
        for ( int l = k; l < D - 1; l++ )
            c = c * sectorsPerSide;
        n = n + sectorCoord.x[k] * c;
    }
    return n;
}

