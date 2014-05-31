/*****************************************************************************
 *	VATA Buchi Automata Library
 *
 *	Copyright (c) 2014	Tomas Lascak <xlasca00@stud.fit.vutbr.cz>
 *
 *	Description:
 *	File for Ramsey-based approach of checking inclusion between
 *	Buchi automata
 *
 *****************************************************************************/

#ifndef _VATA_EXPLICIT_BUCHI_GRAPHS_HH_
#define _VATA_EXPLICIT_BUCHI_GRAPHS_HH_

//Standard library headers
#include <vector>

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

namespace VATA
{
	template <class Rel> class RamseyBased;
}

GCC_DIAG_OFF(effc++)
template <class Rel>
class VATA::RamseyBased
{
GCC_DIAG_ON(effc++)

public: //Public members
	
	typedef VATA::ExplicitBuchiAutCore ExplicitBA;
	
	typedef typename ExplicitBA::StateType StateType;
	//typedef typename ExplicitBA::StateSet StateSet;
	typedef typename ExplicitBA::SymbolType SymbolType;
	
	typedef std::pair<StateType,StateType> Edge;
	
	typedef std::vector<std::pair<Edge,int>> Graph;
		
	typedef std::pair<Edge,Graph> SuperGraph;

	typedef typename Rel::IndexType IndexType;
	
	
private: // data memebers

	const ExplicitBA& smaller_;
	const ExplicitBA& bigger_;

	IndexType& index_;
	IndexType& inv_;


public: // constructor
		RamseyBased(
			const ExplicitBA& smaller,
			const ExplicitBA& bigger,
			IndexType& index,
			IndexType& inv) :
		smaller_(smaller),
		bigger_(bigger),
		index_(index),
		inv_(inv)
	{}
	
private: //Private methods

	
	int ExistNewWay(std::vector<Edge> Edges,StateType s1, StateType s2) {
		
		int index = -1;
		bool isnew;
		
		if (s1 == s2)
			return index;
		
		for (size_t i = 0; i < Edges.size(); i++) {
			if (s2 == Edges[i].first) {
				StateType New = Edges[i].second;
				for (size_t j = 0; j < Edges.size(); j++) {
					if ( s1 == Edges[j].first ) {
						if ( New == Edges[j].second ) {
							isnew = false;
							break;
						}	
						else
							isnew = true;
					}	
					else
						isnew = true;	
				}	
				if(isnew) {
					return i;
				}	
			}
		}
		
		return index;
	}
	
	int ExistNewWayInG(Graph G,StateType s1, StateType s2) {
		
		int index = -1;
		bool isnew;
		
		if (s1 == s2)
			return index;
		
		for (size_t i = 0; i < G.size(); i++) {
			if (s2 == G[i].first.first) {
				StateType New = G[i].first.second;
				for (size_t j = 0; j < G.size(); j++) {
					if ( s1 == G[j].first.first ) {
						if ( New == G[j].first.second ) {
							isnew = false;
							break;
						}	
						else
							isnew = true;
					}	
					else
						isnew = true;	
				}	
				if(isnew) 
					return i;
				
			}
		}
		
		return index;
	}
	
	int GetNumb(Graph G,StateType s1, StateType s2) {
		
		int index = 0;
		bool isnew;
		
		if (s1 == s2)
			return index;
		
		for (size_t i = 0; i < G.size(); i++) {
			if (s2 == G[i].first.first) {
				StateType New = G[i].first.second;
				for (size_t j = 0; j != G.size(); j++) {
					if ( s1 == G[i].first.first ) {
						if ( New == G[i].first.second ) {
							isnew = false;
							break;
						}	
						else
							isnew = true;
					}	
					else
						isnew = true;	
				}	
				if(isnew) {
					return G[i].second;
					
				}
				
			}
		}
		
		return index;
	}

	bool ExistStates(Graph G,StateType s1, StateType s2) {
		
		if (s1 == s2) {
			return true;	
		}	
		
		for (size_t i = 0; i < G.size(); i++) {
			if (s1 == G[i].first.first && s2 == G[i].first.second) {
				return true;
			}
		}
		return false;
	}
	
	/*
	 * Minimalization of Supergraph
	 */
	SuperGraph MinS(SuperGraph S) {
	//minimalizace SuperGraphu	
		
		for (size_t i = 0; i < S.second.size(); i++) {
			
			for (size_t j = 0; j < S.second.size(); j++) {
				
				if ((S.second[i].second) >= 0 && (S.second[i].first.first == S.second[j].first.first) && 
					(S.second[i].first.second != S.second[j].first.second)) {
					if (S.second[i].second >= 0 && 
					    S.second[i].second <= S.second[j].second &&
					    (bigger_.IsStateAccept(S.second[i].first.first) ||
					     (!bigger_.IsStateAccept(S.second[i].first.first) &&
					     !bigger_.IsStateAccept(S.second[j].first.first)))) {
						S.second[i].second = -1;	
					}	
					
				}
				else
					continue;
					
			}		
		}	
		
		
		return S;	
	}
	
	bool isThere(Edge E, std::vector<Edge> NonExist) {
		
		for (size_t i = 0; i < NonExist.size(); i++) { 
			if (E.first == NonExist[i].first && E.second == NonExist[i].second) {
				return true;
			}
		}
		
		return false;
	}
	
public: //Public methods
	
	/*
	 * Create all Supergraphs from input automata
	 */
	std::vector<SuperGraph> CreateSuper() {
		
		std::unordered_map<SymbolType, size_t> symbolMap;
		size_t symbolCnt = 0;

		//Translator for symbols for Edges and Graphs
		Util::TranslatorWeak2<std::unordered_map<SymbolType, size_t>>
		symbolTranslator(symbolMap, [&symbolCnt](const SymbolType&){ return symbolCnt++; });		
		
		size_t symbol;
		
		std::vector<SuperGraph> result;
		
		typedef std::vector<SymbolType> symbols;
	
		symbols symbolsA,symbolsB;
	
		typedef std::vector<Edge> Edges;
		typedef std::vector<std::pair<Edges,SymbolType>> AllEds;
		typedef std::vector<std::pair<Graph,SymbolType>> AllGraphs;
	
		AllEds AllEdges;
		AllGraphs Graphs;
		
		//Add all edges from basic clusters
		for (auto lstate: *smaller_.transitions_) { //Load all state from left
			for (auto symbolToSet : *lstate.second) { //Symbol of transition
				symbol = symbolTranslator[symbolToSet.first];
				if (std::find(symbolsA.begin(), symbolsA.end(), symbol)==symbolsA.end()) {
					AllEdges.push_back(std::make_pair(GetEdges(symbol),symbol));
					symbolsA.push_back(symbol);
				}
			}
		}
				
		//Add all graphs from basic clusters
		for (auto lstate: *bigger_.transitions_) { //Load all state from left
			for (auto symbolToSet : *lstate.second) { //Symbol of transition
				symbol = symbolTranslator[symbolToSet.first];
				if (std::find(symbolsB.begin(), symbolsB.end(), symbol)==symbolsB.end()) {
					Graphs.push_back(std::make_pair(GetGraph(symbol),symbol));
					symbolsB.push_back(symbol);
				}
			}
		}
		
		for (size_t i = 0; i < Graphs.size();i++) {
			for (size_t k = 0; k < AllEdges.size();k++) {	
				if (Graphs[i].second == AllEdges[k].second) {	
			
					for (size_t j = 0; j < AllEdges[k].first.size(); j++) {
						auto Super = std::make_pair(AllEdges[i].first[j],Graphs[i].first);
						
						result.push_back(Super);
					}
				}
			}
		}
		
		return result;
		
	}	
	
	/*
	 * Get all edges from input automaton A
	 */
	std::vector<Edge> GetEdges(SymbolType findsymbol) {
		std::vector<StateType> States;
		std::vector<Edge> result;

		std::unordered_map<SymbolType, size_t> symbolMap;
		size_t symbolCnt = 0;
	
		//Translator for symbols for Edges
		Util::TranslatorWeak2<std::unordered_map<SymbolType, size_t>>
		symbolTranslator(symbolMap, [&symbolCnt](const SymbolType&){ return symbolCnt++; });			
		
		size_t symbol;
		
		//Add all edges from basic clusters
		for (auto lstate: *smaller_.transitions_) { //Load all state from left

			for (auto symbolToSet : *lstate.second) { //Symbol of transition
				symbol = symbolTranslator[symbolToSet.first];
				if (symbol != findsymbol) { //If symbol is not that we want lets go an another symbol of alphabet
					continue;
				}		
			
				for (auto setState : symbolToSet.second) { //Right state of transition
					result.push_back(std::make_pair(lstate.first,setState));
				}
			}	
		}
		
		for (size_t i = 0; i < result.size(); i++) {	
			int index = ExistNewWay(result,result[i].first,result[i].second);

			if (index > 0) {
				result.push_back(std::make_pair(result[i].first,result[index].second));
			}
		}	
	
		return result;		
	}
	
	/*
	 * Get all graphs from input automaton B
	 */
	Graph GetGraph(SymbolType findsymbol) {
		
		std::vector<StateType> States;
		int index,ind;
		Graph result,pom;

		std::unordered_map<SymbolType, size_t> symbolMap;
		size_t symbolCnt = 0;
	
		size_t symbol;
		
		//Translator for symbols for Edges and Graphs
		Util::TranslatorWeak2<std::unordered_map<SymbolType, size_t>>
		symbolTranslator(symbolMap, [&symbolCnt](const SymbolType&){ return symbolCnt++; });		
	
		//Add all edges from basic clusters
		for (auto lstate: *bigger_.transitions_) { //Load all state from left
			for (auto symbolToSet : *lstate.second) { //Symbol of transition
				
				symbol = symbolTranslator[symbolToSet.first];
				if (findsymbol != symbol) { //If symbol is not that we want lets go an another symbol of alphabet
					continue;
				}	
				
				for (auto setState : symbolToSet.second) { //Right state of transition
					if (bigger_.IsStateAccept(setState)) {
						result.push_back(std::make_pair(std::make_pair(lstate.first,setState),1));
					}
					else { 
						result.push_back(std::make_pair(std::make_pair(lstate.first,setState),0));
					}
				}
			}	
		}
	
		for (size_t i = 0; i < result.size(); i++) {	
			index = ExistNewWayInG(result,result[i].first.first,result[i].first.second);
			
			if (index > 0) {
				ind = GetNumb(result,result[i].first.first,result[i].first.second);
				
				if (bigger_.IsStateAccept(result[index].first.second) || bigger_.IsStateAccept(result[i].first.second) || ind == 1) {
					result.push_back(std::make_pair(std::make_pair(result[i].first.first,result[index].first.second),1));
				}
				else {
					result.push_back(std::make_pair(std::make_pair(result[i].first.first,result[index].first.second),0));	
				}
			}
		}
	
		pom.insert(pom.end(),result.begin(),result.end());
				
		
		//Testing states and create rest of them with symbol -1
		for (size_t i = 0; i < pom.size(); i++) {
			for (size_t j = 0; j < pom.size(); j++) {

				if (!ExistStates(result,pom[i].first.first,pom[j].first.first)) {
					result.push_back(std::make_pair(std::make_pair(pom[i].first.first,pom[j].first.first),-1));
				}
				if (!ExistStates(result,pom[i].first.second,pom[j].first.second)) {
					result.push_back(std::make_pair(std::make_pair(pom[i].first.second,pom[j].first.second),-1));
				}
				if (!ExistStates(result,pom[i].first.second,pom[j].first.first)) {
					result.push_back(std::make_pair(std::make_pair(pom[i].first.second,pom[j].first.first),-1));
				}
				if (!ExistStates(result,pom[i].first.first,pom[j].first.second)) {
					result.push_back(std::make_pair(std::make_pair(pom[i].first.first,pom[j].first.second),-1));
				}					
				if (!ExistStates(result,pom[j].first.first,pom[i].first.first)) {
					result.push_back(std::make_pair(std::make_pair(pom[j].first.first,pom[i].first.first),-1));
				}
				if (!ExistStates(result,pom[j].first.second,pom[i].first.second)) {
					result.push_back(std::make_pair(std::make_pair(pom[j].first.second,pom[i].first.second),-1));
				}
				if (!ExistStates(result,pom[j].first.first,pom[i].first.second)) {
					result.push_back(std::make_pair(std::make_pair(pom[j].first.first,pom[i].first.second),-1));
				}
				if (!ExistStates(result,pom[j].first.second,pom[i].first.first)) {
					result.push_back(std::make_pair(std::make_pair(pom[j].first.second,pom[i].first.first),-1));
				}
				
			}
		}			

	return result;		
		
	}

	bool isTheSame(Graph g, Graph h) {
	
		if (g.size() == h.size()) {	
			for (size_t i=0; i<g.size(); i++) {
				if (g[i].first.first != h[i].first.first && g[i].first.second != h[i].first.second) {
					return false;
				}	
			}	
		}	
		else 
			return false;
		return true;
		
	}	
	/*
	 * Relaxed double graph test
	 * contains two tests
	 */
	bool RDGT(
		const SuperGraph g,	
		const SuperGraph h) {
		
		if (!isWeaklyProper(g.first,h.first) || !LFT(g.second,h.second)) {
			return false;
		}
	
		return true;
	}
	
	/*
	 * Test sumbsumtion relation of two supergraphs
	 */
	bool TestSumbsumtion(
		const SuperGraph g,	
		const SuperGraph h) {			
		
		//first test edges from SuperGraph
		bool val = false;
		unsigned int z = 0;
		unsigned int y = 0;
		size_t i = 0;
			
		
		if (g.first.first == h.first.first) {
			if 	(!smaller_.IsStateAccept(g.first.second) && 
				  smaller_.IsStateAccept(h.first.second)) {
				
				return val;
			}
		}
		else {
			return val;
	
		}	

		//second test sumbsumtion for Graphs from SuperGraph

		for (i = 0; i < g.second.size(); i++) {		
			
			for (size_t j = 0; j < h.second.size(); j++) {
				if (g.second[i].first.first == h.second[j].first.first) {
					if (g.second[i].second >= 0 && g.second[i].second <= h.second[j].second) {
						y++;	
						if 	(bigger_.IsStateAccept(h.second[j].first.second) ||
							(!bigger_.IsStateAccept(h.second[j].first.second) &&
							 !bigger_.IsStateAccept(g.second[i].first.second))) {
							val = true;
							z++;
						}
					}	
				}
			}
			
			if (!val) //Doesnt exist or sumbsumtion is not right
				return val;
		}
		
		if ( z == y) {	
			return true;	
		}	
		else
			return false;
		
		
	}	
		
	/*
	 * Test composition of two supergraphs
	 */	
	bool TestComposition(
		const SuperGraph g,	
		const SuperGraph h) {
		
		
		if (g.first.second != h.first.first) {
			return false;
		}	
		
		for (size_t i = 0; i < g.second.size(); i++) {
			for (size_t j = 0; j < h.second.size(); j++) {	
				if (g.second[i].first.second == h.second[j].first.first) {
					return true;
				}	
			}
		}	
	
		return false;
	}	
		
	/*
	 * Composition of two supergraphs
	 */	
	SuperGraph Composition(		
		const SuperGraph g,	
		const SuperGraph h) {
		
		SuperGraph result;

		result.first.first = g.first.first;
		result.first.second = h.first.second;
	
		for (size_t i = 0; i < g.second.size(); i++) {
			for (size_t j = 0; j < h.second.size(); j++) {
				if (!ExistStates(result.second,g.second[i].first.first,g.second[i].first.second)) {
					result.second.push_back(g.second[i]);
				}	
				if (!ExistStates(result.second,h.second[j].first.first,h.second[j].first.second)) {
					result.second.push_back(h.second[j]);
				}
			}	
		}	
		
		return result;			
	}

	/*
	 * Test set Zgh is weakly proper
	 */
	bool isWeaklyProper(Edge g, 
			    Edge h) {
		
		if (smaller_.IsStateInitial(g.first) && smaller_.IsStateAccept(h.second) ) {
			if (	(smaller_.IsStateAccept(g.second) ||
				(!smaller_.IsStateAccept(g.second) &&
				!smaller_.IsStateAccept(h.first))) &&
				(smaller_.IsStateAccept(h.second) ||
				(!smaller_.IsStateAccept(h.second) &&
				!smaller_.IsStateAccept(h.first)))) {
				return false;
			}
		}
	
		return true;			
	}

	/*
	 * Lasso finding test main function
	 */
	bool LFT(Graph g, 
		 Graph h) {
		
		for (size_t i = 0; i < g.size(); i++) {
			for (size_t j = 0; j < h.size(); j++) {
				if (g[i].first.second == h[j].first.first) {
					if (LassoTest(h,h[j].first.first)) {
						return true;	
					}
				}	
			}	
		}
		return false;		
	}
	
	/*
	 * finding lasso of state in graph h
	 */
	bool LassoTest(Graph h, 
		       StateType &state) {
				
		//Stack for lasso states
		std::vector<Edge> Stack;		
	
		//Edge for testing
		Edge E;
	
		bool nonelse = false;
		
		//Stack for non exist lasso states
		std::vector<Edge> NonExist;	
	
		//Finding first arcs for state
		for (size_t i = 0; i < h.size(); i++) {
			if (h[i].first.first == state && h[i].second >= 0) {
				Stack.push_back(h[i].first);
			}	
		}
		
	
		while (!Stack.empty()) {
			E = Stack.back();
		
			for (size_t i = 0; i < Stack.size(); i++) {
				if ( E.second == Stack[i].first ) {
					return true;
				}
			}	
		
			for (size_t i = 0; i < h.size(); i++) {
				if (E.second == h[i].first.first) {
					if (!isThere(E,NonExist)) {
						Stack.push_back(h[i].first);
						nonelse = true;
						break;
					}	
				}
				nonelse = false;
			}
		
			if (!nonelse) {
				Stack.pop_back();
				NonExist.push_back(E);
			}	
		}
	
		return false;
	}

	/**
	 * Main algorithm of Ramsey-based approach
	 * for checking inclusion of two BA
	 */
	bool Ramsey_Incl_Check() {
	
		bool pom;
		
		//Values for algorithm inclusion
		std::vector<SuperGraph> Next,Init,Processed;
		SuperGraph f,g;		

		//First is create set of supergraphs from input automata
		Next = CreateSuper();
		
		//Minimalization supergraphs
		for (size_t i = 0; i < Next.size(); i++) {
			Next[i] = MinS(Next[i]);
		}		

		
		if (Next.size() == 0)
			return false;
		

		//Prepairing and remove supergraphs which are sumbsumed by others
		while (Next.size() != 0) {

			g = Next.back();
			Next.pop_back();
		
			if (Init.size() != 0) {
				
				f = Init.back();	
				if (TestSumbsumtion(f,g)) {
					continue;	
				}	
				else if (TestSumbsumtion(g,f)){
					Init.pop_back();		
				}
			}		
		
			Init.push_back(g);	
		}
		
		Next.clear();
		
		Next.insert(Next.end(),Init.begin(),Init.end());
		
		//Main algorithm
		while (Next.size() != 0) {
	
			
			g = Next.back();
			
			//Test and finding lasso
			if (!RDGT(g,g)) {
				return false;
			}
			//Finding lasso between all supergraphs in Processed and g
			if (Processed.size() != 0) {
				for(size_t i = 0; i < Processed.size(); i++) {
					if (!RDGT(Processed[i],g) || !RDGT(g,Processed[i])) {
						return false;
					}
				}
			}
			
			Next.pop_back();	
			Processed.push_back(g);
	
			//Composition of supergraphs
			for(size_t i = 0; i < Init.size(); i++) {

				if (TestComposition(g,Init[i])) {
					
					f = MinS(Composition(g,Init[i]));
					for(size_t j = 0; j < Processed.size(); j++) {
												
						if (!pom) {
							if (TestSumbsumtion(Processed[j],f)) {
								pom = true;
								continue;	
							}	
							else {
								pom = false;	
								if (isTheSame(Processed[j].second, f.second)) {
									pom = true;
								}
							}
						}
					}	
					for(size_t j = 0; j < Next.size(); j++) {
						if (!pom) {
							if (TestSumbsumtion(Next[j],f)) {
								pom = true;
								continue;	
							}	
							else {
								pom = false;		
							}
						}	
					}
					
					if (pom)
						continue;
					
					Processed.clear();
					Next.clear();
					Next.push_back(f);
				}
			}
		}
	
		return true;
	}	
	
	
};


#endif