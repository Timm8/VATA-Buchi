/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasc00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Reversion for explicitly represented buchi automata.
 *
 *****************************************************************************/

// VATA headers
#include <vata/vata.hh>

#include "explicit_buchi_aut_core.hh"

VATA::ExplicitBuchiAutCore VATA::ExplicitBuchiAutCore::Reverse(
	AutBase::StateToStateMap* /*pTranslMap*/) const
{
	typedef VATA::ExplicitBuchiAutCore ExplicitBA;

	ExplicitBA result;

	result.acceptStates_ = initialStates_; //Set accept states
	result.initialStates_ = acceptStates_; //Set initial states
	result.initialStateToSymbols_ = initialStateToSymbols_; //Start symbols

	//Changing the order of left and right in each transition
	for (auto stateToCluster : *transitions_) {
		for (auto symbolToSet : *stateToCluster.second) {
			for (auto stateInSet : symbolToSet.second) {
				result.AddTransition(stateInSet,symbolToSet.first,
					stateToCluster.first);
			}
		}
	}

	return result;
}