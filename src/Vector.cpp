#include "Vector.h"

Vector::Vector() :
	m_x(0),
	m_y(0),
	m_z(0)
{
}

Vector::Vector( double v ) : 
	m_x(v),
	m_y(v),
	m_z(v)
{
}

Vector::Vector( double x, double y, double z ) : 
	m_x(x),
	m_y(y),
	m_z(z)
{
}

Vector::Vector( double r, Direction d ) 
{
    m_x = r * sin(d.theta) * cos(d.phi); 
    m_y = r * sin(d.theta) * sin(d.phi);
	m_z = r * cos(d.theta);
}


void Vector::operator=( double c ) { m_x = c; m_y = c; m_z = c; }

Vector Vector::operator+( Vector v ) { return Vector{ m_x + v.m_x, m_y + v.m_y, m_z + v.m_z }; }

Vector Vector::operator-( Vector v ) { return Vector{ m_x - v.m_x, m_y - v.m_y, m_z - v.m_z }; }

Vector Vector::operator*( double k ) { return Vector{ m_x * k, m_y * k, m_z * k }; }

Vector Vector::operator/( double k ) { return Vector{ m_x / k, m_y / k, m_z / k }; }

Vector operator*( double k, Vector v ) { return v * k; }

Vector operator-( Vector v ) { return Vector { -v.m_x, -v.m_y, -v.m_z }; }

double Vector::operator*( Vector v ) { return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z; }

Vector Vector::operator^( Vector v ) { return Vector{ ( m_y * v.m_z - m_z * v.m_y ), ( m_z * v.m_x - m_x * v.m_z ), ( m_x * v.m_y - m_y * v.m_x ) }; }

bool Vector::operator==( Vector v ) { if ( fabs( m_x - v.m_x ) < cutoff && fabs( m_y - v.m_y ) < cutoff && fabs( m_z - v.m_z ) < cutoff) return true; else return false; }

void Vector::operator+=( Vector & v ) { this->m_x = this->m_x + v.m_x; this->m_y = this->m_y + v.m_y; this->m_z = this->m_z + v.m_z; }

Direction Vector::getDirection() 
{
	Direction d{ 0, 0 };
	double r = this->getMod();
	if ( r > 0 ) {
    	d.theta = acos( m_z / r );
		if ( m_x == 0 ) d.phi = 0;
		else d.phi = atan2( m_y, m_x );
	}
	return d;
}

std::string Vector::toString() { return "(" + std::to_string(m_x) + "," + std::to_string(m_y) + "," + std::to_string(m_z) + ")"; }

void Vector::setRandomComponents( double a, double b ) 
{ 
	m_x = (double) rand() / RAND_MAX * ( b - a ) + a;
	m_y = (double) rand() / RAND_MAX * ( b - a ) + a; 
	m_z = (double) rand() / RAND_MAX * ( b - a ) + a; 
}

void Vector::rotate( Direction d )
{
	// double theta, mod;
	// theta = this->getDirection().theta + d.theta;
	// if ( theta > M_PI ) theta -= 2 * M_PI;
	// else if ( theta < -M_PI ) theta += 2 * M_PI;
	// mod = this->getMod();
	// m_x = mod * cos( theta );
	// m_y = mod * sin( theta );
}

Vector Vector::Unit( Vector v ) { 
	double norm = v.getMod();
	return Vector{ v.m_x / norm, v.m_y / norm, v.m_z / norm }; 
}

Vector Vector::RandomVector( double a, double b )
{
	return Vector{ (double) rand() / RAND_MAX * ( b - a ) + a, (double) rand() / RAND_MAX * ( b - a ) + a, (double) rand() / RAND_MAX * ( b - a ) + a  };
}

double Vector::Angle( Vector a, Vector b ) 
{ 
	double scalar = a * b;
	double _a = a.getMod(), _b = b.getMod();
	if ( _a == 0 || _b == 0 ) return 0;
	else 					return acos( scalar / _a / _b );
}



