#ifndef SELLER_H
#define SELLER_H

#include <iostream>
#include <vector>
#include <string>
#include "json.hpp"
#include <fstream>
#include <unordered_map>

using namespace std;
using json = nlohmann::json;

namespace seller {

    // In-memory representation of a seller, kept in a hash map for O(1) lookup by username.
    struct Seller {
        string shop_name;
        // category -> list of products in that category
        unordered_map<string, vector<string>> products;
    };

    // username -> Seller. This is the "fetch data from seller.json and store it in
    // the map for faster access" step described in the README.
    inline unordered_map<string, Seller> sellers;

    const string SELLER_JSON_PATH = "../JSON/seller.json";

    inline void save_to_file() {
        json j;
        for (const auto& [username, s] : sellers) {
            j["users"][username]["shop_name"] = s.shop_name;
            for (const auto& [category, items] : s.products) {
                j["users"][username]["products"][category] = items;
            }
        }

        ofstream out(SELLER_JSON_PATH);
        if (!out) {
            cerr << "Could not open " << SELLER_JSON_PATH << " for writing.\n";
            return;
        }
        out << j.dump(4);
    }

    inline void load_from_file() {
        sellers.clear();

        ifstream in(SELLER_JSON_PATH);
        if (!in) {
            cerr << "Could not open " << SELLER_JSON_PATH << " for reading.\n";
            return;
        }

        json j;
        try {
            in >> j;
        } catch (const json::parse_error&) {
            // Empty or invalid file: start with no sellers rather than crashing.
            return;
        }

        if (!j.contains("users")) return;

        for (auto& [username, data] : j["users"].items()) {
            Seller s;
            s.shop_name = data.value("shop_name", "");

            if (data.contains("products")) {
                for (auto& [category, items] : data["products"].items()) {
                    if (items.is_array()) {
                        s.products[category] = items.get<vector<string>>();
                    } else if (items.is_string()) {
                        // Tolerate legacy/malformed entries like "home": "none"
                        // instead of crashing the whole load.
                        string val = items.get<string>();
                        if (val != "none" && !val.empty()) {
                            s.products[category] = { val };
                        }
                    }
                }
            }

            sellers[username] = s;
        }
    }

    // Registers a new seller. Returns false if the username is already taken.
    inline bool register_user(const string& name, const string& shopname) {
        if (sellers.find(name) != sellers.end()) {
            return false;
        }

        Seller s;
        s.shop_name = shopname;
        sellers[name] = s;

        save_to_file();
        return true;
    }

    // Adds a product under a category for an existing seller.
    inline bool add_item(const string& name, const string& category, const string& product) {
        auto it = sellers.find(name);
        if (it == sellers.end()) return false;

        it->second.products[category].push_back(product);
        save_to_file();
        return true;
    }

    // Prints everything the seller currently has listed.
    inline void show_current_products(const string& name) {
        auto it = sellers.find(name);
        if (it == sellers.end()) {
            cout << "No such seller: " << name << "\n";
            return;
        }

        cout << "Shop: " << it->second.shop_name << "\n";
        if (it->second.products.empty()) {
            cout << "  (no products listed yet)\n";
            return;
        }
        for (const auto& [category, items] : it->second.products) {
            cout << "  " << category << ": ";
            for (const auto& item : items) cout << item << " ";
            cout << "\n";
        }
    }
}

#endif
