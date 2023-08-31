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

double BoundaryConditions::Periodic::Distance_sq( Vector<double, D> & a, Vector<double, D> & b )
{
    Vector<double, D> Diff = a - b;
    for( int i = 0; i < D; i++ )
        Diff.x[i] = Image( Diff.x[i] );
    
    return Diff.MagSq();
}

double BoundaryConditions::Periodic::Distance( Vector<double, D> & a, Vector<double, D> & b )
{
    Vector<double, D> Diff = a - b;
    for (int i = 0; i < D; i++)
        Diff.x[i] = Image(Diff.x[i]);

    return Diff.Mag();
}

Vector<double, D> BoundaryConditions::Periodic::Relative( Vector<double, D> a, Vector<double, D> b ) // returns periodic Vector (a-b)
{
    Vector<double, D> Diff = a - b;
    for (int i = 0; i < D; i++)
        Diff.x[i] = Image(Diff.x[i]);

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

void Box::Sector::MakeNeighbors()
{
}

int Box::Decrease( int Layer, Vector<int, D> & SectorCoord )
{
    int top = 1, bottom = -1;
    if ( Layer < 0 || Layer >= D )
        return 1;
    SectorCoord.x[ Layer ] -= 1;
    if (SectorCoord.x[Layer] < bottom)
    {
        SectorCoord.x[Layer] = top;
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

/**************************************************************

static void make_sector_neighbors(sector *s)
{
    vector_int v, w;
    for (int k = 0; k < D; k++)
        w.x[k] = 1;

    for (int i = 0; i < b.sector_number_of_neighbors; i++)
    {
        for (int k = 0; k < D; k++)
        {
            v.x[k] = s->x[k];
            v.x[k] += b.sectors_per_side + w.x[k];
            v.x[k] %= b.sectors_per_side;
        }
        s->neighbor[i] = find_sector(&v);
        if (decrease(D - 1, &w))
        {
            printf("Error (sector): Something went wrong while defining neighbors.\n");
            exit(1);
        }
    }
}

static void make_sectors()
{
    for (int i = 0; i < b.number_of_sectors; i++)
    {
        b.s[i].m = malloc(sizeof(int) * b.sector_max_members);
        b.s[i].neighbor = malloc(sizeof(int) * b.sector_number_of_neighbors);
        int residue = i;
        for (int k = 0; k < D; k++)
        {
            int den = _ipow(b.sectors_per_side, D - k - 1);
            b.s[i].x[k] = residue / den;
            residue = residue % den;
        }
        b.s[i].buffer = 0;
        for (int l = 0; l < b.sector_max_members; l++)
            b.s[i].m[l] = empty_slot;
        make_sector_neighbors(&b.s[i]);
    }
}

***********************************************************/