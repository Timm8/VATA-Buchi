/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	Header file for explicit representation of buchi automata.
 * 
 *****************************************************************************/

#ifndef _VATA_EXPLICIT_BUCHI_AUT_CORE_HH_
#define _VATA_EXPLICIT_BUCHI_AUT_CORE_HH_

// VATA headers
#include <vata/aut_base.hh>
#include <vata/explicit_buchi_aut.hh>

#include <vata/util/transl_weak.hh>
#include <vata/explicit_lts.hh>
#include <vata/incl_param.hh>

// Standard library headers
#include <unordered_set>

namespace VATA
{
	class ExplicitBuchiAutCore;
}

GCC_DIAG_OFF(effc++) // non virtual destructors warnings suppress
class VATA::ExplicitBuchiAutCore : public AutBase
{
GCC_DIAG_ON(effc++)


	template<class Rel>
	friend class RamseyBased;


public: 
	using SymbolType	= ExplicitBuchiAut::SymbolType;
	using StringSymbolType	= ExplicitBuchiAut::StringSymbolType;
	using SymbolSet 	= ExplicitBuchiAut::SymbolSet;
	using SymbolBackTranslStrict   = ExplicitBuchiAut::SymbolBackTranslStrict;
	
	//using Transition	= ExplicitBuchiAut::Transition;
	
	using AbstractAlphabet = ExplicitBuchiAut::AbstractAlphabet;
	


private: // private type definitions
	
	typedef std::string string;

	using Convert      = VATA::Util::Convert;
	using AlphabetType = ExplicitBuchiAut::AlphabetType;
	using StateSet     = ExplicitBuchiAut::StateSet;
		
	typedef AutDescription::State State;
	
	typedef std::unordered_map<StateType,SymbolSet> StateToSymbols;
	
	// The states on the right side of transitions
	typedef StateSet RStateSet;	
	/*
	 * Transition cluster maps symbol to set of states when
	 * there exists transition for a given symbol to the states.
	 */
	GCC_DIAG_OFF(effc++)
	class TransitionCluster : public std::unordered_map<SymbolType,RStateSet>{
	GCC_DIAG_ON(effc++)
	public:
		 RStateSet& uniqueRStateSet(const SymbolType &symbol) {
			auto iter = this->find(symbol);
			if (iter == this->end()) {
				return this->insert(std::make_pair(symbol, RStateSet())).first->second;
			}
			else {
			 return iter->second;
			}
		}
	};
	typedef std::shared_ptr<TransitionCluster> TransitionClusterPtr;

	/*
	 * Maps a state to transition cluster, so all transitions
	 * for given state are stored in the transition cluster.
	 */
	GCC_DIAG_OFF(effc++)
	class StateToTransitionClusterMap :
		public std::unordered_map<StateType,TransitionClusterPtr>{
	GCC_DIAG_ON(effc++)
	public:
		const TransitionClusterPtr &uniqueCluster(const StateType &state) {
			auto& clusterPtr = this->insert(
				std::make_pair(state,TransitionClusterPtr(nullptr))
					).first->second;
				if (!clusterPtr) {
					clusterPtr = TransitionClusterPtr(new TransitionCluster());
				}
				else if (!clusterPtr.unique()) {
					clusterPtr = TransitionClusterPtr(new TransitionCluster(*clusterPtr));
				}
				return clusterPtr;
		}
	};

	typedef std::shared_ptr<StateToTransitionClusterMap> StateToTransitionClusterMapPtr;

	
protected: //Private data memebers

	StateSet acceptStates_;
	StateSet initialStates_;
	
	//Transitions defining initial states
	StateToSymbols initialStateToSymbols_;
	

private: //Private static data memebers

	StateToTransitionClusterMapPtr transitions_;

	AlphabetType alphabet_;

	static AlphabetType globalAlphabet_;

public:   //Methods
  
	StateToTransitionClusterMapPtr trans = transitions_;
	
	explicit ExplicitBuchiAutCore(AlphabetType& alphabet = globalAlphabet_);
	
	ExplicitBuchiAutCore(const ExplicitBuchiAutCore& aut);

	ExplicitBuchiAutCore& operator=(const ExplicitBuchiAutCore& rhs);

	//ExplicitBuchiAutCore& operator=(const ExplicitBuchiAutCore&& rhs);
	
	~ExplicitBuchiAutCore()
	{ }
		

	template <
		class StateTranslFunc,
		class SymbolTranslFunc>
	void loadFromAutDescInternal(
		const AutDescription&            desc,
		StateTranslFunc&                 stateTransl,
		SymbolTranslFunc&                symbolTransl,
		const std::string&               /* params */ = "")
	{
		for (auto symbolRankPair : desc.symbols) {
			symbolTransl(symbolRankPair.first);
		}	
		
		//Load accept states
		for (auto s : desc.finalStates) { //Accept states extraction
			this->acceptStates_.insert(stateTransl(s));
		}
		
		//Load transitions
		for (auto t : desc.transitions) {
			const std::string& symbol = t.second;
			const State& rightState = t.third;

			//Check whether there are no start states
			if (t.first.empty()) {
				StateType translatedState = stateTransl(rightState);

				SymbolType translatedSymbol = symbolTransl(symbol);

				SetStateInitial(translatedState, translatedSymbol);

				continue;
			}

			if (t.first.size() != 1) { //Symbols only with arity one
				throw std::runtime_error("Not a buchi automaton");
			}

			//Load here because t could be empty
			const State& leftState = t.first[0];

			this->AddTransition(
				stateTransl(leftState),
				symbolTransl(symbol),
				stateTransl(rightState));
		}	
	}

	/*
	 * Function converts the internal automaton description
	 * to a string.
	 */
	template <
		class StateBackTranslFunc>
	AutDescription dumpToAutDescInternal(
		StateBackTranslFunc                       stateTransl,
		const AlphabetType&                       alphabet,
		const std::string&                        /* params */ = "") const
	{
		assert(nullptr != alphabet);
		
		AbstractAlphabet::BwdTranslatorPtr symbolTransl =
			alphabet->GetSymbolBackTransl();
		assert(nullptr != symbolTransl);

		AutDescription desc;
		
		//Dump the accept states
		for (auto& s : this->acceptStates_) {
			desc.finalStates.insert(stateTransl(s));
		}
		
		//Dump start states
		std::string x("x"); //Initial input symbol
		std::vector<std::string> leftStateAsTuple;
		for (auto &s : this->initialStates_) {
			SymbolSet SymSet = this->GetStartSymbols(s);
			
			if (!SymSet.size()) { //No start symbols are defined,
				AutDescription::Transition Trans(
					leftStateAsTuple,
					x,
					stateTransl(s));
				desc.transitions.insert(Trans);
			}
			else { //Print all starts symbols
				for (auto &Sym : SymSet) {
					AutDescription::Transition Trans(
						leftStateAsTuple,
						(*symbolTransl)(Sym),
						stateTransl(s));
					desc.transitions.insert(Trans);
					break;
				}
			}	
		}	
		
		
		/*
		 * Converts transitions to data structure for serializer
		 */
		for (auto& ls : *(this->transitions_)) {
			for (auto& s : *ls.second) {
				for (auto& rs : s.second) {
					std::vector<std::string> leftStateAsTuple;
					leftStateAsTuple.push_back(stateTransl(ls.first));
					AutDescription::Transition trans(
						leftStateAsTuple,
						(*symbolTransl)(s.first),
						stateTransl(rs));
					desc.transitions.insert(trans);
				}
			}
		}
		
		return desc;	
	}



public: //Public static functions
	
	ExplicitBuchiAutCore ReindexStates(
		StateToStateTranslWeak&    stateTransl) const
	{
		ExplicitBuchiAutCore res;
		this->ReindexStates(res, stateTransl);
		return res;
	}
	
	/*
	 * The current indexes for states are transform to the new ones,
	 * and stored to the new automaton
	 */
	template <class Index>
	void ReindexStates(ExplicitBuchiAutCore& dst, Index& index) const {

		//Converts the accept states
		for (auto& state : this->acceptStates_)	{
			dst.SetStateAccept(index[state]);
		}

		//Converts the initial states
		for( auto& state : this->initialStates_) {
			dst.SetExistingStateInitial(index[state],GetStartSymbols(state));
		}

		auto clusterMap = dst.uniqueClusterMap();

		/*
		 * Conversion of all states and symbols that are in
		 * transitions.
		 */
		for (auto& stateClusterPair : *this->transitions_) {
			//assert(stateClusterPair.second);
			auto cluster = clusterMap->uniqueCluster(index[stateClusterPair.first]);
			for (auto& symbolRStateSetPair : *stateClusterPair.second) {
				//assert(symbolTupleSetPair.second);
				RStateSet& rstatesSet = cluster->uniqueRStateSet(symbolRStateSetPair.first);
				for (auto& rState : symbolRStateSetPair.second) {
					rstatesSet.insert(index[rState]);
				}
			}
		}
	}
	
	void AddTransition(
		const StateType&           lstate,
		const SymbolType&          symbol,
		const StateType&           rstate) 
	{
		this->IntAddTransition(lstate, symbol, rstate);
	}

	/*
	 * Get from tree automata part of library
	 */
	template <class T>
	static const typename T::mapped_type::element_type* genericLookup(
		const T&                         cont,
		const typename T::key_type&      key)
	{
		auto iter = cont.find(key);
		if (iter == cont.end())
			return nullptr;
		return iter->second.get();

	}	
	
	
	AlphabetType& GetAlphabet() {
		assert(nullptr != alphabet_);

		return alphabet_;
	}

	const AlphabetType& GetAlphabet() const {
		assert(nullptr != alphabet_);

		return alphabet_;
	}

	void SetAlphabet(AlphabetType& alphabet) {
		// Assertions
		assert(nullptr != alphabet);

		alphabet_ = alphabet;
	}
	
	bool IsStateAccept(const StateType &state) const {
		return (acceptStates_.find(state) != acceptStates_.end());
	}

	void SetStateAccept(const StateType& state) {
		this->acceptStates_.insert(state);
	}	
	
	const StateSet& GetAcceptStates() const {
		return acceptStates_;
	}

	bool IsStateInitial(const StateType &state) const	{
		return (initialStates_.find(state) != initialStates_.end());
	}
	
	const StateSet& GetInitialStates() const {
		return this->initialStates_;
	}
	
public: // Public setter
	
	void SetStateInitial(
		const StateType&       state,
		const SymbolType&      symbol)
	{
		initialStates_.insert(state);

		//Add start transition
		if (!initialStateToSymbols_.count(state)) {
			initialStateToSymbols_.insert(
				 std::make_pair(state,std::unordered_set<SymbolType>())).
				 first->second.insert(symbol);
		}
		else { //Just add new symbol
			initialStateToSymbols_.find(state)->second.insert(symbol);
		}
	}
	

	// Set start state with set of symbols in start transitions
	void SetExistingStateInitial(
		const StateType&        state,
		const SymbolSet&        symbolSet)
	{
		initialStates_.insert(state);
		assert(!initialStateToSymbols_.count(state));
		initialStateToSymbols_.insert(std::make_pair(state,symbolSet));
	}

public: // Getters

	// Return a set of the symbols which are in start transitions
	// for given state
	const SymbolSet& GetStartSymbols(StateType state) const	{
		assert(initialStateToSymbols_.find(state) != initialStateToSymbols_.end());
		return initialStateToSymbols_.find(state)->second;
	}
	
	ExplicitBuchiAutCore RemoveUnreachableStates(
			VATA::AutBase::StateToStateMap* pTranslMap = nullptr);


	ExplicitBuchiAutCore RemoveUselessStates(
			VATA::AutBase::StateToStateMap* pTranslMap = nullptr);
	
	
	ExplicitBuchiAutCore GetCandidateTree() const;

	
	template <class Dict>
	ExplicitBuchiAutCore Complement(
		const                     Dict&) const
	{
		throw NotImplementedException(__func__);
	}


	template <class Index = Util::IdentityTranslator<StateType>>
	ExplicitLTS Translate(const Index&                          stateIndex = Index()) const;
	
	/*
	 * Creates union of two automata. It just reindexes
	 * existing states of both automata to a new one.
	 * Reindexing of states is not done in this function, this
	 * function just prepares translators.
	 */
	static ExplicitBuchiAutCore Union(
		const ExplicitBuchiAutCore& lhs,
		const ExplicitBuchiAutCore& rhs,
		AutBase::StateToStateMap* pTranslMapLhs = nullptr,
		AutBase::StateToStateMap* pTranslMapRhs = nullptr);
	
	static ExplicitBuchiAutCore UnionDisjointStates(
		const ExplicitBuchiAutCore& lhs,
		const ExplicitBuchiAutCore& rhs);
	
	static VATA::ExplicitBuchiAutCore Intersection(
			const VATA::ExplicitBuchiAutCore &lhs,
			const VATA::ExplicitBuchiAutCore &rhs,
			AutBase::ProductTranslMap* pTranslMap = nullptr);


	static bool CheckInclusion(
		const VATA::ExplicitBuchiAutCore&    smaller,
		const VATA::ExplicitBuchiAutCore&    bigger,
		const VATA::InclParam&                params);
	
	ExplicitBuchiAutCore Reverse(
			AutBase::StateToStateMap* pTranslMap = nullptr) const;
	

protected:

	// Returns pointer to map where state is mapped to transition cluster
	const StateToTransitionClusterMapPtr& uniqueClusterMap()
	{
		if (!transitions_.unique()) {
			transitions_ = StateToTransitionClusterMapPtr(
				new StateToTransitionClusterMap(*transitions_));
		}
		return transitions_;
	}

	/*
	 * Add internal transition to the automaton
	 */
	void IntAddTransition(
		const StateType&         lstate,
		const SymbolType&        symbol,
		const StateType&         rstate)
	{
		this->uniqueClusterMap()->uniqueCluster(lstate)->
			uniqueRStateSet(symbol).insert(rstate);
	}	

	/***************************************************
	 * Simulation functions
	 */
public:   // methods


	/*template <class Index>
	AutBase::StateBinaryRelation ComputeDownwardSimulation(
		size_t              size )
	{
		AutBase::StateBinaryRelation relation;
		std::vector<std::vector<size_t>> partitionte(1);
		return Translate(partition, relation, index = Index()).computeSimulation(size);
	}*/


	AutBase::StateBinaryRelation ComputeDownwardSimulation(
		const SimParam&          params) const;


	template <class Index>
	AutBase::StateBinaryRelation ComputeDownwardSimulation(
		size_t            size,
		const Index&      index) const;

	AutBase::StateBinaryRelation ComputeDownwardSimulation(
		size_t            size) const;
		
	AutBase::StateBinaryRelation ComputeSimulation(
		const VATA::SimParam&          params) const;

		
		
	
};	
	
#endif