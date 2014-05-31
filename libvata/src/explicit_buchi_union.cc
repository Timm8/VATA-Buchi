/*********************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Implementation of union operation for explicit buchi omega word automata.
 *
 *********************************************************************************/

#include "explicit_buchi_aut_core.hh"

using VATA::ExplicitBuchiAutCore;

ExplicitBuchiAutCore ExplicitBuchiAutCore::Union(
	const ExplicitBuchiAutCore& lhs,
	const ExplicitBuchiAutCore& rhs,
	AutBase::StateToStateMap* pTranslMapLhs,
	AutBase::StateToStateMap* pTranslMapRhs)
{
	/*
	 * If the maps are not given
	 * it creates own new maps
	 */
	StateToStateMap translMapLhs;
	StateToStateMap translMapRhs;

	if (!pTranslMapLhs) {
		pTranslMapLhs = &translMapLhs;
	}

	if (!pTranslMapRhs) {
		pTranslMapRhs = &translMapRhs;
	}

	// New translation function
	StateType stateCnt = 0;
	auto translFunc = [&stateCnt](const StateType&){return stateCnt++;};

	StateToStateTranslWeak stateTransLhs(*pTranslMapLhs, translFunc);
	StateToStateTranslWeak stateTransRhs(*pTranslMapRhs, translFunc);

	ExplicitBuchiAutCore result;

	lhs.ReindexStates(result, stateTransLhs);
	rhs.ReindexStates(result, stateTransRhs);

	return result;
}

ExplicitBuchiAutCore ExplicitBuchiAutCore::UnionDisjointStates(
	const ExplicitBuchiAutCore &lhs,
	const ExplicitBuchiAutCore &rhs)
{
	ExplicitBuchiAutCore result(lhs);

	// Use uniqueCluster function, not explicitly transitions_,
	// because of the possibility of the need of creating the
	// new clusterMap
	result.uniqueClusterMap()->insert(rhs.transitions_->begin(),
		rhs.transitions_->end());
	assert(lhs.transitions_->size() + rhs.transitions_->size() == result.transitions_->size());

	result.initialStates_.insert(rhs.initialStates_.begin(),
		rhs.initialStates_.end());

	result.initialStateToSymbols_.insert(rhs.initialStateToSymbols_.begin(),
		rhs.initialStateToSymbols_.end());

	result.acceptStates_.insert(rhs.acceptStates_.begin(),
		rhs.acceptStates_.end());
	return result;
}
