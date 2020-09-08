/*
 * @file   TSPSolver.h
 * @author (original JAVA) William Fiset, william.alexandre.fiset@gmail.com
 *         (conversion to C++) Armin Zare Zadeh, ali.a.zarezadeh@gmail.com
 *         (modified) Frank Hart frankhart010@gmail.com
 * @date   6 September 2020
 * @version 0.1
 * @brief   An implementation of the traveling salesman problem using dynamic programming to improve
 * the time complexity from O(n!) to O(n^2 * 2^n).
 *
 * Time Complexity: O(n^2 * 2^n) Space Complexity: O(n * 2^n)
 *
 * # Finds the minimum TSP tour cost.
 * # m - 2D adjacency matrix representing graph
 * # S - The start node (0 <= S < N)
 * function tsp(m, S):
 *   N = matrix.size
 *
 *   # Initialize memo table.
 *   # Fill table with null values or +inf
 *   memo = 2D table of size N by 2^N
 *
 *   setup(m, memo, S, N)
 *   solve(m, memo, S, N)
 *   minCost = findMinCost(m, memo, S, N)
 *   tour = findOptimalTour(m, memo, S, N)
 *
 *   return (minCost, tour)
 *
 *
 * # Initialize the memo table by caching
 * # the optimal solution from the start
 * # node to every other node.
 * function setup(m, memo, S, N):
 *   for (i=0; i<N; i=i+1):
 *     if i == S: continue
 *
 *     # Store the optimal value from node S
 *     # to each node i (this is given as input)
 *     # in the adjacency matrix m).
 *     memo[i][1 << S | 1 << i] = m[S][i]
 *
 *
 * function solve(m, memo, S, N):
 *   for (r=3; r<=N; r++):
 *     # The combinations function generates all bit sets
 *     # of size N with r bits set to 1. For example,
 *     # combinations(3,4)={0111, 1011, 1101, 1110}
 *     for subset in combinations(r, N):
 *       if notIn(S, subset): continue
 *         for (next = 0; next < N; next = next + 1):
 *           if next == S || notIn(next, subset): continue
 *           # The subset state without the next node
 *           state = subset ^ (1 << next)
 *           minDist = +inf
 *           # 'e' is short for end node.
 *           for (e = 0; e < N; e = e + 1):
 *             if e == S || e == next || notIn(e, subset): continue
 *             newDistance = memo[e][state] + m[e][next]
 *             if (newDistance < minDist): minDist = newDistance
 *           memo[next][subset] = minDist
 *
 *
 * # Returns true if the i'th bit in 'subset' is not set
 * function notIn(i, subset):
 *   return ((1 << i) & subset) == 0
 *
 *
 * # Generate all bit sets of size n with r bits set to 1.
 * function combinations(r, n):
 *   subsets = []
 *   combinations(0, 0, r, n, subsets)
 *   return subsets
 *
 *
 * # Recursive method to generate bit sets.
 * function combinations(set, at, r, n, subsets):
 *   if r == 0:
 *     subsets.add(set)
 *   else:
 *     for (i=at; i<n; i=i+1):
 *       # Flip on i'th bit
 *       set = set | (1 << i)
 *
 *       combinations(set, i+1, r-1, n, subsets)
 *
 *       # Backtrack and flip off i'th bit
 *       set = set & ~(1 << i)
 *
 *
 * function findMinCost(m, memo, S, N):
 *   # The end state is the bit mask with
 *   # N bits set to 1 (equivalently 2^N - 1)
 *   END_STATE = (1 << N) - 1
 *
 *   minTourCost = +inf
 *
 *   for (e=0; e<N; e=e+1):
 *     if e == S: continue
 *
 *     tourCost = memo[e][END_STATE] + m[e][S]
 *     if tourCost < minTourCost:
 *       minTourCost = tourCost
 *
 *   return minTourCost
 *
 *
 * function findOptimalTour(m, memo, S, N):
 *   lastIndex = S
 *   state = (1 << N) - 1; # End state
 *   tour = array of size N+1
 *
 *   for (i = N-1; i >= 1; i--):
 *     index = -1
 *     for (j = 0; j < N; j++):
 *       if j == S | notIn(j, state): continue
 *       if (index == -1) index = j
 *       prevDist = memo[index][state] + m[index][lastIndex]
 *       newDist = memo[j][state] + m[j][lastIndex];
 *       if (newDist < prevDist) index = j
 *
 *     tour[i] = index
 *     state = state ^ (1 << index)
 *     lastIndex = index
 *
 *   tour[0] = tour[N] = S
 *   return tour
 */

#ifndef D_GRAPH_TSPDYNAMICPROGITER_H
#define D_GRAPH_TSPDYNAMICPROGITER_H

#include <list>
#include <algorithm>

#include <limits>
#include <stdexcept>

template<typename T>
class Matrix
{
public:

  Matrix()
    : m_width(0)
    , m_height(0)
    , m_pData(nullptr)
  {

  }

  Matrix(size_t width, size_t height)
    : m_width(width)
    , m_height(height)
    , m_pData(new T[width * height])
  {

  }

  Matrix(size_t width, size_t height, T initVal)
    : m_width(width)
    , m_height(height)
    , m_pData(new T[width * height])
  {
    for (size_t i = 0; i < m_width * m_height; i++)
      m_pData[i] = initVal;
  }

  ~Matrix()
  {
    delete[] m_pData;
  }

  Matrix(Matrix const & other)
    : m_width(other.m_width)
    , m_height(other.m_height)
    , m_pData(new T[m_width * m_height])
  {
    for (size_t i = 0; i < m_width * m_height; i++)
      m_pData[i] = other.m_pData[i];
  }

  Matrix & operator=(Matrix const & other)
  {
    if (this != &other)
    {
      m_width = other.m_width;
      m_height = other.m_height;
      delete[] m_pData;
      m_pData = new T[m_width * m_height];
      for (size_t i = 0; i < m_width * m_height; i++)
        m_pData[i] = other.m_pData[i];
    }

    return *this;
  }

  T operator()(size_t x, size_t y) const
  {
    return m_pData[x + y * m_width];
  }

  T & operator()(size_t x, size_t y)
  {
    return m_pData[x + y * m_width];
  }

  size_t Width() const
  {
    return m_width;
  }

  size_t Height() const
  {
    return m_height;
  }

private:

  size_t m_width;
  size_t m_height;
  T * m_pData;
};

template<typename T>
class TSPSolver
{
private:

  bool notIn(int elem, int subset)
  {
    return ((1 << elem) & subset) == 0;
  }

  // This method generates all bit sets of size n where r bits
  // are set to one. The result is returned as a list of integer masks.
  std::list<int> combinations(int r, int n)
  {
    std::list<int> subsets;
    combinations(0, 0, r, n, subsets);
    return subsets;
  }

  // To find all the combinations of size r we need to recurse until we have
  // selected r elements (aka r = 0), otherwise if r != 0 then we still need to select
  // an element which is found after the position of our last selected element
  void combinations(int set, int at, int r, int n, std::list<int> & subsets)
  {

    // Return early if there are more elements left to select than what is available.
    int elementsLeftToPick = n - at;
    if (elementsLeftToPick < r) return;

    // We selected 'r' elements so we found a valid subset!
    if (r == 0)
    {
      subsets.push_back(set);
    }
    else
    {
      for (int i = at; i < n; i++)
      {
        // Try including this element
        set ^= (1 << i);

        combinations(set, i + 1, r - 1, n, subsets);

        // Backtrack and try the instance where we did not include this element
        set ^= (1 << i);
      }
    }
  }

public:

  TSPSolver(Matrix<T> & distance) 
    : TSPSolver(0, distance)
  {

  }

  TSPSolver(int start, Matrix<T> const & distance)
  {
    if (distance.Height() != distance.Width())
      throw std::invalid_argument("Must be a square matrix.");

    if (distance.Height() <= 2)
      throw std::invalid_argument("N <= 2 not yet supported.");

    if (start < 0 || start >= distance.Height())
      throw std::invalid_argument("Invalid start node.");

    if (distance.Height() > 32)
      throw std::invalid_argument("Matrix too large! A matrix that size for the DP TSP problem with a time complexity of O(n^2*2^n) requires way too much computation for any modern home computer to handle");

    m_minTourCost = std::numeric_limits<T>::infinity();
    m_nodes = static_cast<int>(distance.Width());
    m_start = start;
    m_distance = distance;
  }

  // Solves the traveling salesman problem and caches solution.
  // TODO What is solve is called more than once?
  void solve()
  {
    int END_STATE = (1 << m_nodes) - 1;
    Matrix<T> memo(m_nodes, 1ull << m_nodes, std::numeric_limits<T>::infinity());

    // Add all outgoing edges from the starting node to memo table.
    for (int end = 0; end < m_nodes; end++)
    {
      if (end == m_start) continue;
      memo(end, (1 << m_start) | (1 << end)) = m_distance(m_start, end);
    }

    for (int r = 3; r <= m_nodes; r++)
    {
      for (int subset : combinations(r, m_nodes))
      {
        if (notIn(m_start, subset)) continue;
        for (int next = 0; next < m_nodes; next++)
        {
          if (next == m_start || notIn(next, subset)) continue;
          int subsetWithoutNext = subset ^ (1 << next);
          T minDist = std::numeric_limits<T>::infinity();
          for (int end = 0; end < m_nodes; end++)
          {
            if (end == m_start || end == next || notIn(end, subset)) continue;
            T newDistance = memo(end, subsetWithoutNext) + m_distance(end, next);
            if (newDistance < minDist)
            {
              minDist = newDistance;
            }
          }
          memo(next, subset) = minDist;
        }
      }
    }

    // Connect tour back to starting node and minimize cost.
    for (int i = 0; i < m_nodes; i++)
    {
      if (i == m_start) continue;
      T tourCost = memo(i, END_STATE) + m_distance(i, m_start);
      if (tourCost < m_minTourCost)
      {
        m_minTourCost = tourCost;
      }
    }

    int lastIndex = m_start;
    int state = END_STATE;
    m_tour.push_front(m_start);

    // Reconstruct TSP path from memo table.
    for (int i = 1; i < m_nodes; i++)
    {

      int index = -1;
      for (int j = 0; j < m_nodes; j++)
      {
        if (j == m_start || notIn(j, state)) continue;
        if (index == -1) index = j;
        T prevDist = memo(index, state) + m_distance(index, lastIndex);
        T newDist = memo(j, state) + m_distance(j, lastIndex);
        if (newDist < prevDist)
        {
          index = j;
        }
      }

      m_tour.push_front(index);
      state = state ^ (1 << index);
      lastIndex = index;
    }

    m_tour.push_front(m_start);
  }

  // Returns the optimal tour for the traveling salesman problem.
  const std::list<int> & getTour()
  {
    return m_tour;
  }

  // Returns the minimal tour cost.
  T getTourCost()
  {
    return m_minTourCost;
  }

private:

  int m_nodes, m_start;
  Matrix<T> m_distance;
  std::list<int> m_tour;
  T m_minTourCost;
};
#endif /* D_GRAPH_TSPDYNAMICPROGITER_H */