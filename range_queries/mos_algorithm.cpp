/**
 * @file mos_algorithm.cpp
 * @brief Mo's Algorithm for Range Query problems
 *
 * Mo's Algorithm is an offline algorithm that answers range queries
 * in O((N + Q) * sqrt(N)) time by sorting queries in a specific order
 * to minimize the number of add/remove operations.
 *
 * @details
 * - Preprocessing Time: O((N + Q) * sqrt(N))
 * - Query Time: O(1) per query after processing
 * - Space: O(N + Q)
 *
 * Example: Count of distinct elements in range [L, R]
 *
 * @see https://en.wikipedia.org/wiki/Mo%27s_algorithm
 */

#include <algorithm>
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
 * @brief Mo's Algorithm to answer range queries offline
 * @details Counts distinct elements in each query range [L, R]
 */
class MosAlgorithm {
 private:
    int n;
    int block_size;
    std::vector<int> arr;
    std::vector<int> freq;
    int current_answer;

    /**
     * @brief Add element at index to current window
     * @param idx index of element to add
     */
    void add(int idx) {
        freq[arr[idx]]++;
        if (freq[arr[idx]] == 1) {
            current_answer++;
        }
    }

    /**
     * @brief Remove element at index from current window
     * @param idx index of element to remove
     */
    void remove(int idx) {
        freq[arr[idx]]--;
        if (freq[arr[idx]] == 0) {
            current_answer--;
        }
    }

 public:
    /**
     * @brief Constructor
     * @param input input array (1-indexed values for freq array)
     */
    explicit MosAlgorithm(const std::vector<int>& input) {
        n = input.size();
        block_size = std::max(1, static_cast<int>(std::sqrt(n)));
        arr = input;
        freq.assign(n + 1, 0);
        current_answer = 0;
    }

    /**
     * @brief Answer multiple range queries for distinct element count
     * @param queries vector of {L, R} pairs (0-indexed)
     * @return vector of answers for each query
     */
    std::vector<int> solve(std::vector<std::pair<int, int>> queries) {
        int q = queries.size();
        std::vector<int> order(q);
        std::vector<int> answers(q);

        // Sort queries by block of L, then by R
        for (int i = 0; i < q; i++) order[i] = i;
        std::sort(order.begin(), order.end(),
                  [&](int a, int b) {
                      int ba = queries[a].first / block_size;
                      int bb = queries[b].first / block_size;
                      if (ba != bb) return ba < bb;
                      return (ba & 1) ? (queries[a].second > queries[b].second)
                                      : (queries[a].second < queries[b].second);
                  });

        int cur_l = 0, cur_r = -1;
        freq.assign(n + 1, 0);
        current_answer = 0;

        for (int i : order) {
            int l = queries[i].first;
            int r = queries[i].second;

            while (cur_r < r) add(++cur_r);
            while (cur_l > l) add(--cur_l);
            while (cur_r > r) remove(cur_r--);
            while (cur_l < l) remove(cur_l++);

            answers[i] = current_answer;
        }
        return answers;
    }
};

}  // namespace range_queries

/**
 * @brief Test cases
 */
void tests() {
    std::vector<int> arr = {1, 2, 1, 3, 2, 4, 3, 2};
    range_queries::MosAlgorithm mo(arr);

    std::vector<std::pair<int, int>> queries = {
        {0, 4},  // [1,2,1,3,2] -> 3 distinct
        {1, 6},  // [2,1,3,2,4,3] -> 4 distinct
        {0, 7},  // [1,2,1,3,2,4,3,2] -> 4 distinct
        {3, 5},  // [3,2,4] -> 3 distinct
    };

    std::vector<int> answers = mo.solve(queries);

    assert(answers[0] == 3);
    assert(answers[1] == 4);
    assert(answers[2] == 4);
    assert(answers[3] == 3);

    std::cout << "All tests passed!\n";
}

/**
 * @brief Main function
 */
int main() {
    tests();
    return 0;
}