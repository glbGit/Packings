#include "Particle.h"
#include "Box.h"

Particle::Particle() : 
	id( NO_INDEX ),
	sector( NO_INDEX ),
    position(),
    velocity(),
    diameter(0)
{
}

Particle::Particle( const Vector<double, D> & r, double d ) : 
	id( NO_INDEX ),
	sector( NO_INDEX ),
    position(r),
    velocity(),
    diameter(d)
{
}

Particle::Particle( const Vector<double, D> & r, const Vector<double, D> & v ) : 
	id( NO_INDEX ),
	sector( NO_INDEX ),
    position(r),
    velocity(v),
    diameter(0)
{
}

Particle::Particle( int index, const Vector<double, D> & r, const Vector<double, D> & v, double d ) : 
	id( index ),
	sector( NO_INDEX ),
    position(r),
    velocity(v),
    diameter(d)
{
}

Particle::~Particle()
{
}

void Particle::move( double t )
{ 
    Vector<double, D> vt = this->velocity * t;
    Vector<double, D> NewPosition = this->position + vt;
    for ( int i = 0; i < D; i++ )
        NewPosition.x[i] =  BoundaryConditions::Periodic::Image( NewPosition.x[i] );
    position = NewPosition; 
}

void Particle::move( Vector<double, D> r )
{ 
    Vector<double, D> NewPosition = this->position + r;
    for ( int i = 0; i < D; i++ )
        NewPosition.x[i] =  BoundaryConditions::Periodic::Image( NewPosition.x[i] );
    this->position = NewPosition; 
}




