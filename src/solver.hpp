#pragma once

#include <unordered_map>
#include <vector>
#include <set>
#include <cstdint>
#include <unordered_set>
#include <optional>
#include <cassert>

#include "common.hpp"
#include "board.hpp"
#include "solution.hpp"
#include "helper.hpp"
#include "puzzles.hpp"

template <size_t Width, size_t Height>
class Solver 
{
public:
    Solver(Puzzle<Width, Height>&& puzzle)
        : mTargets(std::move(puzzle.targets)),
          mAvailableNodes(NodeOrder)
    {
        Solution<Width, Height> initialSoln(std::move(puzzle.initialState), mTargets);
        InsertNode(std::move(initialSoln));
    }

    Solution<Width, Height> GenerateSolution(uint32_t maxIterations = 100000);
    
private:
    static bool NodeOrder(const Solution<Width, Height>& l, const Solution<Width, Height>& r);

    void InsertNode(Solution<Width, Height>&& solution); 
    Solution<Width, Height> GetNextNode();
    bool UpdateBestSolution(Solution<Width, Height>&& candidate);

    std::unordered_map<Target, std::vector<BoardPos>> mTargets;
    std::set<Solution<Width, Height>, decltype(&NodeOrder)> mAvailableNodes;
    std::unordered_map<Board<Width, Height>, typename std::set<Solution<Width, Height>>::iterator> mNodeMap;
    std::unordered_set<Board<Width, Height>> mExpandedNodes;
    std::optional<Solution<Width, Height>> mBestSolution;
    size_t mFilteredSolutions = 0;
};

template <size_t W, size_t H>
Solution<W, H> Solver<W, H>::GenerateSolution(uint32_t maxIterations)
{
    std::cout << "[Info] Attempting to solve:\n" << mAvailableNodes.begin()->board << std::endl;
    for (uint32_t i = 0; i < maxIterations; i++)
    {
        if (i % 100 == 0)
            std::cout << "[Info] Iteration " << i << ": # of pending nodes = " << mAvailableNodes.size() << ", # of filtered nodes = " << mFilteredSolutions << ", found solution = " << mBestSolution.has_value() << std::endl;
        if (mAvailableNodes.empty())
        {
            if (mBestSolution)
            {
                std::cout << "[Info] Exhausted all possible nodes, found optimal solution, terminating @ iteration " << i << std::endl;
                return *mBestSolution;
            }

            std::cerr << "[Error] Out of nodes to expand (explored " << i << " states, filtered " << mFilteredSolutions << "), problem has no solution" << std::endl;
            exit(1);
        }

        Solution currentNode(GetNextNode());
        if (mBestSolution && mBestSolution->NOfMoves() <= currentNode.GetTotalCost())
        {
            std::cout << "[Info] Current node heuristic cost (" << currentNode.GetTotalCost() << ") exceeds bound of current solution (" << mBestSolution->NOfMoves() << "), terminating @ iteration " << i << std::endl;
            return *mBestSolution;
        }
        
        for (Move& move : currentNode.board.GetPossibleMoves())
        {
            Solution candidate(currentNode);
            candidate.ApplyMove(std::move(move), mTargets);
            if (!UpdateBestSolution(std::move(candidate)))
            {
                InsertNode(std::move(candidate));
            }
        }
    }
    std::cerr << "[Error] Unable to find solution in " << maxIterations << " iterations, giving up." << std::endl;
    exit(1);
}

template <size_t W, size_t H>
bool Solver<W, H>::NodeOrder(const Solution<W, H>& l, const Solution<W, H>& r)
{
    // We require NodeOrder(l, r) == NodeOrder(r, l) iff l == r
    if (l.GetTotalCost() == r.GetTotalCost())
    {
        if (l.NOfMoves() == r.NOfMoves())
        {
            for (int8_t y = 0; y < l.board.height(); y++)
            {
                for (int8_t x = 0; x < l.board.width(); x++)
                {
                    BoardPos curr {x, y};
                    BoardState lState = l.board.at(curr);
                    BoardState rState = r.board.at(curr);
                    if (lState == rState)
                        continue;
                    
                    return static_cast<uint32_t>(lState) < static_cast<uint32_t>(rState);
                }
            }
        }
        return l.NOfMoves() > r.NOfMoves();
    }
    return l.GetTotalCost() < r.GetTotalCost();
}

template <size_t W, size_t H>
void Solver<W, H>::InsertNode(Solution<W, H>&& solution)
{
    Board solutionBoard = solution.board;
    if (mExpandedNodes.contains(solutionBoard))
    {
        mFilteredSolutions++;
        return;
    }
    
    if (mNodeMap.contains(solutionBoard))
    {
        bool isBetterSolution = solution.NOfMoves() < mNodeMap.at(solutionBoard)->NOfMoves();
        if (!isBetterSolution)
        {
            mFilteredSolutions++;
            return;
        }

        // Solutions should have same heuristic cost since board states are identical
        assert(solution.GetTotalCost() < mNodeMap.at(solutionBoard)->GetTotalCost());
        mAvailableNodes.erase(mNodeMap.at(solutionBoard));
        mNodeMap.erase(solutionBoard);
    }

    auto [it, setRes] = mAvailableNodes.insert(std::move(solution));
    if (!setRes)
    {
        std::cerr << "[Error] Unable to insert solution into mAvailableNodes (should be impossible)" << std::endl;
        exit(1);
    }
    auto [_, mapRes] = mNodeMap.emplace(std::move(solutionBoard), it);
    if (!mapRes)
    {
        std::cerr << "[Error] Unable to insert mAvailableNodes iterator to solution into mNodeMap (should be impossible)" << std::endl;
        exit(1);
    }
}

template <size_t W, size_t H>
Solution<W, H> Solver<W, H>::GetNextNode()
{
    Solution top = mAvailableNodes.extract(mAvailableNodes.begin()).value();
    mNodeMap.erase(top.board);
    mExpandedNodes.insert(top.board);
    return top;
}

template <size_t W, size_t H>
bool Solver<W, H>::UpdateBestSolution(Solution<W, H>&& candidate)
{
    if (candidate.IsComplete(mTargets))
    {
        bool isBetterSolution = !mBestSolution || candidate.NOfMoves() < mBestSolution->NOfMoves();
        if (isBetterSolution)
        {
            mBestSolution = candidate;
            return true;
        }
    }
    return false;
}