#pragma once

#include "pch.h"
#include "Vector.h"

using namespace Constant;

class Particle
{
public:
    Particle();
    Particle( const Vector<double, D> &, double );
    Particle( const Vector<double, D> &, const Vector<double, D> & );
    Particle( int, const Vector<double, D> &, const Vector<double, D> &, double );
    ~Particle();

    void                                    move( double );
    void                                    move( Vector<double, D> );

    int                                     id;
    int                                     sector;
	Vector<double, D>                       position;
    Vector<double, D>                       velocity;
	double                                  diameter;
    std::vector<int>                        first_neighbors;
};

