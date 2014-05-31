/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Header file for the ExplicitBuchiAut facade.
 * 
 *****************************************************************************/

#ifndef _VATA_EXPLICIT_BUCHI_AUT_HH_
#define _VATA_EXPLICIT_BUCHI_AUT_HH_

// VATA headers
#include <vata/vata.hh>
#include <vata/aut_base.hh>

#include <vata/incl_param.hh>
#include <vata/sim_param.hh>
#include <vata/explicit_lts.hh>

#include <vata/util/convert.hh>
#include <vata/parsing/abstr_parser.hh>
#include <vata/serialization/abstr_serializer.hh>

#include <vata/util/transl_weak.hh>
#include <vata/util/transl_strict.hh>

namespace VATA
{
	class ExplicitBuchiAut;

	template <class>
		class LoadableAut;

	class ExplicitBuchiAutCore;
}

GCC_DIAG_OFF(effc++) 
class VATA::ExplicitBuchiAut : public AutBase 
{
GCC_DIAG_ON(effc++)   
  
  
private: // private data types
	using CoreAut = VATA::LoadableAut<ExplicitBuchiAutCore>;
	std::unique_ptr<CoreAut> core_;

public: // public data types
	using SymbolType       = uintptr_t ;
	using SymbolSet        = std::unordered_set<SymbolType>;
	using StringSymbolType = std::string;
	
	using SymbolDict                     =
		VATA::Util::TwoWayDict<std::string, SymbolType>;
	using SymbolBackTranslStrict         =
		VATA::Util::TranslatorStrict<typename SymbolDict::MapBwdType>;
	using StringSymbolToSymbolTranslWeak = Util::TranslatorWeak<SymbolDict>;
	
	class AbstractAlphabet
	{
	public:  // data types

		using FwdTranslator    = VATA::Util::AbstractTranslator<StringSymbolType, SymbolType>;
		using FwdTranslatorPtr = std::unique_ptr<FwdTranslator>;
		using BwdTranslator    = VATA::Util::AbstractTranslator<SymbolType, StringSymbolType>;
		using BwdTranslatorPtr = std::unique_ptr<BwdTranslator>;

	public:  // methods

		virtual FwdTranslatorPtr GetSymbolTransl() = 0;
		virtual BwdTranslatorPtr GetSymbolBackTransl() = 0;

	};


	class OnTheFlyAlphabet : public AbstractAlphabet
	{
	private:  // data members

		SymbolDict symbolDict_{};
		SymbolType nextSymbol_ = 0;

	public:   // methods

		virtual FwdTranslatorPtr GetSymbolTransl() override
		{
			FwdTranslator* fwdTransl = new
				StringSymbolToSymbolTranslWeak{symbolDict_,
				[&](const StringSymbolType&){return nextSymbol_++;}};

			return FwdTranslatorPtr(fwdTransl);
		}

		virtual BwdTranslatorPtr GetSymbolBackTransl() override
		{
			BwdTranslator* bwdTransl =
				new SymbolBackTranslStrict(symbolDict_.GetReverseMap());

			return BwdTranslatorPtr(bwdTransl);
		}

	};

	using AlphabetType = std::shared_ptr<AbstractAlphabet>;
	
	// Stateset is unordered_set with operation for checking subset
	GCC_DIAG_OFF(effc++)
	class StateSet : public std::unordered_set<StateType>	 {
	GCC_DIAG_ON(effc++)
	public:
		bool IsSubsetOf(const StateSet& rhs) const {
			for (StateType state : *this) {
				if (!rhs.count(state)) { // counterexample found
					return false;
				}
			}
			return true;
		}
	};
	

	
public: // loading automaton methods
	
	void LoadFromString(
		VATA::Parsing::AbstrParser&       parser,
		const std::string&                str,
		const std::string&                params = "");
	
	void LoadFromString(
		VATA::Parsing::AbstrParser&      parser,
		const std::string&               str,
		StateDict&                       stateDict,
		const std::string&               params = "");
	
	void LoadFromString(
		VATA::Parsing::AbstrParser&      parser,
		const std::string&               str,
		StringToStateTranslWeak&         stateTransl,
		const std::string&               params = "");

	void LoadFromAutDesc(
		const AutDescription&            desc,
		const std::string&               params = "");
	
	void LoadFromAutDesc(
		const AutDescription&            desc,
		StringToStateTranslWeak&         stateTransl,
		const std::string&               params = "");
	
	void LoadFromAutDesc(
		const AutDescription&            desc,
		StateDict&                       stateDict,
		const std::string&               params = "");
	
	std::string DumpToString(
		VATA::Serialization::AbstrSerializer&			serializer,
		StateBackTranslStrict&                    stateTransl,
		const std::string&                        params = "") const;
	std::string DumpToString(
		VATA::Serialization::AbstrSerializer&			serializer,
		const StateDict&                          stateDict,
		const std::string&                        params = "") const;
	std::string DumpToString(
		VATA::Serialization::AbstrSerializer&			serializer,
		const std::string&                        params = "") const;

	
public: // Constructors and operators

	ExplicitBuchiAut();
	ExplicitBuchiAut(const ExplicitBuchiAut& aut);
	ExplicitBuchiAut(ExplicitBuchiAut&& aut);
	explicit ExplicitBuchiAut(CoreAut&& aut);

	ExplicitBuchiAut& operator=(const ExplicitBuchiAut& rhs);
	ExplicitBuchiAut& operator=(ExplicitBuchiAut&& rhs);

	~ExplicitBuchiAut();
	
	void AddTransition(
		const StateType&           lstate,
		const SymbolType&          symbol,
		const StateType&           rstate);

	void SetStateAccept(const StateType& state);
	void SetStateInitial(const StateType& state, const SymbolType& symbol);
	
	void SetExistingStateInitial(const StateType& state, const SymbolSet& symbol);
	const SymbolSet& GetInitialSymbols(StateType state) const;

	AlphabetType& GetAlphabet();
	const AlphabetType& GetAlphabet() const;
	void SetAlphabet(AlphabetType& alphabet);
	
	const StateSet& GetInitialStates() const;
	const StateSet& GetAcceptStates() const;
	
	ExplicitBuchiAut ReindexStates (StateToStateTranslWeak& stateTransl) const;
	
	template <class Index>
	void ReindexStates(ExplicitBuchiAut& dst, Index& index) const;
	
	ExplicitBuchiAut RemoveUnreachableStates(
			VATA::AutBase::StateToStateMap* pTranslMap = nullptr);

	ExplicitBuchiAut RemoveUselessStates(
			VATA::AutBase::StateToStateMap* pTranslMap = nullptr);

	ExplicitBuchiAut GetCandidateTree() const;
		
	
	/*
	 * Creates union of two automata. It just reindexs
	 * existing states of both automata to a new one.
	 * Reindexing of states is not done in this function, this
	 * function just prepares translators.
	 */
	static ExplicitBuchiAut Union(
		const ExplicitBuchiAut& lhs,
		const ExplicitBuchiAut& rhs,
		AutBase::StateToStateMap* pTranslMapLhs = nullptr,
		AutBase::StateToStateMap* pTranslMapRhs = nullptr);
	
	static VATA::ExplicitBuchiAut Intersection(
			const VATA::ExplicitBuchiAut &lhs,
			const VATA::ExplicitBuchiAut &rhs,
			AutBase::ProductTranslMap* pTranslMap = nullptr);
	
	template <class Dict>
	ExplicitBuchiAut Complement(
			const Dict &)
	{
		throw NotImplementedException(__func__);
	}
	
	
	static ExplicitBuchiAut UnionDisjointStates(
		const ExplicitBuchiAut& lhs,
		const ExplicitBuchiAut& rhs);

	static bool CheckInclusion(
		const ExplicitBuchiAut&    smaller,
		const ExplicitBuchiAut&    bigger,
		const InclParam&            params);
	
	ExplicitBuchiAut Reverse(
			AutBase::StateToStateMap* pTranslMap = nullptr) const;
		
	ExplicitBuchiAut Reduce() const
	{
		throw NotImplementedException(__func__);
	}

	AutBase::StateBinaryRelation ComputeSimulation(
		const VATA::SimParam&                  params) const;
		
	
};

#endif