#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "Packing.h"
#include "Vector.h"

using namespace BoundaryConditions::Periodic;
using namespace System;
using namespace System::Variables;
using namespace System::Interaction;
using namespace Distribution;
using namespace Constant;
using namespace Packings;

//---------------------------------------------------------------------------------------------

Packing::Packing() :
    energy(0),
    phi(0),
    steps(0),
    accepted(0)
{
    g = new double[N];
    p = new Particle[N];
    c = new Local[ NumOfPairs ];
    for ( int i = 0; i < N; i++ ) 
    {
        p[i] = Particle{ };
        g[i] = 2;
    }
}

Packing::~Packing() 
{
    delete[] p;
    delete[] c;
    delete[] g;
}

void Packing::Initialize()
{
    s_min = GetMinDiameter();
    s_max = GetMaxDiameter();
    g_min = s_min * a0;
    g_max = s_max * a0;
    distribution_type = GetDistributionInfo().type;

}

void Packing::Make()
{
    // Distribution
    Distribution::Info info = GetDistributionInfo();

    // Particles
    printf( "Adding %lu particles to the system..    ", N );

    for ( int i = 0; i < N; i++ ) 
    {
        if ( this->p[i].id == NO_INDEX ) 
        {
            this->p[i].id = i;
            this->p[i].diameter = RandomDiameter( info );
            this->g[i] = a0 * this->p[i].diameter;
            int count = 0;
		    do 
            {
                count++; 
                if ( count > N ) Exit( RELOCATION_OVERFLOW, { "Failed to place particle ", to_cstr(i), "." } );
			    this->p[i].position.SetRandomComponents( 0, L );
		    } while ( this->DoesOverlap( i, i ) );

		    double sigma = 1;
		    for ( int k = 0; k < D; k++ )
			    sigma = sigma * p[i].diameter;
		    phi = phi + sigma;
        }
	}
    vol_const = pow( 0.25 * Pi, 0.5 * D ) / pow( L, D ) / exp( lgamma( 1 + 0.5 * D ) );
	phi = phi * vol_const;

    printf( "Particles successfully added.\n" );

    // Box
    double s_max_star = pow(Phi_t / phi * pow(info.sigma_max, D), 1. / D);
    static Box _Box( s_max_star );
    b = &_Box;
    b->MakeNeighbors();

	for ( int i = 0; i < N; i++ )
	{
		Vector<int, D> _s;
		for ( int k = 0; k < D; k++ )
			_s.x[k] = static_cast<int>( p[i].position.x[k] * b->SectorLengthInverse );

		int l = b->FindSector( _s );
		b->s[l].member_list.push_back(i);
        p[i].sector = l;
	}

    this->Initialize();
    this->FullUpdate();
}

void Packing::Make( const char * filename )
{   
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
            int n, entries = 0;
            int result = fscanf( stream, "%[^\n]", line );
            if ( result != 0 )
            {
                if ( line[0] == '\0' ) Exit( INSUFFICIENT_DATA, { "Too few entries (", to_cstr(i), "/", to_cstr(N), ")." } );
		        char * line_ptr = line;
		        for ( int k = 0; k < D; k++ )
		        {
	    	        entries += sscanf( line_ptr, "%lg %n", &p[i].position.x[k], &n );
			        line_ptr += n;
		        }
	            entries += sscanf( line_ptr, "%lg %n", &p[i].diameter, &n );
		        line_ptr += n;
                p[i].diameter = p[i].diameter + p[i].diameter;
		        g[i] = a0 * p[i].diameter;
	            fgetc( stream );
                line[0] = '\0';
                if ( entries != D + 1 ) Exit( WRONG_INPUT_FORMAT, { "Entry format must be [x_1] [x_2] ... [x_D] [rad]." } );

                this->p[i].id = i;
                this->g[i] = a0 * this->p[i].diameter;
            } else Exit( EMPTY_LINE, { "Empty line (", to_cstr( i + 1 ), ")." } );
            
		    double sigma = 1;
		    for ( int k = 0; k < D; k++ )
			    sigma = sigma * p[i].diameter;
		    phi = phi + sigma;

	    }
	    phi = phi * vol_const;
        fclose( stream );
    } else Exit( DIR_NOT_FOUND, { "File \"", filename, "\" not found." } );
    printf( "Particles successfully added.\n" );
    
    // Distribution
    Distribution::Info info = GetDistributionInfo();

    // Box
    double s_max_star = pow(Phi_t / phi * pow(info.sigma_max, D), 1. / D);
    static Box _Box( s_max_star );
    b = &_Box;
    b->MakeNeighbors();

	for ( int i = 0; i < N; i++ )
	{
		Vector<int, D> _s;
		for ( int k = 0; k < D; k++ )
			_s.x[k] = static_cast<int>( p[i].position.x[k] * b->SectorLengthInverse );

		int l = b->FindSector( _s );
		b->s[l].member_list.push_back(i);
        p[i].sector = l;
	}

    this->Initialize();
    this->FullUpdate();
}

void Packing::PrintToFile() 
{
    FILE * out;
    out = fopen( "data/output.dat", "w+" );
    if ( out != NULL )
    {
        for ( int i = 0; i < N; i++ )
        {
            for ( int k = 0; k < D; k++ )
                fprintf( out, "%.16f ", p[i].position.x[i] );
            fprintf( out, "%.16f\n", 0.5 * p[i].diameter );
        }
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
        {
            for ( int k = 0; k < D; k++ )
                fprintf( out, "%.16f ", p[i].position.x[i] );
            fprintf( out, "%.16f\n", 0.5 * p[i].diameter );
        }
        fclose( out );
    } else Exit( DIR_NOT_FOUND, { "Destination folder \"/data\" not found." } ); 
}

void Packing::PrintToFile( FILE * out, int frame ) 
{
    if ( out != NULL )
    {
        for ( int i = 0; i < N; i++ )
        {
            for ( int k = 0; k < D; k++ )
                fprintf( out, "%.16f ", p[i].position.x[i] );
            fprintf( out, "%.16f ", 0.5 * p[i].diameter );
            fprintf( out, "%d\n", frame );
        }
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
	    for ( double q = s_min; q <= s_max; q += 0.01 ) {      
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
void Packing::PrintSystemInfo( On time )
{    
	switch ( time )
	{
    case On::Start:
		printf(
			"-----------------------------------------------\n"
			"Parameters:\n"
			"D = %u\n"
			"N = %u\n"
			"L = %g\n"
			"PSwap = %g\n"
			"DType = %d\n"
			"Ratio = %g\n"
			"Cells = %d\n"
			"Growth rate = %g\n"
			"Phi = %g\n"
			"Phi_t = %g\n"
			"dr = %g\n"
			"-----------------------------------------------\n",
			D, N, L, SwapChance, DType, Ratio, b->sectorsPerSide,  
			a0, this->phi, Phi_t, Delta_r
		);
		break;
	
    case On::Exit:
		printf(
			"-----------------------------------------------\n"
			"System info:\n"
            "Energy = %g\n"
			"Overlaps = %u\n"
			"Monte Carlo steps = %d\n"
			"Acceptance rate = %.3f\n"
			"Volume fraction = %.6f\n"
			"Size dispersity = %.6f\n"
			"Diameter = [%.6f, %.6f]\n"
			"Growth rate = [%.2e, %.2e]\n"
			"-----------------------------------------------\n",
			this->energy, this->Overlaps(), this->steps, (double) this->accepted / this->steps / N, this->phi,
			this->GetDispersity(), s_min, s_max, g_min, g_max
		);
		if ( s_max > b->SectorLength ) printf( "WARNING: Maximum diameter exceeding sector length! (May generate overlaps.)\n" );
		break;

	default:
		break;
	}
}

inline double Packing::VolumeFraction()
{
	double phi = 0;
    for ( int i = 0; i < N; i++ )
    {
	    double sigma = 1;
        for ( int k = 0; k < D; k++ )
		    sigma = sigma * p[i].diameter;
	    phi = phi + sigma;
    }
    phi = phi * vol_const;
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
inline void Packing::Swap( int i, int j )
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

    phi = 0;
    for ( int i = 0; i < N; i++ ) 
    {
		double sigma = 1;
		for ( int k = 0; k < D; k++ )
			sigma = sigma * p[i].diameter;
		phi = phi + sigma;

        this->p[i].first_neighbors.clear();                        
    }
    phi = phi * vol_const;
        
    for ( int k = 0; k < b->numSectors; k++ ) 
    {       /* This sector */
        for ( auto i : b->s[k].member_list )
        {
            for ( auto j : b->s[k].member_list )
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
                        this->c[ Hash<N>( i, j ) ].energy = exp( -Constant::b * ( sqrt(r_ij) - sigma_ij ) );
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
            for ( auto _k : b->s[k].neighbor_list )
            {
                for ( auto j : b->s[_k].member_list )
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
                        this->c[ Hash<N>( i, j ) ].energy = exp( -Constant::b * ( sqrt(r_ij) - sigma_ij ) ); 
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
                        this->c[id].energy = exp( -Constant::b * ( sqrt(r_ij) - sigma_ij ) );
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
        
        for ( int k = 0; k < b->numSectors; k++ ) 
        {       /* This sector */
            for ( auto i : b->s[k].member_list )
            {
                for ( auto j : b->s[k].member_list )
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
                for ( auto _k : b->s[k].neighbor_list )
                {
                    for ( auto j : b->s[_k].member_list ) 
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

double Packing::NewState_( int i, Vector<double, D> & NewPosition, std::vector<System::Interaction::Local> & c_p, bool & flag )
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
                c_p.push_back( { id, exp( -Constant::b * ( sqrt(r_ij) - sigma_ij ) ) } );
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

void Packing::NewState( int i, Vector<double, D> & NewPosition, std::vector<System::Interaction::Local> & c_p, bool & flag )
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
            double rnd;
            bool no_overlap;
            Vector<double, D> _r;    
            std::vector<Local> c_p;                                                                     
            for ( int i = 0; i < N; i++ ) 
            {
                no_overlap = true;
                rnd = RAND;
                if ( rnd < 1 - SwapChance )                    /* Standard Monte Carlo move */
                { 
			        double scalingFactor = 1 / ( p[i].diameter * p[i].diameter );
			        scalingFactor = scalingFactor < b->SectorLength ? scalingFactor : b->SectorLength;
                    for ( int k = 0; k < D; k++ ) 
                    {
                        double coord =  p[i].position.x[k] + ( RAND - 0.5 ) * Delta_r * scalingFactor; 
                        _r.x[k] = IMAGE( coord ); 
                    }
                    this->NewState( i, _r, c_p, no_overlap );
                    if ( no_overlap ) 
                    {
                        accept++;
                        this->p[i].position = _r;
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
                        Vector<int, D> _s;
                        for ( int k = 0; k < D; k++ )  
                            _s.x[k] = static_cast<int>( p[i].position.x[k] / b->SectorLength );
                        int new_sector = b->FindSector( _s );
                        if ( new_sector != this->p[i].sector ) 
                        {
                            b->s[ this->p[i].sector ].member_list.remove(i);
                            b->s[ new_sector ].member_list.push_back(i);
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
            double rnd, dE;
            bool no_overlap;
            Vector<double, D> _r;
            std::vector<Local> c_p;
            for ( int i = 0; i < N; i++ ) 
            {
                no_overlap = true;
                for ( int k = 0; k < D; k++ )
                {
                    double coord = p[i].position.x[k] + ( RAND - 0.5 ) * Delta_r;
                    _r.x[k] = IMAGE( coord );
                }
                dE = this->NewState_( i, _r, c_p, no_overlap );
                rnd = RAND;
                if ( rnd < exp( -dE / T ) && no_overlap ) 
                {
                    accept++;
                    this->p[i].position = _r;
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
                    Vector<int, D> _s;
                    for ( int k = 0; k < D; k++ )  
                        _s.x[k] = static_cast<int>( p[i].position.x[k] / b->SectorLength );
                    int new_sector = b->FindSector( _s );
                    if ( new_sector != this->p[i].sector ) 
                    {
                        b->s[ this->p[i].sector ].member_list.remove(i);
                        b->s[ new_sector ].member_list.push_back(i);
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
    this->steps = this->steps + 1;
    return accept;
}

/* Compress the system by growing particles. */
void Packing::Compress()
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
        printf( "Overlaps %lu, phi = %.6f", this->Overlaps(), this->phi );
        while ( this->overlap_list.size() > 0 ) 
        {
            this->accepted = this->accepted + this->Step();
            this->Update( System::State::Relaxation );
            count++;
            if ( count > MaxSteps ) 
            {  
                printf( " - System failed to compress.\n\n" );
                /* Decompress on exit */
                for ( int i = 0; i < N; i++ ) this->p[i].diameter = this->p[i].diameter - this->g[i];
                this->FullUpdate();
                this->PrintToFile();
                this->PrintDiameterDistribution();
                this->PrintSystemInfo( On::Exit );
                Time( On::End );
                exit( STEPS_OVERFLOW );
            }
        }
        printf( " - mcs = %d\n", count );
        s_min = s_min + g_min;
        s_max = s_max + g_max;
    }  
                this->FullUpdate();
    printf( "Completed.\n" ); 
}

double Packing::GetMinDiameter()
{
    double Min = L;
    for ( int i = 0; i < N; i++ )
        if ( this->p[i].diameter < Min ) Min = this->p[i].diameter;
    return Min;
}

double Packing::GetMaxDiameter()
{
    double Max = 0;
    for ( int i = 0; i < N; i++ )
        if ( this->p[i].diameter > Max ) Max = this->p[i].diameter;
    return Max;
}

double Packing::GetDispersity()
{	
    double eval = 0;
    for ( int i = 0; i < N; i++ )
        eval += p[i].diameter;
    eval /= N;
	
    double var = 0;
    for ( int i = 0; i < N; i++ )
    {
        double diff = p[i].diameter - eval;
        var += diff * diff;
    }
    var /= N;
	
    return sqrt( var ) / eval;
}

/* Return the interaction state between two given particles. */
Interaction::State Packing::State( double r_sq, double sigma )
{
    double r_cut_sq;
    if ( interaction_type == Potential::Hard ) r_cut_sq = s_max + 2 * Delta_r;   
    else r_cut_sq = 0.5 * s_max + sigma + 2 * Delta_r;
    r_cut_sq = r_cut_sq * r_cut_sq;
	if ( r_sq > r_cut_sq ) return Interaction::State::Free;
    else
    {
        sigma = sigma * sigma;
        if ( r_sq > sigma + Epsilon )  return Interaction::State::FirstNeighbor;
        else if ( r_sq < sigma - Epsilon ) return Interaction::State::Overlap;  
        else return Interaction::State::Contact;           
    } 
}

//---------------------------------------------------------------------------------------------

bool Packing::DoesOverlap( int j, int n )
{
    double r_ij, sigma_ij;
	for ( int i = 0; i < n; i++ ) {
		if ( i != j ) {
            r_ij = Distance_sq( this->p[i].position, this->p[j].position );
            sigma_ij = ( this->p[i].diameter + this->p[j].diameter ) / 2;
            sigma_ij *= sigma_ij;
			if ( r_ij < sigma_ij - Epsilon ) return true;
        }
	}
	return false;
}

bool Packing::Drift( int i )
{
    int count = 0;
	double d, mod, p, r_ij, sigma_ij;
	Vector<double, D> rel, F0, F;
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
			mod = rel.Mag();
			if ( d != mod )
				rel = rel * (-1);
			rel = rel/mod;

			p = ( this->p[i].diameter + this->p[j].diameter ) / 2 - d;
			F = F + rel*p;
		}
	}
	this->p[i].move(F);
	if ( F == (F0 * (-1)) || count > N / 50) {
		//std::cout << "CAGED" << std::endl;
		return true;
	}
	F0 = F;
	} while ( this->DoesOverlap( i, i ) );

	return false;
}
