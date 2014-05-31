/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Source file for an explicit buchi automaton.
 * 
 *****************************************************************************/

// VATA headers
#include <vata/vata.hh>
#include <vata/explicit_buchi_aut.hh>

#include "explicit_buchi_aut_core.hh"
#include "loadable_aut.hh"


using VATA::ExplicitBuchiAutCore;

//Global alphabet
ExplicitBuchiAutCore::AlphabetType ExplicitBuchiAutCore::globalAlphabet_ =
	AlphabetType(new ExplicitBuchiAut::OnTheFlyAlphabet);

ExplicitBuchiAutCore::ExplicitBuchiAutCore(AlphabetType& alphabet) :
	acceptStates_(),
	initialStates_(),
	initialStateToSymbols_(),
	transitions_(StateToTransitionClusterMapPtr(new StateToTransitionClusterMap)),
	alphabet_(alphabet)
{ }

ExplicitBuchiAutCore::ExplicitBuchiAutCore(const ExplicitBuchiAutCore& aut) :
	acceptStates_(aut.acceptStates_),
	initialStates_(aut.initialStates_),
	initialStateToSymbols_(aut.initialStateToSymbols_),
	transitions_(aut.transitions_),
	alphabet_(aut.alphabet_)
{ }

ExplicitBuchiAutCore& ExplicitBuchiAutCore::operator=(const ExplicitBuchiAutCore& rhs)
{
	if (this != &rhs) {
		acceptStates_          	= rhs.acceptStates_;
		initialStates_          = rhs.initialStates_;
		initialStateToSymbols_  = rhs.initialStateToSymbols_;
		transitions_          	= rhs.transitions_;
		alphabet_            	= rhs.alphabet_;
	}

	return *this;
}