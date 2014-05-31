/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Get candidate for buchi automata in explicit encoding.
 *
 *****************************************************************************/

// VATA headers
#include <vata/vata.hh>
#include "explicit_buchi_aut_core.hh"

// std library headers
#include <list>
#include <unordered_map>

/*
 * Get candidate ba. It is the smallest ba which
 * has language which is subset of the input ba
 */
VATA::ExplicitBuchiAutCore VATA::ExplicitBuchiAutCore::GetCandidateTree() const {


	std::unordered_set<ExplicitBuchiAutCore::StateType> reachableStates;
	std::list<ExplicitBuchiAutCore::StateType> newStates;

	ExplicitBuchiAutCore result;


	//Starts from initial states
	for (StateType s : this->GetInitialStates()) {
		if (reachableStates.insert(s).second) {
			newStates.push_back(s);
		}
		result.SetExistingStateInitial(s, this->GetStartSymbols(s));
	}

	while (!newStates.empty()) {
		StateType actState = newStates.front();
		//Find transitions for state s
		auto transitionsCluster = transitions_->find(actState);

		newStates.pop_front();

		if (transitionsCluster == transitions_->end()) {
			continue;
		}

		for (auto symbolToState : *transitionsCluster->second) {
			//All states which are reachable from actState are added to result automaton
			for (auto stateInSet : symbolToState.second){

				if (reachableStates.insert(stateInSet).second) {
					newStates.push_back(stateInSet);
				}

				if (this->IsStateAccept(stateInSet)) { //Set accept state and return
					result.SetStateAccept(stateInSet);
					result.transitions_->insert(std::make_pair(actState,transitionsCluster->second));

					return result.RemoveUselessStates();
				}
				//Add to transitions
				result.transitions_->insert(std::make_pair(actState,transitionsCluster->second));
			}
		}
	}

	return result.RemoveUselessStates();
}
