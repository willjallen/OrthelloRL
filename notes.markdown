https://en.wikipedia.org/wiki/Monte_Carlo_tree_search

Leaf parallelization, i.e. parallel execution of many playouts from one leaf of the game tree.
Root parallelization, i.e. building independent game trees in parallel and making the move basing on the root-level branches of all these trees.
Tree parallelization, i.e. parallel building of the same game tree, protecting data from simultaneous writes either with one, global mutex, with more mutexes, or with non-blocking synchronization.

https://arxiv.org/abs/2003.13741
https://deepai.org/publication/parallelization-of-monte-carlo-tree-search-in-continuous-domains
Fig 8 - Leaf max paralleization has best performance

https://www.deepmind.com/blog/alphago-zero-starting-from-scratch
The neural network should observe the state space, then pick the (legal) location of play


Neural network parameters (theta)
-> Run MCTS using (theta) as selection (maximum action value)
-> Generate from the MCTS a policy (PI_t) at each step t
-> Upon a terminal state, update all (PI_t) with who won (z in {-1, 0, 1})
-> Optimize network to minimize difference in (z) and maximize similarity between (theta) and (pi)


**Neural network architecture(in alphaZero paper)**

Input:
(Not including temporal dimension or an 8x8 constant "who's playing" feature)
- 8x8 binary encoded player tiles
  - 0 if empty or other player
  - 1 if player


- 8x8 binary encoded enemy tiles
  - 0 if empty or other player
  - 1 if player


- 8x8 binary encoded legal moves
  - 0 if illegal
  - 1 if legal


**Bold** dd

Output:
- 8x8 vector action probability distribution
- scalar value prediction (-1, 1) [How likely this position is a winning position]




