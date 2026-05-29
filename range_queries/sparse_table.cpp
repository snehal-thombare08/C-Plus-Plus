/**
 * @file sparse_table.cpp
 * @brief Sparse Table for Range Minimum Query (RMQ)
 *
 * Sparse Table is a data structure that allows answering
 * range minimum queries in O(1) time after O(n log n) preprocessing.
 *
 * @details
 * - Build Time: O(n log n)
 * - Query Time: O(1)
 * - Space: O(n log n)
 *
 * @see https://en.wikipedia.org/wiki/Sparse_table
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

/**
 * @namespace range_queries
 * @brief Range query algorithms
 */
namespace range_queries {

/**
 * @brief Sparse Table class for Range Minimum Query
 */
class SparseTable {
 private:
    int n;
    std::vector<std::vector<int>> table;
    std::vector<int> log2_floor;

 public:
    /**
     * @brief Constructor - builds the sparse table
     * @param arr input array
     */
    explicit SparseTable(const std::vector<int>& arr) {
        n = arr.size();
        int LOG = std::log2(n) + 1;

        table.assign(LOG, std::vector<int>(n));
        log2_floor.resize(n + 1);

        // Precompute log2 values
        log2_floor[1] = 0;
        for (int i = 2; i <= n; i++) {
            log2_floor[i] = log2_floor[i / 2] + 1;
        }

        // Base case
        for (int i = 0; i < n; i++) {
            table[0][i] = arr[i];
        }

        // Build table
        for (int j = 1; j < LOG; j++) {
            for (int i = 0; i + (1 << j) <= n; i++) {
                table[j][i] = std::min(table[j - 1][i],
                                       table[j - 1][i + (1 << (j - 1))]);
            }
        }
    }

    /**
     * @brief Query minimum in range [l, r]
     * @param l left index (0-based)
     * @param r right index (0-based)
     * @return minimum value in range [l, r]
     */
    int query(int l, int r) {
        int k = log2_floor[r - l + 1];
        return std::min(table[k][l], table[k][r - (1 << k) + 1]);
    }
};

}  // namespace range_queries

/**
 * @brief Test cases
 */
void tests() {
    std::vector<int> arr = {2, 4, 3, 1, 6, 7, 8, 9, 1, 7};
    range_queries::SparseTable st(arr);

    assert(st.query(0, 9) == 1);
    assert(st.query(0, 4) == 1);
    assert(st.query(0, 2) == 2);
    assert(st.query(5, 9) == 1);
    assert(st.query(2, 5) == 1);

    std::cout << "All tests passed!\n";
}

/**
 * @brief Main function
 */
int main() {
    tests();
    return 0;
}