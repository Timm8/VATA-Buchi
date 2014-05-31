/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Checking language inclusion for explicitly represented buchi automata.
 *
 *****************************************************************************/


// VATA headers
#include <vata/incl_param.hh>

#include "explicit_buchi_aut_core.hh"
#include "explicit_buchi_ramsey_incl.hh"

#include <vata/vata.hh>
#include <vata/explicit_buchi_aut.hh>


using VATA::ExplicitBuchiAutCore;


namespace VATA
{
	template<class Rel>
	bool CheckBuchiAutInclusion(
		const ExplicitBuchiAutCore& smaller,
		const ExplicitBuchiAutCore& bigger,
		const Rel& preorder);
	
}

bool ExplicitBuchiAutCore::CheckInclusion(
	const ExplicitBuchiAutCore&             smaller,
	const ExplicitBuchiAutCore&             bigger,
	const VATA::InclParam&                 params)
{
	
	
	ExplicitBuchiAutCore newSmaller;
	ExplicitBuchiAutCore newBigger;
	typename AutBase::StateType states = static_cast<typename AutBase::StateType>(-1);

	if (!params.GetUseSimulation()) {
		newSmaller = smaller;
		newBigger = bigger;

		states = AutBase::SanitizeAutsForInclusion(newSmaller, newBigger);
	
	}
	
	
	
	switch (params.GetOptions()) {	
		
		/*case InclParam::RAMSEY_INCL:
		{
			throw NotImplementedException("Inclusion:\n" +
				params.toString());
		}
		case InclParam::RANK_INCL:
		{
			throw NotImplementedException("Inclusion:\n" +
				params.toString());
		}*/	
		
		//this is for default setting of inclusion parameter 
		//Ramsey-based is use instead antichain
		case InclParam::ANTICHAINS_UP_NOSIM:
		{
			assert(static_cast<typename AutBase::StateType>(-1) != states);

			typedef VATA::Util::Identity Rel;
			
			return VATA::CheckBuchiAutInclusion<Rel>(newSmaller, newBigger, VATA::Util::Identity(states));
		}
		default:
		{
			throw NotImplementedException("Inclusion:\n" +
				params.toString());
		}	
	}
	
}


/*
 * Function wrapping inclusion checking
 */
template<class Rel>
bool VATA::CheckBuchiAutInclusion(
	const VATA::ExplicitBuchiAutCore& smaller,
	const VATA::ExplicitBuchiAutCore& bigger,
	const Rel& preorder) {

	typedef typename VATA::Util::Identity::IndexType IndexType;

	IndexType index;
	IndexType inv;
	
	preorder.buildIndex(index,inv);
	
	VATA::RamseyBased <Rel>RamseyBased(smaller,bigger,index,inv);

	return RamseyBased.Ramsey_Incl_Check();
	
}