# Optimisation <!-- omit in toc -->

Despite being initially written with performance in mind due to the $O(2^n)$ complexity of the algorithm, I ran into performance bottlenecks very quickly, warranting further optimisation. This file maintains a changelog documenting the various optimisations tested and applied to improve the performance of the solver. 

- [Fixing the hash function](#fixing-the-hash-function)
- [Running out of RAM :sweat\_smile:](#running-out-of-ram-sweat_smile)


## Fixing the hash function
Initially, the solver took ~76s on [`Queen_A1`](src/puzzles.hpp#120), which took 26k iterations to solve. This slow performance was due to a bug in the `std::hash<Board>` implementation which caused all boards to be hashed to 0 - leading to every node colliding with the others, and degrading the access times of the solver's `std::unordered_set`s and `std::unordered_map`s from $O(1)$ to $O(n)$ (oopsie). Once corrected this resulted in `Queen_A1` only taking 176ms, a roughly 430x speedup in performance. The process which led to the discovery of this bug is outlined below:

Using `perf`, the following hot functions were identified:
```
# Overhead  Command  Shared Object        Symbol
# ........  .......  ...................  ...............................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................
#
    81.92%  solver   solver               [.] Solver<4ul, 5ul>::InsertNode
     8.33%  solver   solver               [.] Solver<4ul, 5ul>::GenerateSolution
     7.16%  solver   solver               [.] std::__detail::_Map_base<Board<4ul, 5ul>, std::pair<Board<4ul, 5ul> const, std::_Rb_tree_const_iterator<Solution<4ul, 5ul> > >, std::allocator<std::pair<Board<4ul, 5ul> const, std::_Rb_tree_const_iterator<Solution<4ul, 5ul> > > >, std::__detail::_Select1st, std::equal_to<Board<4ul, 5ul> >, std::hash<Board<4ul, 5ul> >, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<true, false, true>, true>::at
     2.38%  solver   solver               [.] std::_Hashtable<Board<4ul, 5ul>, std::pair<Board<4ul, 5ul> const, std::_Rb_tree_const_iterator<Solution<4ul, 5ul> > >, std::allocator<std::pair<Board<4ul, 5ul> const, std::_Rb_tree_const_iterator<Solution<4ul, 5ul> > > >, std::__detail::_Select1st, std::equal_to<Board<4ul, 5ul> >, std::hash<Board<4ul, 5ul> >, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<true, false, true> >::_M_erase
```
Clearly `Solver::InsertNode` dominates the processing time of the solver, although I struggled drilling down into the function to determine exactly what part of it took up so much time (I tried `perf annotate` but it wasn't particularly helpful). Despite this, `perf` was still able to provide a starting point to try and optimise. In hindsight, the other hot functions hint at the cause of the true problem. `Solver::GenerateSolution` is the main body of the function, so it makes sense that this takes up execution time, but the following two function both appear to be `std::unorderd_map` methods, likely related to the `std::unordered_map<Board, std::set<Solution>::iterator mNodeMap` based on the template types it contains. The first method appears to refer to `at` (element access), and the second `_M_erase` (element removal - requires access).

Without access to a more detailed breakdown of what instructions were taking the longest, the first thing I considered that could cause `Solver::InsertNode`'s terrible performance was the set comparison function used, `Solver::NodeOrder`. Without access to more detailed metrics, it's impossible to know if this actually was an issue, but since this comparison operator is called $O(\log{n})$ times per insertion, and there are usually multiple insertions per iteration, it seemed like the best place to start, who's original implementation is shown below:
```c++
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
```
Since in (what I assumed to be) the majority of cases, the total costs and number of moves between nodes are identical, many times this function needs to iterate through the boards of both nodes comparing them, with a most likely difficult to predict jump of `lState == rState`. I wanted to try and rework this part of the set comparison to try and remove the conditional `continue` in order to reduce the number of control hazards that could be affecting the number of retiring instructions in the CPU. Once again, without profiling this there's no way of knowing if this is an actual issue, but since the size of the boards is relatively small in this problem, it seemed like the most likely issue. 

I thought a great way of solving this would be to use the `std::hash<Board>` implementation I defined, as given the size of the problem, the board should be small enough for its hash to be unique for any possible boardstate, resulting in no collisions and ensuring that the requirement of `NodeOrder(l, r) == NodeOrder(r, l) iff l == r` was met. It's worth noting that this implementation contains 2 conditional jumps in the body of the loop, one checking if the state of a particular tile is `BoardState::BLOCKED`, and the other handling hash overflows. I was able to optimise the latter one away using a `if constexpr()`, since the possibility of the unique `BoardState` encoding exceeding the number of bits available in `size_t` can be computed at compile-time. I considered the former to be less of an issue as tiles with the `BoardState::BLOCKED` state are identical across all possible nodes, meaning that a branch predictor could learn this pattern (depending on the architecture of the branch predictor of course). Regardless of whether or not this reasoning was valid, after changing the implementation of `Solver::NodeOrder`, the runtime checks in `Solver::InsertNode` verifying that node insertion actually took place quickly failed (defensive programming :pray:), and subsequent investigation revealed that I missed a XOR operation in the hash function. Solving this issue dramatically improved the scalability of the solver, and led to the huge performance increase mentionned above :party:! 

Moral of the story: probably good to write unit tests for custom hash functions to check they work as expected (and more unit tests in general).

## Running out of RAM :sweat_smile:
After the big boost in performance from fixing the hashing issue, I wanted to see if the solver could handle the [`King_A1`](src/puzzles.hpp#135) level that it would time out on earlier. Since the code ran much faster, it was easier to test significantly more iterations than before. Unfortunately, at around 3.5M iterations, the program is killed because WSL runs out of the 4GB limit I've allocated to it, leading to the next problem to solve: minimising the memory consumption of the solver. 

Since each node contains a board, and a board is a 2-D array of `BoardState`s, when first writing the code I tried to tell the compiler to make these states 1-byte long to reduce the overall memory footprint, which I did using the `__attribute__((packed))` directive as in the enum class declaration. Unfortunately, this directive seems to only work for raw `enum`s, not `enum class`es (see [godbolt](https://godbolt.org/z/vb6K5dcfG)). With an enum class, the underlying type used can instead be specified when declaring the class:
```c++
// Raw enum
typedef enum __attribute__((packed)) {
    EMPTY,
    BLUE,
    RED,
    YELLOW,
    BLOCKED
} PackedBoardState;

// enum class
enum class BoardState : uint8_t {
    EMPTY,
    BLUE,
    RED,
    YELLOW,
    BLOCKED
}; 

static_assert(sizeof(PackedBoardState) == sizeof(BoardState)) // true (both have sizeof == 1)
```
Fixing this did improve the memory footprint, and the solver was now able to make it to 5.7M iterations before hitting the 4GB limit.