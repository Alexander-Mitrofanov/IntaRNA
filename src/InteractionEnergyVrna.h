
#ifndef INTERACTIONENERGYVIENNA_H_
#define INTERACTIONENERGYVIENNA_H_

#include "InteractionEnergy.h"
#include "VrnaHandler.h"


extern "C" {
	#include <ViennaRNA/fold_vars.h>
	#include <ViennaRNA/model.h>
	#include <ViennaRNA/params.h>
}

// TODO http://www.tbi.univie.ac.at/RNA/ViennaRNA/doc/RNAlib-2.2.10.pdf

/**
 * Implements an energy interface based on free energy estimates computed
 * with the Vienna RNA package.
 *
 * @author Martin Mann 2014
 */
class InteractionEnergyVrna: public InteractionEnergy {


public:

	/**
	 * Construct energy utility object given the accessibility ED values for
	 * two sequences.
	 *
	 * @param accS1 accessibility of the first sequence
	 * @param accS2 accessibility of the second sequence
	 * @param vrnaHandler the VRNA parameter handler to be used
	 * @param maxInternalLoopSize1 maximal number of enclosed unpaired positions
	 *          between two intermolecular base pairs in sequence 1, ie it holds
	 *          for an intermolecular loop closed by base pairs (i1,i2) and
	 *          (j1,j2) : (j1-i1+1) <= maxInternalLoopSize
	 * @param maxInternalLoopSize2 maximal number of enclosed unpaired positions
	 *          between two intermolecular base pairs in sequence 2, ie it holds
	 *          for an intermolecular loop closed by base pairs (i1,i2) and
	 *          (j1,j2) : (j2-i2+1) <= maxInternalLoopSize
	 *
	 */
	InteractionEnergyVrna( const Accessibility & accS1
					, const ReverseAccessibility & accS2
					, VrnaHandler &vrnaHandler
					, const size_t maxInternalLoopSize1 = 16
					, const size_t maxInternalLoopSize2 = 16
				);

	virtual ~InteractionEnergyVrna();


	/**
	 * Computes the energy estimate for the interaction loop region closed by
	 * the intermolecular base pairs (i1,i2) and (j1,j2) where the regions
	 * [i1,j1] and [i2,j2] are considered unpaired.
	 * The energy estimate is derived via the Vienna RNA package loop energies
	 * or is E_INF if the internal loop size exceeds
	 * the allowed maximum (see constructor).
	 *
	 * @param i1 the index of the first sequence interacting with i2
	 * @param j1 the index of the first sequence interacting with j2
	 * @param i2 the index of the second sequence interacting with i1
	 * @param j2 the index of the second sequence interacting with j1
	 *
	 * @return energy in kcal/mol for the loop closed by (i1,i2)
	 *         or
	 *         E_INF if the allowed loop size is exceeded
	 */
	virtual
	E_type
	getInterLoopE( const size_t i1, const size_t j1, const size_t i2, const size_t j2 ) const;


	/**
	 * Computes the dangling end energy penalty estimate for the left side of
	 * an interaction loop region closed on the left by the intermolecular
	 * base pair (i1,i2).
	 *
	 * This incorporates an additional penalty for stems not closed by a GC
	 * base pair.
	 *
	 * @param i1 the index of the first sequence interacting with i2
	 * @param i2 the index of the second sequence interacting with i1
	 *
	 * @return the dangling end penalty for the left side of the interaction
	 */
	virtual
	E_type
	getDanglingLeft( const size_t i1, const size_t i2 ) const;


	/**
	 * Computes the dangling end energy penalty estimate for the right side of
	 * an interaction loop region closed on the right by the intermolecular
	 * base pair (j1,j2).
	 *
	 * @param j1 the index of the first sequence interacting with j2
	 * @param j2 the index of the second sequence interacting with j1
	 *
	 * @return the dangling end penalty for the right side of the interaction
	 */
	virtual
	E_type
	getDanglingRight( const size_t j1, const size_t j2 ) const;


	/**
	 * Provides the penalty for closing an interaction with the given
	 * base pair on the "left side" (i1 = 5' end of seq1 of the interaction)
	 *
	 * @param i1 the index of the first sequence interacting with i2
	 * @param i2 the index of the second sequence interacting with i1
	 *
	 * @return the loop closure penalty for the left side of the interaction
	 */
	virtual
	E_type
	getEndLeft( const size_t i1, const size_t i2 ) const;

	/**
	 * Provides the penalty for closing an interaction with the given
	 * base pair on the "right side" (j1 = 3' end of seq1 of the interaction)
	 *
	 * @param i1 the index of the first sequence interacting with i2
	 * @param i2 the index of the second sequence interacting with i1
	 *
	 * @return the loop closure penalty for the right side of the interaction
	 */
	virtual
	E_type
	getEndRight( const size_t j1, const size_t j2 ) const;


	/**
	 * Returns the normalized energy in mol/kcal unit
	 * @return R*temperature
	 */
	virtual
	E_type
	getRT() const;

	/**
	 * Provides the minimal energy gain via stacking possible for this energy
	 * model
	 * @return the minimal energy possible for any stacking combination
	 */
	virtual
	E_type
	getBestStackingEnergy() const;

	/**
	 * Provides the minimal energy gain possible for interaction initiation
	 * for this energy model
	 * @return the initiation constant used
	 */
	virtual
	E_type
	getBestInitEnergy() const;

	/**
	 * Provides the minimal energy gain possible for left/right dangling ends
	 * for this energy model
	 * @return the best initiation energy gain produced by getDanglingLef() or
	 *          getDanglingRight()
	 */
	virtual
	E_type
	getBestDangleEnergy() const;

	/**
	 * Provides the best energy gain possible for left/right interaction ends
	 * for this energy model
	 * @return the best end energy gain produced by getEndLeft() or
	 *          getEndRight()
	 */
	virtual
	E_type
	getBestEndEnergy() const;

protected:

	//! Vienna RNA package : folding parameters to be used for the energy
	//! computation
	vrna_param_t * foldParams;

	//! the RT constant to be used for Boltzmann weight computations
	E_type RT;

	//! base pair code for (C,G)
	const int bpCG;

	//! base pair code for (G,C)
	const int bpGC;

	/**
	 * Checks whether or not a given base pair is a GC base pair
	 * @param i1 the index in the first sequence
	 * @param i2 the index in the second sequence
	 * @return true if (seq1(i1),seq2(i2)) is (G,C) or (C,G)
	 */
	bool
	isGC( const size_t i1, const size_t i2 ) const;

};

#endif /* INTERACTIONENERGYVIENNA_H_ */
