#pragma once

#include "pch.h"
#include "Utils.h"
#include "Constant.h"
#include "Vector.h"

namespace Distribution
{
	enum Type
	{ 
		Uniform,
		Binary,
		PowerLaw,
		LogNormal 
	};

	struct Info 
	{ 
		Type type; 
		const double * param; 
		double sigma_min; 
		double sigma_max; 
	};

	Info GetDistributionInfo();
	double RandomDiameter( const Info & info );
	double Moment( const double * diameter, int order );
	double GetMin( const double * diameter );
	double GetMax( const double * diameter );

}