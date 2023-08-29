#pragma once

#include "pch.h"
#include "Box.h"
#include "Map.h"
#include "Vector.h"
#include "Particle.h"
#include "Distribution.h"

namespace System
{
    enum class State 
    {
        Compression,
        Relaxation
    };

    namespace Interaction
    {
        enum class State 
        {
            Overlap,
            Contact,
            FirstNeighbor,
            Free
        };
        
        enum class Potential 
        {
            Hard,
            SoftRepulsive,
            LennardJones
        };

        class Local
        {   
        public:
            Local() : id( NO_INDEX ), energy(0), overlap( false ) {}
            Local( ULL i ) : id(i), energy(0), overlap( false ) {}
            Local( ULL i, double e ) : id(i), energy(e), overlap(false) {}
            Local( ULL i, double e, bool ov ) : id(i), energy(e), overlap(ov) {}
            ULL id;
            double energy;
            bool overlap;
        };
    }
    
    namespace Variables
    {
        extern double s_min;
        extern double s_max;
        extern double g_min;
        extern double g_max;
        extern Distribution::Type distribution_type;
        extern Interaction::Potential interaction_type;
    }
}

using namespace System;

class Packing
{
public:
    Packing();
    ~Packing();

    void                            Initialize();
    void                            Make();
    void                            Make( const char * );
    int                             Overlaps();
    double                          Energy();
    double                          VolumeFraction();
    double                          NewState_( int, Vector &, std::vector<System::Interaction::Local> &, bool & );
    void                            NewState( int, Vector &, std::vector<System::Interaction::Local> &, bool & );
    void                            NewState( int, double, std::vector<System::Interaction::Local> &, bool & );
    void                            FullUpdate();
    void                            Update( System::State );
    void                            Swap( int, int );
    void                            PrintToFile();
    void                            PrintToFile( const char * );
    void                            PrintToFile( FILE *, int );
    void                            PrintDiameterDistribution();
    void                            PrintFirstNeighbors();
    void                            PrintSystemInfo( On time );
    int                             Step();
    void                            Compress();
    double                          GetMinDiameter();
    double                          GetMaxDiameter();

    bool                            DoesOverlap( int, int );
    bool                            Drift( int );

    static Interaction::State       State( double, double );

    
    Particle                        *p;
    Box::Sector                     *s;
    Interaction::Local              *c;
    double                          *g;
    std::list<ULL>                  overlap_list;
    double                          energy;
    double                          phi;
    int                             steps = 0;
    int                             accepted = 0;
};
