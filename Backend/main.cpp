#include "headers/json.hpp"
#include "headers/buyer.hpp"
#include "headers/seller.hpp"
#include "headers/analytics.hpp"
#include <iostream>
#include <limits>
#include <string>

using namespace std;

// Reads a full line safely (handles names with spaces, clears bad input state).
static string read_line(const string& prompt) {
    cout << prompt;
    string line;
    getline(cin, line);
    return line;
}

static void seller_menu() {
    cout << "\n--- Seller Login ---\n";
    string name = read_line("Username: ");

    if (seller::sellers.find(name) == seller::sellers.end()) {
        cout << "No seller with that username yet. Register first? (y/n): ";
        string ans;
        getline(cin, ans);
        if (ans == "y" || ans == "Y") {
            string shopname = read_line("Shop name: ");
            seller::register_user(name, shopname);
            cout << "Registered.\n";
        } else {
            return;
        }
    }

    while (true) {
        cout << "\n[Seller: " << name << "]\n"
             << "1. Add item\n"
             << "2. Show my products\n"
             << "3. Show trending products (across marketplace)\n"
             << "4. Log out\n"
             << "Choice: ";
        string choice;
        getline(cin, choice);

        if (choice == "1") {
            string category = read_line("Category: ");
            string product = read_line("Product name: ");
            seller::add_item(name, category, product);
            cout << "Added.\n";
        } else if (choice == "2") {
            seller::show_current_products(name);
        } else if (choice == "3") {
            analytics::show_trending();
        } else if (choice == "4") {
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }
}

static void buyer_menu() {
    cout << "\n--- Buyer Login ---\n";
    string name = read_line("Username: ");

    if (buyer::buyers.find(name) == buyer::buyers.end()) {
        cout << "No buyer with that username yet. Register first? (y/n): ";
        string ans;
        getline(cin, ans);
        if (ans == "y" || ans == "Y") {
            buyer::register_user(name);
            cout << "Registered.\n";
        } else {
            return;
        }
    }

    while (true) {
        cout << "\n[Buyer: " << name << "]\n"
             << "1. Browse all products\n"
             << "2. Add product to cart\n"
             << "3. Add product to favourites\n"
             << "4. Show my cart\n"
             << "5. Show my favourites\n"
             << "6. Show trending products\n"
             << "7. Log out\n"
             << "Choice: ";
        string choice;
        getline(cin, choice);

        if (choice == "1") {
            buyer::show_all_products();
        } else if (choice == "2") {
            string product = read_line("Product to add to cart: ");
            buyer::add_to_cart(name, product);
            cout << "Added to cart.\n";
        } else if (choice == "3") {
            string product = read_line("Product to favourite: ");
            buyer::add_favourite(name, product);
            cout << "Added to favourites.\n";
        } else if (choice == "4") {
            buyer::show_cart(name);
        } else if (choice == "5") {
            buyer::show_favourites(name);
        } else if (choice == "6") {
            analytics::show_trending();
        } else if (choice == "7") {
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }
}

int main() {
    // Load both JSON files into their respective hash maps once at startup.
    seller::load_from_file();
    buyer::load_from_file();

    while (true) {
        cout << "\n=== E-Commerce Backend ===\n"
             << "1. Continue as Buyer\n"
             << "2. Continue as Seller\n"
             << "3. Exit\n"
             << "Choice: ";
        string choice;
        getline(cin, choice);

        if (choice == "1") {
            buyer_menu();
        } else if (choice == "2") {
            seller_menu();
        } else if (choice == "3") {
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }

    cout << "Goodbye.\n";
    return 0;
}
