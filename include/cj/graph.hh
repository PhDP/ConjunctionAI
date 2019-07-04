#ifndef CJ_GRAPH_HH_
#define CJ_GRAPH_HH_

#include "cj/common.hh"

namespace cj {

/**
 * \brief ...
 */
template<typename Graph>
using path = pair<
  vector<typename graph_traits<Graph>::vertex_type>,
  typename graph_traits<Graph>::weight_type>;

// Basic graph structures and graph_traits implementation:

/**
 * \brief ...
 */
template<typename W>
struct graph_traits<matrix<W>> {
  static bool const is_weighted = true;
  static bool const dynamic_vertices = false;
  using vertex_type = size_t;
  using weight_type = W;
  using set_type = flat_set<vertex_type>;
};

/**
 * \brief ...
 */
template<typename W>
struct graph_traits<sparse_matrix<W>> {
  static bool const is_weighted = true;
  static bool const dynamic_vertices = false;
  using vertex_type = size_t;
  using weight_type = W;
  using set_type = flat_set<vertex_type>;
};

/**
 * \brief An unweighted graph represented as a map from vertices to sets of vertices.
 *
 * \tparam V        Type of the vertices.
 * \tparam Weight   Type of the weights. Even if the graph is unweighted, this type will be used to
 *                  compute path lengths, with all edges having a 'weight' of Weight(1).
 * \tparam Map      Type used to map vertices to sets of vertices.
 * \tparam Set      Type used for the set of vertices.
 */
template<
  typename V,
  typename Weight = size_t,
  template<typename, typename...> typename Map = flat_map,
  template<typename...> typename Set = flat_set>
using adjacency_set = Map<T, Set<T>>;

// Implement graph_traits for adjacency_map.
template<typename V, typename W, template<typename, typename...> typename M, template<typename...> typename S>
struct graph_traits<adjacency_set<V, W, M, S>> {
  static bool const is_weighted = false;
  static bool const dynamic_vertices = true;
  using vertex_type = V;
  using weight_type = W;
  using set_type = S<V>;
};

/**
 * \brief An unweighted graph represented as a map from vertices to sets of vertices.
 *
 * \tparam V        Type of the vertices.
 * \tparam Weight   Type of the weights. Even if the graph is unweighted, this type will be used to
 *                  compute path lengths, with all edges having a 'weight' of Weight(1).
 * \tparam Map      Type used to map vertices to sets of vertices.
 * \tparam Set      Type used for the set of vertices.
 */
template<
  typename V,
  typename Weight,
  template<typename, typename...> typename InnerMap = flat_map,
  template<typename, typename...> typename OuterMap = flat_map>
using adjacency_map = OuterMap<T, InnerMap<T>>;

// Implement graph_traits for adjacency_map.
template<typename V, typename W, template<typename, typename...> typename O, template<typename, typename...> typename I>
struct graph_traits<adjacency_map<V, W, O, I>> {
  static bool const is_weighted = true;
  static bool const dynamic_vertices = true;
  using vertex_type = V;
  using weight_type = W;
  using set_type = typename map_traits<I>::set_type;
};

/**
 */
template<
  typename V,
  size_t order,
  typename Weight,
  template<typename...> typename Set,
  typename = std::enable_if_t<
    std::is_integral<typename std::iterator_traits<V>::value_type>::value>>
using array_set = std::array<Set<T>, order>;







template<typename Graph>
auto order(Graph const& g) -> size_t {
  return g.size();
}

template<typename Graph>
auto size(Graph const& g) -> size_t {
  auto count = size_t(0);
  for (auto const v : g) {
    count += v.size();
  }
  return count;
}

template<typename V>
auto size(matrix<V> const& m) -> size_t {
  auto count = size_t(0);
  auto const end = m.data() + m.size();
  for (auto i = m.data(); i != end; ++i) {
    count += *i != V(0);
  }
  return count;
}

/**
 * \brief
 */
template<typename Graph>
auto has_node(Graph const& g, typename graph_traits<Graph>::vertex_type const& v) -> bool {
  return g.find(v) != n.end();
}

// ..


// ...

template<typename Graph>
auto has_edge(
    Graph const& g,
    typename graph_traits<Graph>::vertex_type const& source,
    typename graph_traits<Graph>::vertex_type const& target)
    -> bool {
  auto const it = g.find(source);
  return it != g.end() && it->second.find(target) != it->second.end();
}

template<
  typename Graph,
  typename = std::enable_if_t<graph_traits<Graph>::is_weighted>>
auto weight(
    Graph const& g,
    typename graph_traits<Graph>::vertex_type const& source,
    typename graph_traits<Graph>::vertex_type const& target)
    -> typename graph_traits<Graph>::weight_type {
  using weight = graph_traits<Graph>::weight_type;
  auto const i = g.find(source);
  if (i == g.end()) {
    return weight(0);
  }
  auto const j = i->second.find(target);
  return j != i->second.end()? j->second : weight(0);
}

template<
  typename Graph,
  typename = std::enable_if_t<!graph_traits<Graph>::is_weighted>>
auto weight(
    Graph const& g,
    typename graph_traits<Graph>::vertex_type const& source,
    typename graph_traits<Graph>::vertex_type const& target)
    -> typename graph_traits<Graph>::weight_type {
  using weight = graph_traits<Graph>::weight_type;
  auto const i = g.find(source);
  if (i == g.end()) {
    return weight(0);
  }
  auto const j = i->second.find(target);
  return j != i->second.end()? weight(1) : weight(0);
}

} /* end namespace cj */

#endif
