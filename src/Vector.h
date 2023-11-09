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

template <class Type, int Dim>
class Vector
{
public:
    Vector();
    Vector( Type Comp );
    Vector( Type Len, Direction Dir );
    Vector( const Vector<Type, Dim> & Vec );
	

	// TODO
	// Inline all operators

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
    void SetRandomComponents( Type Min, Type Max );

    Type	MagSq();
    double	Mag();
    
    static double   Angle( Vector &, Vector & );

    Type & operator[]( unsigned int index )
    {
		// TODO
		// Add assert for the index
        return static_cast<Type *>( this->x )[ index ];
    }
	
    
    Type              x[ Dim ];
};


//---------------------------------------------------------------------------------------------
// Constructors

template <class Type, int Dim>
Vector<Type, Dim>::Vector()
{
	for ( size_t i = 0; i < Dim; i++ )
		x[i] = 0;
}

template <class Type, int Dim>
Vector<Type, Dim>::Vector( Type Comp )
{
	for (size_t i = 0; i < Dim; i++)
		x[i] = Comp;
}

template <class Type, int Dim>
Vector<Type, Dim>::Vector( Type Mag, Direction Dir )
{
}

template <class Type, int Dim>
Vector<Type, Dim>::Vector( const Vector<Type, Dim> & Vec )
{
	for (size_t i = 0; i < Dim; i++)
		x[i] = Vec.x[i];
}

//---------------------------------------------------------------------------------------------
// Operators

template <class Type, int Dim>
void Vector<Type, Dim>::operator=( double c ) 
{
	for ( int i = 0; i < Dim; i++ )
		x[i] = c;
}

template <class Type, int Dim>
Vector<Type, Dim> Vector<Type, Dim>::operator+( Vector v ) 
{
	Vector w;
	for ( int i = 0; i < Dim; i++ )
		w.x[i] = x[i] + v.x[i];
	return w;
}

template <class Type, int Dim>
Vector<Type, Dim> Vector<Type, Dim>::operator-( Vector v ) 
{
	Vector w;
	for ( int i = 0; i < Dim; i++ )
		w.x[i] = x[i] - v.x[i];
	return w;
}

template <class Type, int Dim>
Vector<Type, Dim> Vector<Type, Dim>::operator*( double k )
{
	Vector w;
	for ( int i = 0; i < Dim; i++ )
		w.x[i] = x[i] * k;
	return w;
}

template <class Type, int Dim>
Vector<Type, Dim> Vector<Type, Dim>::operator/( double k )
{
	if ( k == 0 ) return Vector(0);
	k = 1.0f / k;
	Vector w;
	for ( int i = 0; i < Dim; i++ )
		w.x[i] = x[i] * k;
	return w;
}

template <class Type, int Dim>
double Vector<Type, Dim>::operator*( Vector v )
{
	double Scalar = 0;
	for ( int i = 0; i < Dim; i++ )
		Scalar += x[i] * v.x[i];
	return Scalar;
}

template <class Type, int Dim>
Vector<Type, Dim> Vector<Type, Dim>::operator^( Vector v )
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

template <class Type, int Dim>
bool Vector<Type, Dim>::operator==( Vector v ) 
{
	for ( int i = 0; i < Dim; i++ )
		if ( fabs( x[i] - v.x[i] ) > Epsilon ) return false;
	return true;
}

template <class Type, int Dim>
void Vector<Type, Dim>::operator+=( Vector & v ) 
{
	for ( int i = 0; i < Dim; i++ )
		x[i] = x[i] + v.x[i];
}

//---------------------------------------------------------------------------------------------
// Functions

template <class Type, int Dim>
std::string Vector<Type, Dim>::ToString()
{
	std::string Str = "( ";
	for ( size_t i = 0; i < Dim; i++ )
		Str += std::to_string( x[i] ) + " ";
	Str += ")";
    return Str;
}

template <class Type, int Dim>
void Vector<Type, Dim>::Unit()
{
	Type invMod = 1.0f / Mag();					// How to normalize a vector of integers? See Lattice Theory

	for (size_t i = 0; i < Dim; i++)
		x[i] *= invMod;
}

template <class Type, int Dim>
inline void Vector<Type, Dim>::SetRandomComponents( Type a, Type b ) 
{ 
	for ( size_t i = 0; i < Dim; i++ )
		x[i] = RAND_IN( a, b );
}

template <class Type, int Dim>
inline Type Vector<Type, Dim>::MagSq()
{
	Type m = 0;
	for ( int i = 0; i < Dim; i++ )
		m += x[i] * x[i];
	return m;
}

template <class Type, int Dim>
inline double Vector<Type, Dim>::Mag()
{	
    return sqrt( static_cast<double>(MagSq()) );
}

template <class Type, int Dim>
double Vector<Type, Dim>::Angle( Vector & v, Vector & w ) 
{
	Type scalar = v * w;
	Type _v = v.MagSq(), _w = w.MagSq();
	if ( _v == 0 || _w == 0 ) return 0;
	return acos( scalar / sqrt( _v * _w ) );
}

