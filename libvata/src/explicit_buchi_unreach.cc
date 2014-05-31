/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Removing unreachable states for explicitly represented buchi automata.
 *
 *****************************************************************************/

// VATA headers
#include <vata/vata.hh>
#include <vata/util/transl_strict.hh>

#include "explicit_buchi_aut_core.hh"

// Standard library headers
#include <vector>
#include <unordered_set>
#include <utility>


VATA::ExplicitBuchiAutCore VATA::ExplicitBuchiAutCore::RemoveUnreachableStates(
	VATA::AutBase::StateToStateMap* pTranslMap) {

	// Start from initial states
	std::unordered_set<AutBase::StateType> reachableStates(this->GetInitialStates());
	std::vector<AutBase::StateType> newStates(reachableStates.begin(),reachableStates.end());

	// Find all reachable states
	while (!newStates.empty()) { // While there are new reacheable states
		
		auto actState = newStates.back();
		newStates.pop_back();
		auto cluster = ExplicitBuchiAutCore::genericLookup(*transitions_, actState);

		if (!cluster) {
			continue;
		}

		 // Add all reachable states to reachable state set
		for (auto &symbolsToStateSet : *cluster) {
			for (auto &state : symbolsToStateSet.second) {
				if (reachableStates.insert(state).second) {
					newStates.push_back(state);
				}
			}
		}
	}

	if (pTranslMap) {
		for (auto& state : reachableStates) {
			pTranslMap->insert(std::make_pair(state, state));
		}
	}

	ExplicitBuchiAutCore result;
	
	//Initialize the result automaton
	result.initialStates_ = initialStates_;
	result.initialStateToSymbols_ = initialStateToSymbols_;

	result.transitions_ = StateToTransitionClusterMapPtr(
			new typename ExplicitBuchiAutCore::StateToTransitionClusterMap()
			);

	// Add all reachables states and its transitions to result nfa
	for (auto& state : reachableStates) {
		if (this->IsStateAccept(state)) {
			result.SetStateAccept(state);
		}
		auto stateToClusterIterator = transitions_->find(state);
		if (stateToClusterIterator == transitions_->end()) {
			continue;
		}
		result.transitions_->insert(std::make_pair(state,stateToClusterIterator->second));
	}

	return result;
}
