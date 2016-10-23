
#include "PredictorMfe.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////////

PredictorMfe::PredictorMfe( const InteractionEnergy & energy, OutputHandler & output )
	: Predictor(energy,output)
	, mfeInteraction(energy.getAccessibility1().getSequence()
			,energy.getAccessibility2().getAccessibilityOrigin().getSequence())
	, minStackingEnergy( energy.getBestE_interLoop() )
	, minInitEnergy( energy.getE_init() )
	, minDangleEnergy( energy.getBestE_dangling() )
	, minEndEnergy( energy.getBestE_end() )

{

}

////////////////////////////////////////////////////////////////////////////

PredictorMfe::~PredictorMfe()
{
}


////////////////////////////////////////////////////////////////////////////

void
PredictorMfe::
initMfe()
{
	// initialize global E minimum : should be below 0.0
	mfeInteraction.energy = 0.0;
	// ensure it holds only the boundary
	if (mfeInteraction.basePairs.size()!=2) {
		mfeInteraction.basePairs.resize(2);
	}
	// reset boundary base pairs
	mfeInteraction.basePairs[0].first = RnaSequence::lastPos;
	mfeInteraction.basePairs[0].second = RnaSequence::lastPos;
	mfeInteraction.basePairs[1].first = RnaSequence::lastPos;
	mfeInteraction.basePairs[1].second = RnaSequence::lastPos;
}

////////////////////////////////////////////////////////////////////////////

void
PredictorMfe::
updateMfe( const size_t i1, const size_t j1
		, const size_t i2, const size_t j2
		, const E_type hybridE )
{
//	LOG(DEBUG) <<"#DEBUG : energy( "<<i1<<"-"<<j1<<", "<<i2<<"-"<<j2<<" ) = "
//			<<hybridE;

	// TODO check if reasonable to check only interactions with hybridE+dangleE < 0
	if (hybridE + 2*energy.getBestE_dangling() > 0) {
		return;
	}

	// get final energy of current interaction
	E_type curE = energy.getE( i1,j1, i2,j2, hybridE );
//	LOG(DEBUG) <<"energy( "<<i1<<"-"<<j1<<", "<<i2<<"-"<<j2<<" ) = "
//			<<hybridE <<" : total = "<<curE;

	if (curE < mfeInteraction.energy) {
//	LOG(DEBUG) <<"#DEBUG : new mfe( "<<i1<<"-"<<j1<<", "<<i2<<"-"<<j2<<" ) = "
//			<<hybridE;
		// store new global min
		mfeInteraction.energy = (curE);
		// store interaction boundaries
		// left
		mfeInteraction.basePairs[0] = energy.getBasePair(i1,i2);
		// right
		mfeInteraction.basePairs[1] = energy.getBasePair(j1,j2);
	}
}

////////////////////////////////////////////////////////////////////////////

