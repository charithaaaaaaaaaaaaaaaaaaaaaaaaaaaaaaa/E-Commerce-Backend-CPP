# E-Commerce Backend (C++)

A small e-commerce backend implemented in C++, driven entirely through a
menu-based CLI. No GUI. Buyer and seller data is persisted to JSON and
loaded into hash maps at runtime for fast lookups.

## Data Structures Used

- **Hash maps (`std::unordered_map`)** — every buyer and seller is stored
  in an `unordered_map<string, ...>` keyed by username, giving O(1)
  average-case lookup instead of scanning the JSON file on every request.
- **Min-heap (`std::priority_queue`)** — the "trending products" feature
  computes demand across all buyers' carts and favourites, then uses a
  size-K min-heap to select the Top-K most in-demand products in
  `O(n log k)` instead of sorting the entire product list.

## Project Structure

```
Backend/
  main.cpp               CLI entry point — buyer/seller menus
  headers/
    buyer.hpp             Buyer logic + persistence
    seller.hpp            Seller logic + persistence
    analytics.hpp         Top-K trending products (min-heap)
    json.hpp              nlohmann/json (vendored)
JSON/
  buyer.json              Persisted buyer data
  seller.json             Persisted seller data
```

## Convention

Function names use `snake_case` (e.g. `register_user`, `add_to_cart`,
`show_current_products`).

## General Flow

Functionality is split across three header files:

1. **`buyer.hpp`** — everything related to buyers:
   `register_user`, `show_all_products`, `add_to_cart`, `add_favourite`,
   `show_cart`, `show_favourites`.
2. **`seller.hpp`** — everything related to sellers:
   `register_user`, `add_item`, `show_current_products`.
3. **`analytics.hpp`** — marketplace-wide analytics:
   `top_k_trending`, `show_trending`.

`main.cpp` wires these together into a simple text menu: choose to
continue as a Buyer or a Seller, then register/log in and act.

## Buyer Side — Internal Working

- On startup, `buyer::load_from_file()` reads `buyer.json` and populates
  the `buyer::buyers` hash map (`username -> Buyer{cart, favourites}`)
  for fast access.
- Every mutation (`add_to_cart`, `add_favourite`, `register_user`)
  updates the in-memory map first, then calls `save_to_file()` to
  persist the full map back to `buyer.json`.
- Registering a new buyer creates an entry with empty `cart` and
  `favourites`, then writes it to the JSON file immediately.
- `show_all_products()` reads across every seller currently loaded in
  `seller::sellers` so a buyer can browse the whole marketplace.

## Seller Side — Internal Working

- On startup, `seller::load_from_file()` reads `seller.json` into the
  `seller::sellers` hash map (`username -> Seller{shop_name, products}`).
- `products` is itself a hash map from category to a list of product
  names, so items can be added per category with `add_item`.
- Registration works the same way as the buyer side: new entry added to
  the map, then persisted to `seller.json`.
- The loader tolerates malformed/legacy category data (e.g. a bare
  string instead of an array) instead of crashing on load.

## Trending Products — Internal Working

- `analytics::compute_demand()` scans every buyer's `cart` and
  `favourites` and tallies how many times each product name appears
  across the whole marketplace.
- `analytics::top_k_trending(k)` pushes each product into a size-`k`
  min-heap keyed on demand count. Once the heap is full, any product
  with a higher count than the current minimum evicts it — so the heap
  always holds the current Top-K without ever sorting the full list.
- Available to both buyers and sellers from the CLI menu as
  "Show trending products".

## Building and Running

```bash
cd Backend
g++ -std=c++17 -Wall main.cpp -o ecommerce
./ecommerce
```

The program looks for `../JSON/buyer.json` and `../JSON/seller.json`
relative to the `Backend` directory, so run it from inside `Backend/`.

## Notes

- `buyer.json` and `seller.json` ship as empty (`{}`) so the app starts
  from a clean slate — all data is created by registering and using the
  CLI, not by pre-seeded fixtures.
- There is no GUI. Everything is driven through the CLI in `main.cpp`.
