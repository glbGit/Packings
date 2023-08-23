#include "pch.h"
#include "Packing.h"

using namespace System;
using namespace System::Variables;
using namespace System::Interaction;
using namespace Constant;

double System::Variables::sigma_min = Dmin;
double System::Variables::sigma_max = Dmax;
Potential System::Variables::interaction_type = Potential::Hard;
SizeDispersity System::Variables::diameter_distribution = SizeDispersity::PowerLaw;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int main() 
{
	srand(321);
	int steps = 0; 

	/* Entry point */
	Packing R;
	Packing::Time(1);
	R.Make();

	/* Relax */
	if ( InitialRelaxation )
	{
		interaction_type = Potential::SoftRepulsive;

		/* Relax pre-compression */
		printf( "Relaxing the system before compressing..    " );
		for ( int i = 0; i < 50; i++ )  
		{
			R.Step();
			R.Update( System::State::Relaxation );
		}
		printf( "Completed.\n" );
	}

	/* Compression */
	interaction_type = Potential::Hard;
	R.Compress( steps );

	/* Print */
	R.PrintToFile();
	R.PrintDiameterDistribution();
	R.PrintSystemInfo( steps );

	Packing::Time(0);
	return 0;
}
