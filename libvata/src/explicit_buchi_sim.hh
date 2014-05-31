/*****************************************************************************
 *  VATA Buchi Automata Library
 *
 *  Copyright (c) 2014  Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *  Description:
 *    Implemention of simulation computation for explicitly represented buchi 
 *    automata.
 *
 *****************************************************************************/

#ifndef _VATA_EXPLICIT_BUCHI_SIM_HH_
#define _VATA_EXPLICIT_BUCHI_SIM_HH_

#include "explicit_buchi_translate.hh"

template <class Index>
VATA::AutBase::StateBinaryRelation VATA::ExplicitBuchiAutCore::ComputeDownwardSimulation(
	size_t            size,
	const Index&      index) const
{
	VATA::AutBase::StateBinaryRelation sim =
		this->Translate(index).computeSimulation(size);

	return sim;
}

#endif
