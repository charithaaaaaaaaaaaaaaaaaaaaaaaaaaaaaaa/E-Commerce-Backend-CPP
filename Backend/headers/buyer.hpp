#ifndef BUYER_H
#define BUYER_H

#include <iostream>
#include <vector>
#include <string>
#include "json.hpp"
#include <fstream>
#include <unordered_map>
#include "seller.hpp"

using namespace std;
using json = nlohmann::json;

namespace buyer {

    // In-memory representation of a buyer, kept in a hash map for O(1) lookup by username.
    struct Buyer {
        vector<string> cart;
        vector<string> favourites;
    };

    // username -> Buyer.
    inline unordered_map<string, Buyer> buyers;

    const string BUYER_JSON_PATH = "../JSON/buyer.json";

    inline void save_to_file() {
        json j;
        for (const auto& [username, b] : buyers) {
            j["users"][username]["cart"] = b.cart;
            j["users"][username]["favourites"] = b.favourites;
        }

        ofstream out(BUYER_JSON_PATH);
        if (!out) {
            cerr << "Could not open " << BUYER_JSON_PATH << " for writing.\n";
            return;
        }
        out << j.dump(4);
    }

    inline void load_from_file() {
        buyers.clear();

        ifstream in(BUYER_JSON_PATH);
        if (!in) {
            cerr << "Could not open " << BUYER_JSON_PATH << " for reading.\n";
            return;
        }

        json j;
        try {
            in >> j;
        } catch (const json::parse_error&) {
            // Empty or invalid file: start with no buyers rather than crashing.
            return;
        }

        if (!j.contains("users")) return;

        for (auto& [username, data] : j["users"].items()) {
            Buyer b;
            if (data.contains("cart")) {
                b.cart = data["cart"].get<vector<string>>();
            }
            if (data.contains("favourites")) {
                b.favourites = data["favourites"].get<vector<string>>();
            }
            buyers[username] = b;
        }
    }

    // Registers a new buyer with empty cart/favourites. Returns false if taken.
    inline bool register_user(const string& name) {
        if (buyers.find(name) != buyers.end()) {
            return false;
        }

        buyers[name] = Buyer{};
        save_to_file();
        return true;
    }

    // Lists every product from every seller currently loaded in seller::sellers.
    inline void show_all_products() {
        if (seller::sellers.empty()) {
            cout << "No sellers/products available yet.\n";
            return;
        }

        for (const auto& [seller_name, s] : seller::sellers) {
            cout << "Shop: " << s.shop_name << " (seller: " << seller_name << ")\n";
            for (const auto& [category, items] : s.products) {
                cout << "  " << category << ": ";
                for (const auto& item : items) cout << item << " ";
                cout << "\n";
            }
        }
    }

    inline bool add_to_cart(const string& name, const string& product) {
        auto it = buyers.find(name);
        if (it == buyers.end()) return false;

        it->second.cart.push_back(product);
        save_to_file();
        return true;
    }

    inline bool add_favourite(const string& name, const string& product) {
        auto it = buyers.find(name);
        if (it == buyers.end()) return false;

        it->second.favourites.push_back(product);
        save_to_file();
        return true;
    }

    inline void show_cart(const string& name) {
        auto it = buyers.find(name);
        if (it == buyers.end()) {
            cout << "No such buyer: " << name << "\n";
            return;
        }
        if (it->second.cart.empty()) {
            cout << "Cart is empty.\n";
            return;
        }
        cout << "Cart: ";
        for (const auto& item : it->second.cart) cout << item << " ";
        cout << "\n";
    }

    inline void show_favourites(const string& name) {
        auto it = buyers.find(name);
        if (it == buyers.end()) {
            cout << "No such buyer: " << name << "\n";
            return;
        }
        if (it->second.favourites.empty()) {
            cout << "No favourites yet.\n";
            return;
        }
        cout << "Favourites: ";
        for (const auto& item : it->second.favourites) cout << item << " ";
        cout << "\n";
    }
}

#endif
