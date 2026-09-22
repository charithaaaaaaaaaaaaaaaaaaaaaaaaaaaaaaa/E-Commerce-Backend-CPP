#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include "buyer.hpp"

using namespace std;

namespace analytics {

    // (product_name, demand_count) — demand_count is how many times a product
    // appears across every buyer's cart + favourites.
    using ProductDemand = pair<string, int>;

    // Comparator that makes std::priority_queue behave as a MIN-heap on count,
    // so the smallest-demand item sits at the top and gets evicted first.
    struct MinHeapCompare {
        bool operator()(const ProductDemand& a, const ProductDemand& b) const {
            return a.second > b.second; // reversed: smallest count has highest priority
        }
    };

    // Scans every buyer's cart and favourites and tallies demand per product.
    // O(total items across all buyers).
    inline unordered_map<string, int> compute_demand() {
        unordered_map<string, int> demand;

        for (const auto& [username, b] : buyer::buyers) {
            for (const auto& product : b.cart) {
                demand[product]++;
            }
            for (const auto& product : b.favourites) {
                demand[product]++;
            }
        }

        return demand;
    }

    // Returns the top-k most in-demand products using a size-k min-heap instead
    // of sorting the whole demand map: O(n log k) rather than O(n log n).
    // Result is returned highest-demand first.
    inline vector<ProductDemand> top_k_trending(int k) {
        unordered_map<string, int> demand = compute_demand();

        priority_queue<ProductDemand, vector<ProductDemand>, MinHeapCompare> min_heap;

        for (const auto& entry : demand) {
            if ((int)min_heap.size() < k) {
                min_heap.push(entry);
            } else if (!min_heap.empty() && entry.second > min_heap.top().second) {
                // This product beats the weakest item currently in the top-k:
                // evict it and let this one in.
                min_heap.pop();
                min_heap.push(entry);
            }
        }

        // Drain the heap into a vector, then reverse so the strongest is first.
        vector<ProductDemand> result;
        while (!min_heap.empty()) {
            result.push_back(min_heap.top());
            min_heap.pop();
        }
        reverse(result.begin(), result.end());

        return result;
    }

    inline void show_trending(int k = 5) {
        auto top = top_k_trending(k);

        if (top.empty()) {
            cout << "No demand data yet — no one has added anything to cart/favourites.\n";
            return;
        }

        cout << "\n--- Top " << top.size() << " Trending Products ---\n";
        int rank = 1;
        for (const auto& [product, count] : top) {
            cout << "  " << rank++ << ". " << product << "  (" << count << " buyer interactions)\n";
        }
    }
}

#endif
