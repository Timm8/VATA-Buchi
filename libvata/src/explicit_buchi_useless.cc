/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Removing useless states for explicitly represented buchi automata.
 *
 *****************************************************************************/

// VATA headers
#include <vata/vata.hh>

#include "explicit_buchi_aut_core.hh"

VATA::ExplicitBuchiAutCore VATA::ExplicitBuchiAutCore::RemoveUselessStates(
		VATA::AutBase::StateToStateMap* pTranslMap) {

	//Remove useless states by applying following operations
	return this->
		RemoveUnreachableStates(pTranslMap).
		Reverse(pTranslMap).
		RemoveUnreachableStates().
		Reverse();
}