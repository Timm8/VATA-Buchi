/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Intersection for explicitly represented buchi automata.
 *
 *****************************************************************************/

// VATA headers
#include <vata/vata.hh>

#include "explicit_buchi_aut_core.hh"

//Standard library headers
#include <vector>

VATA::ExplicitBuchiAutCore VATA::ExplicitBuchiAutCore::Intersection(
		const VATA::ExplicitBuchiAutCore &lhs,
		const VATA::ExplicitBuchiAutCore &rhs,
		AutBase::ProductTranslMap* pTranslMap) {
	
	typedef typename ExplicitBuchiAutCore::StateSet StateSet;

	struct val {
		const AutBase::ProductTranslMap::value_type* node;
		int numb;
	};
	
	AutBase::ProductTranslMap translMap;	
	
	if (nullptr == pTranslMap) {
		pTranslMap = &translMap;
	}
	ExplicitBuchiAutCore result;

	std::vector<val> stack;
	
	val pom;
	
	for (auto lss : lhs.initialStates_) {
		for (auto rss : rhs.initialStates_) {
			auto iss = pTranslMap->insert(std::make_pair(std::make_pair(lss,rss), pTranslMap->size())).first; // intersection start state
			pom.node = &*iss;
			pom.numb = 0;
			stack.push_back(pom);
		}
	}
	
	typename ExplicitBuchiAutCore::StateToTransitionClusterMapPtr transitions = result.transitions_;
	
	while (!stack.empty()) {
		pom = stack.back();
		auto actState = pom.node;
		int actNumb = pom.numb;
		stack.pop_back();
	
		//Get transition clusters for given state
		auto lcluster = ExplicitBuchiAutCore::genericLookup (*lhs.transitions_,actState->first.first);
		if (!lcluster) {
			continue;
		}
		
		auto rcluster = ExplicitBuchiAutCore::genericLookup (*rhs.transitions_,actState->first.second);
		if (!rcluster) { //No such transition in right ba
			continue;
		}

		typename ExplicitBuchiAutCore::TransitionClusterPtr clusterptr(nullptr);
		StateSet st;
	
		//Go through transitions of the given state
		for (auto lsymbolToPtrPointer : *lcluster) {

			auto lsymbol = lsymbolToPtrPointer.first;
			auto tempIter = rcluster->find(lsymbol);
			if (tempIter == rcluster->end()) {
				continue;
			}
			
			auto rstateSet = tempIter->second;
			
			//Adding only usefull new state - for change move these two cycles before main cycle
			if (lhs.IsStateInitial(actState->first.first)) {
				for (auto& leftStartSymbol : lhs.GetStartSymbols(actState->first.first)) {
					result.SetStateInitial(actState->second,leftStartSymbol);
				}
			}

			if (rhs.IsStateInitial(actState->first.second)) {
				for (auto& rightStartSymbol : rhs.GetStartSymbols(actState->first.second)) {
					result.SetStateInitial(actState->second,rightStartSymbol);
				}
			}	
				
			//Get the transitions from the result automaton
			if (!clusterptr) { //Insert a new translated state
				clusterptr = transitions->uniqueCluster(actState->second);
			}
			

			if (actNumb == 2) {
				result.SetStateAccept(actState->second);
			}	
			
			//Insert a new symbol
			auto& stateSet = clusterptr->uniqueRStateSet(lsymbol);
			st = stateSet;

			for (auto lstate : lsymbolToPtrPointer.second) {
				for (auto rstate : rstateSet) {
					//Translate to intersection state
					auto istate = pTranslMap->insert
						(std::make_pair(std::make_pair(lstate,rstate),pTranslMap->size()));

					//Insert state from right side of transition
					stateSet.insert(istate.first->second);
					
					if (actNumb == 0) {
						if (lhs.IsStateAccept(istate.first->first.first)) {
							actNumb = 1;
						}
					}	
					else if (actNumb == 1) {
						if (rhs.IsStateAccept(istate.first->first.second)) {
							actNumb = 2;				
						}
						else {
							actNumb = 1;
						}	
					}
					
					if (istate.second) { //New states added to stack
						pom.node = &*istate.first;
						pom.numb = actNumb;
						stack.push_back(pom);
					}
				}
			}
		}
	}	
	
	return result;
}