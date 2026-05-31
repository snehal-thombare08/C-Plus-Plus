/**
 * @file
 * @brief Centroid Decomposition of a tree
 *
 * @details
 * Centroid Decomposition is a technique for efficiently solving
 * path queries on trees. A centroid of a tree is a node whose
 * removal results in no remaining subtree having more than n/2 nodes.
 *
 * The decomposition works recursively:
 * 1. Find the centroid of the current tree
 * 2. Remove it and recurse on each subtree
 * This creates a "centroid tree" of depth O(log N)
 *
 * This implementation demonstrates centroid decomposition by
 * counting the number of paths of a given length in a tree.
 *
 * Algorithm reference: https://cp-algorithms.com/graph/centroid_decomposition.html
 *
 * Complexity:
 *   - Preprocessing: \f$O(N \log N)\f$
 *   - Query: \f$O(\log N)\f$
 *   - Space: \f$O(N)\f$
 *
 * Example tree:
 * <pre>
 *         1
 *       / | \
 *      2  3  4
 *     / \
 *    5   6
 * </pre>
 *
 * @author Your Name
 */
 
#include <cassert>
#include <functional>
#include <map>
#include <vector>
 
/**
 * @namespace graph
 * @brief Graph algorithms
 */
namespace graph {
 
/**
 * @brief Class implementing Centroid Decomposition on a tree.
 *
 * Supports efficient path queries using the decomposition structure.
 */
class CentroidDecomposition {
 public:
    int n;  ///< number of nodes (1-indexed)
    std::vector<std::vector<int>> adj;  ///< adjacency list
    std::vector<int> subtree_sz;        ///< subtree size for each node
    std::vector<bool> removed;          ///< marks removed centroids
    std::vector<int> centroid_parent;   ///< parent in centroid tree
 
    /**
     * @brief Constructor: initializes the decomposition for n nodes.
     * @param n number of nodes (1-indexed)
     */
    explicit CentroidDecomposition(int n)
        : n(n),
          adj(n + 1),
          subtree_sz(n + 1, 0),
          removed(n + 1, false),
          centroid_parent(n + 1, -1) {}
 
    /**
     * @brief Adds an undirected edge between u and v.
     * @param u first node
     * @param v second node
     */
    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
 
    /**
     * @brief Computes subtree sizes via DFS.
     * @param v current node
     * @param parent parent of current node
     * @return size of subtree rooted at v
     */
    int get_subtree_size(int v, int parent) {
        subtree_sz[v] = 1;
        for (int u : adj[v]) {
            if (u != parent && !removed[u]) {
                subtree_sz[v] += get_subtree_size(u, v);
            }
        }
        return subtree_sz[v];
    }
 
    /**
     * @brief Finds the centroid of a subtree rooted at v.
     *
     * The centroid is a node where no subtree (after its removal)
     * has more than tree_size/2 nodes.
     *
     * @param v current node
     * @param parent parent of current node
     * @param tree_size total size of the current tree component
     * @return centroid node index
     */
    int get_centroid(int v, int parent, int tree_size) {
        for (int u : adj[v]) {
            if (u != parent && !removed[u]) {
                if (subtree_sz[u] > tree_size / 2) {
                    return get_centroid(u, v, tree_size);
                }
            }
        }
        return v;
    }
 
    /**
     * @brief Recursively decomposes the tree and records centroid parents.
     *
     * For each component, finds the centroid, marks it as removed,
     * and recurses on remaining subtrees.
     *
     * @param v any node in the current component
     * @param parent_centroid centroid of the parent component (-1 for root)
     */
    void decompose(int v, int parent_centroid) {
        int sz = get_subtree_size(v, -1);
        int centroid = get_centroid(v, -1, sz);
 
        centroid_parent[centroid] = parent_centroid;
        removed[centroid] = true;
 
        for (int u : adj[centroid]) {
            if (!removed[u]) {
                decompose(u, centroid);
            }
        }
    }
 
    /**
     * @brief Builds the centroid decomposition starting from node 1.
     */
    void build() { decompose(1, -1); }
 
    /**
     * @brief Returns the centroid parent of a node.
     * @param v node index
     * @return centroid parent of v, or -1 if v is the centroid root
     */
    int get_centroid_parent(int v) const { return centroid_parent[v]; }
};
 
/**
 * @brief Counts paths of a given target length in the tree using
 *        centroid decomposition.
 *
 * Uses the standard centroid decomposition approach:
 * For each centroid c, count paths passing through c.
 * A path passes through c if it combines a distance from c in one subtree
 * with a distance from c in another subtree summing to target_len.
 *
 * @param n number of nodes
 * @param edges list of edges as pairs (u, v)
 * @param target_len target path length to count
 * @return number of unordered pairs (u, v) with path length == target_len
 */
int count_paths_of_length(int n,
                           const std::vector<std::pair<int, int>>& edges,
                           int target_len) {
    std::vector<std::vector<int>> adj(n + 1);
    for (auto& e : edges) {
        adj[e.first].push_back(e.second);
        adj[e.second].push_back(e.first);
    }
 
    std::vector<int> subtree_sz(n + 1, 0);
    std::vector<bool> removed(n + 1, false);
    int result = 0;
 
    std::function<int(int, int)> calc_size = [&](int v, int p) -> int {
        subtree_sz[v] = 1;
        for (int u : adj[v]) {
            if (u != p && !removed[u]) {
                subtree_sz[v] += calc_size(u, v);
            }
        }
        return subtree_sz[v];
    };
 
    std::function<int(int, int, int)> find_centroid =
        [&](int v, int p, int sz) -> int {
        for (int u : adj[v]) {
            if (u != p && !removed[u] && subtree_sz[u] > sz / 2) {
                return find_centroid(u, v, sz);
            }
        }
        return v;
    };
 
    // collect distances from a node via DFS
    std::function<void(int, int, int, std::vector<int>&)> collect =
        [&](int v, int p, int dist, std::vector<int>& dists) {
        dists.push_back(dist);
        for (int u : adj[v]) {
            if (u != p && !removed[u]) {
                collect(u, v, dist + 1, dists);
            }
        }
    };
 
    // count pairs from a centroid using frequency map
    auto count_from_centroid = [&](int c) {
        std::map<int, int> freq;
        freq[0] = 1;  // centroid itself at distance 0
 
        for (int nb : adj[c]) {
            if (removed[nb]) continue;
            std::vector<int> dists;
            collect(nb, c, 1, dists);
 
            // count valid pairs with distances already recorded
            for (int d : dists) {
                int need = target_len - d;
                if (need >= 0 && freq.count(need)) {
                    result += freq[need];
                }
            }
            // add new distances to frequency map
            for (int d : dists) {
                freq[d]++;
            }
        }
    };
 
    std::function<void(int)> solve = [&](int v) {
        int sz = calc_size(v, -1);
        int c = find_centroid(v, -1, sz);
        count_from_centroid(c);
        removed[c] = true;
        for (int u : adj[c]) {
            if (!removed[u]) {
                solve(u);
            }
        }
    };
 
    solve(1);
    return result;
}
 
}  // namespace graph
 
/**
 * @brief Unit tests for centroid decomposition
 * @returns none
 */
static void tests() {
    // Test 1: basic centroid parent structure on a path graph 1-2-3-4-5
    {
        graph::CentroidDecomposition cd(5);
        cd.add_edge(1, 2);
        cd.add_edge(2, 3);
        cd.add_edge(3, 4);
        cd.add_edge(4, 5);
        cd.build();
 
        // centroid of 1-2-3-4-5 is node 3
        // its centroid parent should be -1 (root of centroid tree)
        assert(cd.get_centroid_parent(3) == -1);
        // nodes in subtrees of 3 should have 3 as centroid parent
        assert(cd.get_centroid_parent(2) == 3 ||
               cd.get_centroid_parent(4) == 3);
    }
 
    // Test 2: count paths of length 2 in a star graph (center = 1)
    {
        /*
         *     1
         *   / | \
         *  2  3  4
         * paths of length 2: (2,3), (2,4), (3,4) => 3 paths
         */
        std::vector<std::pair<int, int>> edges = {{1, 2}, {1, 3}, {1, 4}};
        int result = graph::count_paths_of_length(4, edges, 2);
        assert(result == 3);
    }
 
    // Test 3: count paths of length 2 in a path graph 1-2-3-4-5
    {
        /*
         * 1-2-3-4-5
         * paths of length 2: (1,3), (2,4), (3,5) => 3 paths
         */
        std::vector<std::pair<int, int>> edges = {
            {1, 2}, {2, 3}, {3, 4}, {4, 5}};
        int result = graph::count_paths_of_length(5, edges, 2);
        assert(result == 3);
    }
 
    // Test 4: count paths of length 1 (edges) in a path of 3 nodes
    {
        // 1-2-3: edges = (1,2), (2,3) => 2 paths of length 1
        std::vector<std::pair<int, int>> edges = {{1, 2}, {2, 3}};
        int result = graph::count_paths_of_length(3, edges, 1);
        assert(result == 2);
    }
 
    // Test 5: single node tree — no paths
    {
        std::vector<std::pair<int, int>> edges = {};
        int result = graph::count_paths_of_length(1, edges, 1);
        assert(result == 0);
    }
}
 
/**
 * @brief Main function
 * @returns 0 on success
 */
int main() {
    tests();
    return 0;
}
 
