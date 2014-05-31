/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Implementation of the ExpliciBuchiAut facade.
 * 
 *****************************************************************************/


#include <vata/vata.hh>
#include <vata/explicit_buchi_aut.hh>

#include "explicit_buchi_aut_core.hh"
#include "loadable_aut.hh"

using VATA::AutBase;
using VATA::ExplicitBuchiAut;


ExplicitBuchiAut::ExplicitBuchiAut() :
	core_(new CoreAut(CoreAut::ParentAut()))
{ }

ExplicitBuchiAut::ExplicitBuchiAut(const ExplicitBuchiAut& aut) :
	core_(new CoreAut(*aut.core_))
{ }

ExplicitBuchiAut::ExplicitBuchiAut(ExplicitBuchiAut&& aut) :
	core_(std::move(aut.core_))
{
	aut.core_ = nullptr;
}

ExplicitBuchiAut::ExplicitBuchiAut(CoreAut&& core) :
	core_(new CoreAut(std::move(core)))
{ }


ExplicitBuchiAut& ExplicitBuchiAut::operator=(const ExplicitBuchiAut& rhs)
{
	if (this != &rhs) {
		assert(nullptr != core_);

		*core_ = *rhs.core_;
	}
	return *this;
}

ExplicitBuchiAut& ExplicitBuchiAut::operator=(ExplicitBuchiAut&& rhs)
{
	assert(this != &rhs);
	assert(nullptr != core_);
	assert(nullptr != rhs.core_);
	core_ = std::move(rhs.core_);
	return *this;
}

ExplicitBuchiAut::~ExplicitBuchiAut()
{ }

void ExplicitBuchiAut::SetStateAccept(const StateType& state)
{
	assert(nullptr != core_);
	core_->SetStateAccept(state);
}

void ExplicitBuchiAut::SetStateInitial(
	const StateType&       state,
	const SymbolType&      symbol)
{
	assert(nullptr != core_);
	core_->SetStateInitial(state,symbol);
}

// Set start state with set of symbols in start transitions
void ExplicitBuchiAut::SetExistingStateInitial(
	const StateType&        state,
	const SymbolSet&        symbolSet)
{
	assert(nullptr != core_);
	core_->SetExistingStateInitial(state,symbolSet);
}


const ExplicitBuchiAut::SymbolSet& ExplicitBuchiAut::GetInitialSymbols(
		StateType state) const
{
	assert(nullptr != core_);
	return core_->GetStartSymbols(state);
}

void ExplicitBuchiAut::AddTransition(
	const StateType&           lstate,
	const SymbolType&          symbol,
	const StateType&           rstate)
{
	assert(nullptr != core_);
	core_->AddTransition(lstate, symbol, rstate);
}

ExplicitBuchiAut::AlphabetType& ExplicitBuchiAut::GetAlphabet()
{
	assert(nullptr != core_);
	return core_->GetAlphabet();
}

const ExplicitBuchiAut::AlphabetType& ExplicitBuchiAut::GetAlphabet() const
{
	assert(nullptr != core_);
	return core_->GetAlphabet();
}

ExplicitBuchiAut ExplicitBuchiAut::ReindexStates(
	StateToStateTranslWeak&    stateTransl) const
{
	assert(nullptr != core_);
	return ExplicitBuchiAut(core_->ReindexStates(stateTransl));
}


template <class Index>
void ExplicitBuchiAut::ReindexStates(ExplicitBuchiAut& dst, Index& index) const
{
	assert(nullptr != core_);
	core_->ReindexStates(*dst.core_,index);
}

const ExplicitBuchiAut::StateSet& ExplicitBuchiAut::GetInitialStates() const
{
	assert(nullptr != core_);
	return core_->GetInitialStates();
}


ExplicitBuchiAut ExplicitBuchiAut::Union(
		const ExplicitBuchiAut&        lhs,
		const ExplicitBuchiAut&        rhs,
		AutBase::StateToStateMap*       pTranslMapLhs,
		AutBase::StateToStateMap*       pTranslMapRhs)
{
	assert(nullptr != lhs.core_);
	assert(nullptr != rhs.core_);
	return ExplicitBuchiAut(CoreAut::Union(
			*lhs.core_,*rhs.core_,pTranslMapLhs,pTranslMapRhs));
}

ExplicitBuchiAut ExplicitBuchiAut::UnionDisjointStates(
	const ExplicitBuchiAut&          lhs,
	const ExplicitBuchiAut&          rhs)
{
	assert(nullptr != lhs.core_);
	assert(nullptr != rhs.core_);
	return ExplicitBuchiAut(CoreAut::UnionDisjointStates(
			*lhs.core_,*rhs.core_));
}


ExplicitBuchiAut ExplicitBuchiAut::Intersection(
		const VATA::ExplicitBuchiAut   &lhs,
		const VATA::ExplicitBuchiAut   &rhs,
		AutBase::ProductTranslMap* pTranslMap)
{
	assert(nullptr != lhs.core_);
	assert(nullptr != rhs.core_);
	return ExplicitBuchiAut(CoreAut::Intersection(
				*lhs.core_,*rhs.core_,pTranslMap));
}

ExplicitBuchiAut ExplicitBuchiAut::RemoveUnreachableStates(
	VATA::AutBase::StateToStateMap*   pTranslMap)
{
	assert(nullptr != core_);
	return ExplicitBuchiAut(core_->RemoveUnreachableStates(pTranslMap));
}

ExplicitBuchiAut ExplicitBuchiAut::RemoveUselessStates(
	VATA::AutBase::StateToStateMap*   pTranslMap)
{
	assert(nullptr != core_);
	return ExplicitBuchiAut(core_->RemoveUselessStates(pTranslMap));
}

ExplicitBuchiAut ExplicitBuchiAut::GetCandidateTree() const
{
	assert(nullptr != core_);
	return ExplicitBuchiAut(core_->GetCandidateTree());
}

ExplicitBuchiAut ExplicitBuchiAut::Reverse(
		AutBase::StateToStateMap* pTranslMap) const
{
	assert(nullptr != core_);
	return ExplicitBuchiAut(core_->Reverse(pTranslMap));
}


void ExplicitBuchiAut::LoadFromString(
	VATA::Parsing::AbstrParser&      parser,
	const std::string&               str,
	const std::string&               params)
{
	assert(nullptr != core_);
	core_->LoadFromAutDesc(parser.ParseString(str), params);
}


/*
 ** Function loads automaton to the intern representation
 ** from the string.
 ** It translates from string to the automaton descrtiption
 ** data structure and the calls another function.
 **
 */
void ExplicitBuchiAut::LoadFromString(
	VATA::Parsing::AbstrParser&      parser,
	const std::string&               str,
	StateDict&                       stateDict,
	const std::string&               params)
{
	assert(nullptr != core_);
	core_->LoadFromString(parser,str, stateDict, params);
}

void ExplicitBuchiAut::LoadFromString(
	VATA::Parsing::AbstrParser&      parser,
	const std::string&               str,
	StringToStateTranslWeak&         stateTransl,
	const std::string&               params)
{
	assert(nullptr != core_);
	core_->LoadFromString(parser,str, stateTransl, params);
}


void ExplicitBuchiAut::LoadFromAutDesc(
	const AutDescription&            desc,
	const std::string&               params)
{
	assert(nullptr != core_);
	core_->LoadFromAutDesc(desc, params);
}

void ExplicitBuchiAut::LoadFromAutDesc(
	const AutDescription&            desc,
	StringToStateTranslWeak&         stateTransl,
	const std::string&               params)
{
	assert(nullptr != core_);
	core_->LoadFromAutDesc(
		desc,
		stateTransl,
		params);
}

/*
 * Loads to internal (explicit) representation from the structure given by
 * parser
 */
void ExplicitBuchiAut::LoadFromAutDesc(
	const AutDescription&            desc,
	StateDict&                       stateDict,
	const std::string&               params)
{
	assert(nullptr != core_);
	core_->LoadFromAutDesc(
		desc,
		stateDict,
		params);
}

std::string ExplicitBuchiAut::DumpToString(
	VATA::Serialization::AbstrSerializer&			serializer,
	StateBackTranslStrict&                    stateTransl,
	const std::string&                        params) const
{
	assert(nullptr != core_);
	return core_->DumpToString(serializer, stateTransl, params);
}

std::string ExplicitBuchiAut::DumpToString(
	VATA::Serialization::AbstrSerializer&			serializer,
	const StateDict&                          stateDict,
	const std::string&                        params) const
{
	assert(nullptr != core_);
	return core_->DumpToString(serializer, stateDict, params);
}

std::string ExplicitBuchiAut::DumpToString(
	VATA::Serialization::AbstrSerializer&			serializer,
	const std::string&                        params) const
{
	assert(nullptr != core_);
	return core_->DumpToString(serializer, params);
}

AutBase::StateBinaryRelation ExplicitBuchiAut::ComputeSimulation(
	const SimParam&             params) const
{
	assert(nullptr != core_);

	return core_->ComputeSimulation(params);
}


bool ExplicitBuchiAut::CheckInclusion(
	const VATA::ExplicitBuchiAut&    smaller,
	const VATA::ExplicitBuchiAut&    bigger,
	const VATA::InclParam&						params)
{
	assert(nullptr != smaller.core_);
	assert(nullptr != bigger.core_);
	return CoreAut::CheckInclusion(
				*smaller.core_,*bigger.core_,params);
}
