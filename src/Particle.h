#pragma once

#include "pch.h"
#include "Vector.h"

class Particle
{
public:
    Particle();
    Particle( Vector, double );
    Particle( Vector, Vector );
    Particle( int, Vector, Vector, double );
    ~Particle();

    void                                    move( double );
    void                                    move( Vector );

    int                                     id;
    int                                     sector;
	Vector                                  position;
    Vector                                  velocity;
	double                                  diameter;
    std::vector<int>                        first_neighbors;
};

