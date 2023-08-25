#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "Packing.h"
#include "Vector.h"

using namespace Box;
using namespace Box::BoundaryConditions::Periodic;
using namespace System;
using namespace System::Variables;
using namespace System::Interaction;
using namespace Distribution;
using namespace Constant;
using namespace Packings;

//----------------------------------------------------------------------------------------------------------

Packing::Packing() :
    energy(0),
    phi(0)
{
    s = new Sector[ Sector::N_S ];
    for ( int i = 0; i < Sector::N_S; i++ ) {
        s[i].x = i / ( Sector::_S_ * Sector::_S_ );
        s[i].y = ( i % ( Sector::_S_ * Sector::_S_ ) ) / Sector::_S_;
        s[i].z = i % Sector::_S_; 
        s[i].MakeNeighbors();
    }
    g = new double[N];
    p = new Particle[N];
    c = new Local[ (N * (N - 1) ) / 2 ];
    for ( int i = 0; i < N; i++ ) {
        p[i] = Particle{ NO_INDEX, Vector{}, Vector{}, 0 };
        g[i] = 2;
    }
}

Packing::~Packing() 
{
    delete[] g;
    delete[] p;
    delete[] c;
    delete[] s;
}

void Packing::Initialize()
{
    sigma_min = Distribution::GetMin( this->g );
    sigma_max = Distribution::GetMax( this->g );
    growth_min = sigma_min * a0;
    growth_max = sigma_max * a0;
    distribution_type = GetDistributionInfo().type;
}

void Packing::Make()
{   
    printf( "Adding %lu particles to the system..    ", N );
    Distribution::Info info = GetDistributionInfo();

    for ( int i = 0; i < N; i++ ) 
    {
        if ( this->p[i].id == NO_INDEX ) 
        {
            this->p[i].diameter = RandomDiameter( info );
            int count = 0;
		    do 
            {
                count++; 
                if ( count > N ) Exit( RELOCATION_OVERFLOW, { "Failed to place particle ", to_cstr(i), "." } );
			    this->p[i].position.setRandomComponents( 0, L );
		    } while ( this->DoesOverlap( i, i ) );

            int x = static_cast<int>( this->p[i].position.m_x / Sector::s_length );
            int y = static_cast<int>( this->p[i].position.m_y / Sector::s_length );
            int z = static_cast<int>( this->p[i].position.m_z / Sector::s_length );
            int l = Sector::Find( x, y, z );
            this->s[l].member_list.push_back(i);
            this->p[i].sector = l;
            this->p[i].id = i;
            this->g[i] = a0 * this->p[i].diameter;
        }
	}

    printf( "Particles successfully added.\n" );
    this->Initialize();
    this->FullUpdate();
}

void Packing::Make( const char * filename )
{   
    int entries = 0;
    char line[256];
	char _filename[256];
	sprintf( _filename, "data/%s", filename );
    FILE * stream;
    stream = fopen( _filename, "r" );
    if ( stream != NULL ) 
    {
        printf( "Copying system configuration from file \"%s\"..    ", filename );
        for ( int i = 0; i < N; i++ ) 
        {
            int result = fscanf( stream, "%[^\n]", line );
            if ( result != 0 )
            {
                if ( line[0] == '\0' ) Exit( INSUFFICIENT_DATA, { "Too few entries (", to_cstr(i), "/", to_cstr(N), ")." } );
	            entries = sscanf( line, "%lg %lg %lg %lg", &this->p[i].position.m_x, &this->p[i].position.m_y, &this->p[i].position.m_z, &this->p[i].diameter );
                this->p[i].diameter = this->p[i].diameter + this->p[i].diameter;
	            fgetc( stream );
                line[0] = '\0';
                if ( entries != 4 ) Exit( WRONG_INPUT_FORMAT, { "Entry format must be [x] [y] [z] [rad]." } );

                int x = static_cast<int>( this->p[i].position.m_x / Sector::s_length );
                int y = static_cast<int>( this->p[i].position.m_y / Sector::s_length );
                int z = static_cast<int>( this->p[i].position.m_z / Sector::s_length );
                int l = Sector::Find( x, y, z );
                this->s[l].member_list.push_back(i);
                this->p[i].sector = l;
                this->p[i].id = i;
                this->g[i] = a0 * this->p[i].diameter;
            } else Exit( EMPTY_LINE, { "Empty line (", to_cstr( i + 1 ), ")." } );
	    }
        fclose( stream );
    } else Exit( DIR_NOT_FOUND, { "File \"", filename, "\" not found." } );
    printf( "Particles successfully added.\n" );
    this->FullUpdate();
}

void Packing::PrintToFile() 
{
    FILE * out;
    out = fopen( "data/output.dat", "w+" );
    if ( out != NULL )
    {
        for ( int i = 0; i < N; i++ )
            fprintf( out, "%.16f %.16f %.16f %.16f\n", p[i].position.m_x, p[i].position.m_y, p[i].position.m_z, 0.5 * p[i].diameter );
        fclose( out );
    } else Exit( DIR_NOT_FOUND, { "Destination folder \"/data\" not found." } );
}

void Packing::PrintToFile( const char * filename ) 
{
    FILE * out;
	char _filename[256];
	sprintf( _filename, "data/%s", filename );
    out = fopen( _filename, "w+" );
    if ( out != NULL ) 
    {
        for ( int i = 0; i < N; i++ )
            fprintf( out, "%.16f %.16f %.16f %.16f\n", p[i].position.m_x, p[i].position.m_y, p[i].position.m_z, 0.5 * p[i].diameter );
        fclose( out );
    } else Exit( DIR_NOT_FOUND, { "Destination folder \"/data\" not found." } ); 
}

void Packing::PrintToFile( FILE * f, int frame ) 
{
    if ( f != NULL )
    {
        for ( int i = 0; i < N; i++ )
            fprintf( f, "%.16f %.16f %.16f %.16f %d\n", p[i].position.m_x, p[i].position.m_y, p[i].position.m_z, 0.5 * p[i].diameter, frame );
    }
    else Exit( NULL_PTR, { "Null pointer." } );
}

/* Prints diameter distribution. */
void Packing::PrintDiameterDistribution()
{ 
    FILE * file = fopen( "data/diameter_distribution.dat", "w+" );
    if ( file != NULL )
    {
        int count;
	    for ( double q = sigma_min; q <= sigma_max; q += 0.01 ) {      
		    count = 0;
		    for ( int i = 0; i < N; i++ ) 
                if ( this->p[i].diameter >= q && this->p[i].diameter < q + 0.01 ) count++;
		    fprintf( file, "%g %d\n", q, count );
	    }
	    fclose( file );
    } else Exit( DIR_NOT_FOUND, { "Destination folder \"/data\" not found." } );
}

/* Prints particles first neighbors */
void Packing::PrintFirstNeighbors()
{
	FILE * out = fopen( "data/first_neighbors.dat", "w" );
    if ( out != NULL ) 
    {for ( int i = 0; i < N; i++ )
	{
		fprintf( out, "p%d: ", i );
		for ( auto k : this->p[i].first_neighbors )
		{
			fprintf( out, "%d ", k );
		}
		fprintf( out, "\n" );
	}
	fclose( out );
    } else Exit( DIR_NOT_FOUND, { "Destination folder \"/data\" not found." } );
}

/* Prints system observables. */
void Packing::PrintSystemInfo( int mcs )
{
	printf( 
        "\nSystem info:\n"
        "E_tot = %g\n"
        "Overlaps = %lu\n"
        "N = %lu\n"
        "Monte Carlo steps = %d\n"
        "Volume fraction = %.6f\n"
        "Diameter = [%.6f, %.6f]\n"
        "\n",
        this->energy, static_cast<UL>( this->overlap_list.size() ), N, mcs, this->phi, sigma_min, sigma_max
    );
    if ( sigma_max > Sector::s_length ) printf( "WARNING: Maximum diameter exceeding sector length! (May generate overlaps.)\n" );
}

inline double Packing::VolumeFraction()
{
	double phi = 0;
	for ( int i = 0; i < N; i++ ) phi += vol_c * p[i].diameter * p[i].diameter * p[i].diameter;
	return phi;
}

inline int Packing::Overlaps() {  return static_cast<int>( this->overlap_list.size() );  }

double Packing::Energy()
{
	double energy = 0;
	for ( int i = 0; i < Constant::NumOfPairs; i++ ) 
        energy = energy + this->c[i].energy;
	return energy;
}

/* Swap two diameters. */ 
void Packing::Swap( int i, int j )
{
    double temp;
    temp = this->p[i].diameter;
    this->p[i].diameter = this->p[j].diameter;
    this->p[j].diameter = temp;
    temp = this->g[i];
    this->g[i] = this->g[j];
    this->g[j] = temp;
}

void Packing::FullUpdate()
{
    this->overlap_list.clear();

    int count = 0;
    for ( int i = 0; i < N; i++ ) {
        phi = phi + vol_c * p[i].diameter * p[i].diameter * p[i].diameter;
        this->p[i].first_neighbors.clear();                        
    }
        
    for ( int k = 0; k < Sector::N_S; k++ ) 
    {       /* This sector */
        for ( auto i : this->s[k].member_list )
        {
            for ( auto j : this->s[k].member_list )
            {
                if ( j > i )
                { 
                    double r_ij = Distance_sq( this->p[i].position, this->p[j].position );
                    double sigma_ij = 0.5 * ( this->p[i].diameter + this->p[j].diameter );
                    switch ( Packing::State( r_ij, sigma_ij ) )
                    {
                    case Interaction::State::Overlap:
                        this->overlap_list.push_back( Hash<N>( i, j ) );
                        this->c[ Hash<N>( i, j ) ].energy = 1.e16;
                        this->p[i].first_neighbors.push_back(j);
                        this->p[j].first_neighbors.push_back(i);
                        break;
                    case Interaction::State::Contact:
                        this->c[ Hash<N>( i, j ) ].energy = 1;
                        this->p[i].first_neighbors.push_back(j);
                        this->p[j].first_neighbors.push_back(i);
                        break;
                    case Interaction::State::FirstNeighbor:
                        this->c[ Hash<N>( i, j ) ].energy = exp( -b * ( sqrt(r_ij) - sigma_ij ) );
                        this->p[i].first_neighbors.push_back(j);
                        this->p[j].first_neighbors.push_back(i);
                        break;

                    default:
                        this->c[ Hash<N>( i, j ) ].energy = 0;
                        break;
                    }  
                }
            }
            /* Neighbor sectors */
            for ( auto _k : this->s[k].neighbor_list )
            {
                for ( auto j : this->s[_k].member_list )
                {
                    double r_ij = Distance_sq( this->p[i].position, this->p[j].position );
                    double sigma_ij = 0.5 * ( this->p[i].diameter + this->p[j].diameter ); 
                    switch ( Packing::State( r_ij, sigma_ij ) )
                    {
                    case Interaction::State::Overlap:
                        this->overlap_list.push_back( Hash<N>( i, j ) );
                        this->c[ Hash<N>( i, j ) ].energy = 1.e16;
                        this->p[i].first_neighbors.push_back(j);
                        this->p[j].first_neighbors.push_back(i);
                        break;
                    case Interaction::State::Contact:
                        this->c[ Hash<N>( i, j ) ].energy = 1;
                        this->p[i].first_neighbors.push_back(j);
                        this->p[j].first_neighbors.push_back(i);
                        break; 
                    case Interaction::State::FirstNeighbor:
                        this->c[ Hash<N>( i, j ) ].energy = exp( -b * ( sqrt(r_ij) - sigma_ij ) ); 
                        this->p[i].first_neighbors.push_back(j);
                        this->p[j].first_neighbors.push_back(i);
                        break;

                    default:
                        this->c[ Hash<N>( i, j ) ].energy = 0;
                        break;
                    }
                }
            }
        }
    }

    this->energy = this->Energy();
    this->phi = this->VolumeFraction();
}

void Packing::Update( System::State state )
{
    switch ( state )
    {
    case System::State::Compression:
    {
        for ( int i = 0; i < N; i++ )
        {
            for ( auto j : this->p[i].first_neighbors )
            {
                if ( j > i )
                {   
                    ULL id = Hash<N>( i, j );
                    double r_ij = Distance_sq( this->p[i].position, this->p[j].position );
                    double sigma_ij = 0.5 * ( this->p[i].diameter + this->p[j].diameter ); 
                    switch ( Packing::State( r_ij, sigma_ij ) )
                    {
                    case Interaction::State::Overlap:
                        this->overlap_list.push_back(id);
                        this->c[id].energy = 1.e16;
                        this->c[id].overlap = true;
                        break;
                    case Interaction::State::Contact:
                        this->c[id].energy = 1;
                        break;
                    case Interaction::State::FirstNeighbor:
                        this->c[id].energy = exp( -b * ( sqrt(r_ij) - sigma_ij ) );
                        break;

                    default:
                        break;
                    } 
                } 
            } 
        }
        break;
    }
    case System::State::Relaxation:
    {
        for ( int i = 0; i < N; i++ )  this->p[i].first_neighbors.clear();
        
        for ( int k = 0; k < Sector::N_S; k++ ) 
        {       /* This sector */
            for ( auto i : this->s[k].member_list )
            {
                for ( auto j : this->s[k].member_list )
                {
                    if ( j > i )
                    {
                        double r_ij = Distance_sq( this->p[i].position, this->p[j].position );
                        double sigma_ij = 0.5 * ( this->p[i].diameter + this->p[j].diameter ); 
                        switch ( Packing::State( r_ij, sigma_ij ) )
                        {
                        case Interaction::State::Free:
                            break;
                        default:
                            this->p[i].first_neighbors.push_back(j);
                            this->p[j].first_neighbors.push_back(i);
                            break;
                        }  
                    }
                }
                /* Neighbor sectors */
                for ( auto _k : this->s[k].neighbor_list )
                {
                    for ( auto j : this->s[_k].member_list ) 
                    {
                        double r_ij = Distance_sq( this->p[i].position, this->p[j].position );
                        double sigma_ij = 0.5 * ( this->p[i].diameter + this->p[j].diameter ); 
                        switch ( Packing::State( r_ij, sigma_ij ) )
                        {
                        case Interaction::State::Free:
                            break;

                        default:
                            this->p[i].first_neighbors.push_back(j);
                            this->p[j].first_neighbors.push_back(i);
                            break;
                        }  
                    }
                }
                
            }
        }
        break;
    } 
    default:
        break;
    }
}

double Packing::NewState_( int i, Vector & NewPosition, std::vector<System::Interaction::Local> & c_p, bool & flag )
{
    double de = 0;
    flag = true;
    for ( auto j : this->p[i].first_neighbors ) {
        ULL id = Hash<N>( i, j );
        double r_ij = Distance_sq( NewPosition, this->p[j].position );
        double sigma_ij = 0.5 * ( this->p[i].diameter + this->p[j].diameter ); 
        switch ( Packing::State( r_ij, sigma_ij ) )
        {
            case Interaction::State::Overlap:
                flag = false;
                return 1.e16;
            case Interaction::State::Contact:
                c_p.push_back( { id, 1 } );
                de = de + c_p.back().energy - this->c[id].energy;
                break;
            case Interaction::State::FirstNeighbor:
                c_p.push_back( { id, exp( -b * ( sqrt(r_ij) - sigma_ij ) ) } );
                de = de + c_p.back().energy - this->c[id].energy;
                break;
            case Interaction::State::Free:
                c_p.push_back( { id, 0 } );
                de = de - this->c[id].energy;
                break;
        }
    }
    return de;
}

void Packing::NewState( int i, Vector & NewPosition, std::vector<System::Interaction::Local> & c_p, bool & flag )
{
    flag = true;
    for ( auto j : this->p[i].first_neighbors)  
    {
        ULL id = Hash<N>( i, j );
        double r_ij = Distance_sq( NewPosition, this->p[j].position );
        double sigma_ij = 0.5 * ( this->p[i].diameter + this->p[j].diameter ); 
        switch ( Packing::State( r_ij, sigma_ij ) )
        {
            case Interaction::State::Overlap:
                flag = false;
                return;

            default:
                c_p.push_back( {id} );
                break;
        }
    }
}

void Packing::NewState( int i, double NewDiameter, std::vector<System::Interaction::Local> & c_p, bool & flag )
{
    flag = true;
    for ( auto k : this->p[i].first_neighbors ) {
        ULL id = Hash<N>( i, k );
        double r_ij = Distance_sq( this->p[i].position, this->p[k].position );
        double sigma_ij = 0.5 * ( NewDiameter + this->p[k].diameter ); 
        switch ( Packing::State( r_ij, sigma_ij ) )
        {
            case Interaction::State::Overlap:
                flag = false;
                return;

            default:
                c_p.push_back( {id} );
                break;
        }     
    }
    return;
}

/* Perform one Monte Carlo step, consisting of N elementary steps. */
int Packing::Step()
{
    int accept = 0;
    switch ( interaction_type ) 
    {
        case Potential::Hard:
        {      
            double _x, _y, _z, rnd;
            bool no_overlap;
            Vector _r;    
            std::vector<Local> c_p;                                                                     
            for ( int i = 0; i < N; i++ ) 
            {
                no_overlap = true;
                rnd = u_rand;
                if ( rnd < 1 - PSwap )                    /* Standard Monte Carlo move */
                { 
                    _x = Image( this->p[i].position.m_x + ( u_rand - 0.5 ) * delta_r );
                    _y = Image( this->p[i].position.m_y + ( u_rand - 0.5 ) * delta_r );
                    _z = Image( this->p[i].position.m_z + ( u_rand - 0.5 ) * delta_r );
                    _r.Set( _x, _y, _z );
                    this->NewState( i, _r, c_p, no_overlap );
                    if ( no_overlap ) 
                    {
                        accept++;
                        this->p[i].position.Set( _x, _y, _z );
                        /* Sync overlaps */                           
                        for ( auto & l : c_p ) 
                        {
                            if ( this->c[l.id].overlap ) 
                            {
                                this->c[l.id].overlap = false;
                                this->overlap_list.remove(l.id);
                            }
                        }
                        /* Sync sectors */
                        int x = static_cast<int>( this->p[i].position.m_x / Sector::s_length );
                        int y = static_cast<int>( this->p[i].position.m_y / Sector::s_length );
                        int z = static_cast<int>( this->p[i].position.m_z / Sector::s_length );
                        int new_sector = Sector::Find( x, y, z );
                        if ( new_sector != this->p[i].sector ) 
                        {
                            this->s[ this->p[i].sector ].member_list.remove(i);
                            this->s[ new_sector ].member_list.push_back(i);
                            this->p[i].sector = new_sector;
                        }
                    } 
                } else                                  /* Swap */
                {
                    int j = rand() % N;
                    if ( this->p[j].diameter > this->p[i].diameter ) this->NewState( i, this->p[j].diameter, c_p, no_overlap );
                    else this->NewState( j, this->p[i].diameter, c_p, no_overlap );
                    if ( no_overlap ) 
                    {
                        accept++;
                        this->Swap( i, j );
                        /* Sync overlaps */                            
                        for ( auto & l : c_p ) 
                        {
                            if ( this->c[l.id].overlap ) 
                            {
                                this->c[l.id].overlap = false;
                                this->overlap_list.remove(l.id);
                            }
                        }
                    } 
                }
                c_p.clear(); 
            }
            break;
        }
        case Potential::SoftRepulsive: 
        {
            double _x, _y, _z, rnd, dE;
            bool no_overlap;
            Vector _r;
            std::vector<Local> c_p;
            for ( int i = 0; i < N; i++ ) 
            {
                no_overlap = true;
                _x = Image( this->p[i].position.m_x + ( u_rand - 0.5 ) * delta_r );
                _y = Image( this->p[i].position.m_y + ( u_rand - 0.5 ) * delta_r );
                _z = Image( this->p[i].position.m_z + ( u_rand - 0.5 ) * delta_r );
                _r.Set( _x, _y, _z );
                dE = this->NewState_( i, _r, c_p, no_overlap );
                rnd = u_rand;
                if ( rnd < exp( -dE / T ) && no_overlap ) 
                {
                    accept++;
                    this->p[i].position.Set( _x, _y, _z );
                    /* Sync energy and overlaps */ 
                    for ( auto & l : c_p ) 
                    {
                        this->c[l.id].energy = l.energy;
                        if ( this->c[l.id].overlap ) 
                        {
                            this->c[l.id].overlap = false;
                            this->overlap_list.remove(l.id);
                        }
                    }
                    /* Sync sector */
                    int x = static_cast<int>( _x / Sector::s_length );
                    int y = static_cast<int>( _y / Sector::s_length );
                    int z = static_cast<int>( _z / Sector::s_length );
                    int new_sector = Sector::Find( x, y, z );
                    if ( new_sector != this->p[i].sector ) 
                    {
                        this->s[ this->p[i].sector ].member_list.remove(i);
                        this->s[ new_sector ].member_list.push_back(i);
                        this->p[i].sector = new_sector;
                    }
                }
                c_p.clear();
            }
            break;
        }
        default:
            break;
    }
    return accept;
}

/* Compress the system by growing particles. */
void Packing::Compress( int & steps )
{
    printf( "Compression in progress..    " ); 
    while ( this->phi < Phi_t ) 
    {
        int count = 0, accept = 0;
        /* Compress */
        for ( int i = 0; i < N; i++ ) 
        {
            this->p[i].diameter = this->p[i].diameter + this->g[i];
        }
        this->Update( System::State::Compression );
        this->phi = this->VolumeFraction();
        printf( "Overlaps %lu, phi = %.6f", static_cast<int>( this->overlap_list.size() ), this->phi ); 
        while ( this->overlap_list.size() > 0 ) 
        {
            accept = this->Step();
            this->Update( System::State::Relaxation );
            count++;
            if ( count > MaxSteps ) 
            {  
                printf( " - System failed to compress.\n\n" );
                /* Decompress on exit */
                for ( int i = 0; i < N; i++ ) this->p[i].diameter = this->p[i].diameter - this->g[i];
                this->FullUpdate();
                this->PrintToFile( "output.dat" );
                this->PrintDiameterDistribution();
                this->PrintSystemInfo( steps + count );
                Time( On::End );
                exit( STEPS_OVERFLOW );
            }
        }
        printf( " - mcs = %d\n", count );
        sigma_min = sigma_min + growth_min;
        sigma_max = sigma_max + growth_max;
        steps  = steps + count;
    }  
    printf( "Completed.\n" ); 
}

/* Return the interaction state between two given particles. */
Interaction::State Packing::State( double r_sq, double sigma )
{
    double r_cut_sq;
    if ( interaction_type == Potential::Hard ) r_cut_sq = sigma_max + 2 * delta_r;   
    else r_cut_sq = 0.5 * sigma_max + sigma + 2 * delta_r;
    r_cut_sq = r_cut_sq * r_cut_sq;
	if ( r_sq > r_cut_sq ) return Interaction::State::Free;
    else
    {
        sigma = sigma * sigma;
        if ( r_sq > sigma + epsilon )  return Interaction::State::FirstNeighbor;
        else if ( r_sq < sigma - epsilon ) return Interaction::State::Overlap;  
        else return Interaction::State::Contact;           
    } 
}

//----------------------------------------------------------------------------------------------------------------------------

bool Packing::DoesOverlap( int j, int n )
{
    double r_ij, sigma_ij;
	for ( int i = 0; i < n; i++ ) {
		if ( i != j ) {
            r_ij = Distance_sq( this->p[i].position, this->p[j].position );
            sigma_ij = ( this->p[i].diameter + this->p[j].diameter ) / 2;
			if ( Packing::State( r_ij, sigma_ij ) == Interaction::State::Overlap ) return true;
        }
	}
	return false;
}

bool Packing::Drift( int i )
{
    int count = 0;
	double d, mod, p, r_ij, sigma_ij;
	Vector rel, F0, F;
	F0 = 2 * L;
	do {
    count++;
    F = 0;
	for ( int j = 0; j < i; j++ ) {
        r_ij = Distance_sq( this->p[i].position, this->p[j].position );
        sigma_ij = ( this->p[i].diameter + this->p[j].diameter ) / 2;
		if ( Packing::State( r_ij, sigma_ij ) == Interaction::State::Overlap ) {
			rel = this->p[i].position - this->p[j].position;
			d = Distance_sq( this->p[i].position, this->p[j].position );
			mod = rel.SqrtMod();
			if ( d != mod )
				rel = -rel;
			rel = rel/mod;

			p = ( this->p[i].diameter + this->p[j].diameter ) / 2 - d;
			F = F + p*rel;
		}
	}
	this->p[i].move(F);
	if ( F == (-F0) || count > N / 50 ) {
		//std::cout << "CAGED" << std::endl;
		return true;
	}
	F0 = F;
	} while ( this->DoesOverlap( i, i ) );

	return false;
}
