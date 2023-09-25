#pragma once 

#include "pch.h"
#include "Utils.h"
#include "Constant.h"

struct Direction /* Angles are defined according to polar coordinates: theta[0,Pi], phi(-Pi,Pi] */
{
    double theta;
    double phi;
};

template <int Dim>
struct Point
{
    double x[Dim];
};

template <class Ty, int Dim>
class Vector
{
public:
    Vector();
    Vector( Ty Value );
    Vector( Ty Mag, Direction Dir );
    Vector( const Vector<Ty, Dim> & Vec );

    Vector operator+( Vector );
    Vector operator-( Vector );
    Vector operator*( double Scalar );
    Vector operator^( Vector );
    Vector operator/( double Scalar );
    void   operator=( double Scalar );
    bool   operator==( Vector );
    void   operator+=( Vector & );
    double operator*( Vector );

    std::string ToString();
    void Unit();
    void SetRandomComponents( Ty Min, Ty Max );

    double Mag();
    double MagSq();
    
    static double   Angle( Vector &, Vector & );
    
    Ty              x[ Dim ];
};


//-----------------------------------------------------------------------------
// Constructors

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
Vector<Ty, Dim>::Vector( const Vector<Ty, Dim> & Vec )
{
	for (size_t i = 0; i < Dim; i++)
		x[i] = Vec.x[i];
}

//-----------------------------------------------------------------------------
// Operators

template <class Ty, int Dim>
void Vector<Ty, Dim>::operator=( double c ) 
{
	for ( int i = 0; i < Dim; i++ )
		x[i] = c;
}

template <class Ty, int Dim>
Vector<Ty, Dim> Vector<Ty, Dim>::operator+( Vector v ) 
{
	Vector w;
	for ( int i = 0; i < Dim; i++ )
		w.x[i] = x[i] + v.x[i];
	return w;
}

template <class Ty, int Dim>
Vector<Ty, Dim> Vector<Ty, Dim>::operator-( Vector v ) 
{
	Vector w;
	for ( int i = 0; i < Dim; i++ )
		w.x[i] = x[i] - v.x[i];
	return w;
}

template <class Ty, int Dim>
Vector<Ty, Dim> Vector<Ty, Dim>::operator*( double k )
{
	Vector w;
	for ( int i = 0; i < Dim; i++ )
		w.x[i] = x[i] * k;
	return w;
}

template <class Ty, int Dim>
Vector<Ty, Dim> Vector<Ty, Dim>::operator/( double k )
{
	if ( k == 0 ) return Vector(0);
	Vector w;
	for ( int i = 0; i < Dim; i++ )
		w.x[i] = x[i] / k;
	return w;
}

template <class Ty, int Dim>
double Vector<Ty, Dim>::operator*( Vector v )
{
	double Scalar = 0;
	for ( int i = 0; i < Dim; i++ )
		Scalar += x[i] * v.x[i];
	return Scalar;
}

template <class Ty, int Dim>
Vector<Ty, Dim> Vector<Ty, Dim>::operator^( Vector v )
{
	if ( Dim == 3 )
	{
		Vector w;
		w.x[0] = x[1] * v.x[2] - x[2] * v.x[1];
		w.x[1] = x[2] * v.x[0] - x[0] * v.x[2];
		w.x[2] = x[0] * v.x[1] - x[1] * v.x[0];
		return w;
	}
	else return Vector(0);
}

template <class Ty, int Dim>
bool Vector<Ty, Dim>::operator==( Vector v ) 
{
	for ( int i = 0; i < Dim; i++ )
		if ( fabs( x[i] - v.x[i] ) > Epsilon ) return false;
	return true;
}

template <class Ty, int Dim>
void Vector<Ty, Dim>::operator+=( Vector & v ) 
{
	for ( int i = 0; i < Dim; i++ )
		x[i] = x[i] + v.x[i];
}

//-----------------------------------------------------------------------------
// Functions

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
		x[i] = RAND_IN( a, b );
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
	for ( int i = 0; i < Dim; i++ )
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

