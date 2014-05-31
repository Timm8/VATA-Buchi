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

// VATA headers
#include <vata/aut_base.hh>

#include "explicit_buchi_aut_core.hh"
#include "explicit_buchi_translate.hh"

using VATA::AutBase;
using VATA::ExplicitBuchiAutCore;

using StateBinaryRelation  = AutBase::StateBinaryRelation;

StateBinaryRelation ExplicitBuchiAutCore::ComputeDownwardSimulation(
	const SimParam&          params) const
{
	if (params.GetNumStates() != static_cast<size_t>(-1)) {
		return this->ComputeDownwardSimulation(params.GetNumStates());
	}
	else {
		throw NotImplementedException(__func__);
	}
}


StateBinaryRelation ExplicitBuchiAutCore::ComputeSimulation(
	const VATA::SimParam&                  params) const
{
	switch (params.GetRelation())
	{
		case SimParam::e_sim_relation::TA_UPWARD:
		{
			throw NotImplementedException(__func__);
		}
		case SimParam::e_sim_relation::TA_DOWNWARD:
		{
			return this->ComputeDownwardSimulation(params);
		}
		default:
		{
			throw std::runtime_error("Unknown simulation parameters: " + params.toString());
		}
	}
}

StateBinaryRelation ExplicitBuchiAutCore::ComputeDownwardSimulation(
	size_t            size) const
{
	return this->Translate().computeSimulation(size);
}

