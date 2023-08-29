#include "Vector.h"

using namespace Constant;

template <class Ty, int Dim>
Vector<Ty, Dim>::Vector()
{
	for ( size_t i = 0; i < Dim; i++ )
		x[i] = 0;
}

template <class Ty, int Dim>
Vector<Ty, Dim>::Vector( Ty Value )
{
	for (size_t i = 0; i < Dim; i++)
		x[i] = Value;
}

template <class Ty, int Dim>
Vector<Ty, Dim>::Vector( Ty Mag, Direction Dir )
{
}

template <class Ty, int Dim>
Vector<Ty, Dim>::Vector( Vector<Ty, Dim> & Vec )
{
	for (size_t i = 0; i < Dim; i++)
		x[i] = Vec.x[i];
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

bool Vector::operator==( Vector v ) { if ( fabs( m_x - v.m_x ) < epsilon && fabs( m_y - v.m_y ) < epsilon && fabs( m_z - v.m_z ) < epsilon) return true; else return false; }

void Vector::operator+=( Vector & v ) { this->m_x = this->m_x + v.m_x; this->m_y = this->m_y + v.m_y; this->m_z = this->m_z + v.m_z; }

template <class Ty, int Dim>
std::string Vector<Ty, Dim>::ToString()
{
	std::string Str = "(";
	for ( size_t i = 0; i < Dim; i++ )
		Str += std::to_string( x[i] ) + " ";
	Str += ")";
    return Str;
}

template <class Ty, int Dim>
inline void Vector<Ty, Dim>::Unit()
{
	Ty Mod = Mag();

	for (size_t i = 0; i < Dim; i++)
		x[i] /= Mod;
}

template <class Ty, int Dim>
inline void Vector<Ty, Dim>::SetRandomComponents( Ty a, Ty b ) 
{ 
	for ( size_t i = 0; i < Dim; i++ )
		x[i] = u_rand_in( a, b );
}

template <class Ty, int Dim>
inline double Vector<Ty, Dim>::Mag()
{	
    return sqrt( MagSq() );
}

template <class Ty, int Dim>
inline double Vector<Ty, Dim>::MagSq()
{
	Ty m = 0;
	for ( size_t i = 0; i < count; i++ )
		m += x[i] * x[i];
	return m;
}

template <class Ty, int Dim>
double Vector<Ty, Dim>::Angle( Vector & v, Vector & w )
{
	Ty scalar = v * w;
	Ty _v = v.Mag(), _w = w.Mag();
	if ( _v == 0 || _w == 0 ) return 0;
	return acos( scalar / _v / _w );
}



