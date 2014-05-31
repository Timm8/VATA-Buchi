/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Translate to LTS for explicitly represented buchi automata.
 *
 *****************************************************************************/

#ifndef _VATA_EXPLICIT_BUCHI_AUT_TRANSL_
#define _VATA_EXPLICIT_BUCHI_AUT_TRANSL_

#include <unordered_map>
#include <boost/functional/hash.hpp>

// VATA headers
#include <vata/vata.hh>
#include <vata/aut_base.hh>
#include <vata/util/transl_strict.hh>
#include <vata/util/transl_weak.hh>
#include <vata/util/convert.hh>
#include <vata/explicit_lts.hh>

#include "explicit_buchi_aut_core.hh"

/*template <class Index>
VATA::ExplicitLTS VATA::ExplicitBuchiAutCore::Translate(const Index& stateIndex) const {

	ExplicitLTS result;
	std::unordered_map<SymbolType, size_t> symbolMap;
	size_t symbolCnt = 0;
	Util::TranslatorWeak2<std::unordered_map<SymbolType, size_t>>
		symbolTranslator(symbolMap, [&symbolCnt](const SymbolType&){ return symbolCnt++; });	
		
	assert(nullptr !=transitions_);

	//Add all transitions to LTS
	for (auto stateToCluster : *this->transitions_) { //Left state of transition
		assert(stateToCluster.second);
		size_t leftStateTranslated = stateIndex[stateToCluster.first];
		for (auto symbolToSet : *stateToCluster.second) { //Symbol of transition
			size_t symbol = symbolTranslator(symbolToSet.first);
			for (auto setState : symbolToSet.second) { //Right state of transition
				result.addTransition(leftStateTranslated,symbol,stateIndex[setState]);
			}
		}
	}	

	result.init();

	return result;
}*/

/*
 * Function translates given ba to lts and
 * creates partition and set relation
 */
template <class Index>
VATA::ExplicitLTS VATA::ExplicitBuchiAutCore::Translate(const Index& stateIndex) const {

	AutBase::StateBinaryRelation relation;
	std::vector<std::vector<size_t>> partition(1);

	VATA::ExplicitLTS res;
	std::unordered_map<SymbolType, size_t> symbolMap;
	size_t symbolCnt = 0;
	
	//Translator for new representation in lts
	Util::TranslatorWeak2<std::unordered_map<SymbolType, size_t>>
		symbolTranslator(symbolMap, [&symbolCnt](const SymbolType&){ return symbolCnt++; });

	//Checks whether are all states accept
	auto areAllStartStatesAccept = [this]() -> bool {
		for (auto fs : this->GetAcceptStates()) {
			if (!this->IsStateInitial(fs)) {
				return false;
			}
		}
		return true;
	};

	size_t base;
	
	//When all states are accept just two partitions will be created, otherwise three
	if (this->GetAcceptStates().size() > 0 &&
		(this->GetAcceptStates().size() <= this->transitions_->size() ||
			!areAllStartStatesAccept())) {
		base = 3;
	}
	else {
		base = 2;
	}

	partition.clear();
	partition.resize(base);

	//Add all accept states to the first partition
	for (auto& acceptState : this->GetAcceptStates()) {
		partition[0].push_back(stateIndex[acceptState]);
	}

	//Add all transitions to LTS
	for (auto stateToCluster : *this->transitions_) { //Left state of transition
		assert(stateToCluster.second);
		size_t leftStateTranslated = stateIndex[stateToCluster.first];
		
		//Non accept states to second partition
		if (!this->IsStateAccept(stateToCluster.first)) {
			partition[base-2].push_back(leftStateTranslated);
		}

		for (auto symbolToSet : *stateToCluster.second)	{ //Symbol of transition
			for (auto setState : symbolToSet.second) { //Right state of transition
				res.addTransition(
					leftStateTranslated,
					symbolTranslator[symbolToSet.first],
					stateIndex[setState]);
			}
		}
	}


	size_t max = 0;
	for (auto& r : partition) {
		for (auto& s : r) {
			if (max < s) {
				max = s;
			}
		}
	}
	max++;

	for (auto& initialState : this->GetInitialStates()) { //Add start transitions
		for (auto& startSymbol : this->GetStartSymbols(initialState)) {
			res.addTransition(max,
				symbolTranslator[startSymbol],stateIndex[initialState]);
		}
	}

	//Parition represents initial state
	partition[base-1].push_back(max);

	relation.resize(partition.size());
	relation.reset(false);

	//0 accepting, 1 non-accepting, 2 .. environments
	relation.set(0, 0, true);
	if (base == 3) {
		relation.set(1, 0, true);
		relation.set(1, 1, true);
	}

	relation.set(base - 1, base - 1, true); //Reflexivity of initial state
	res.init();
	return res;
}

#endif