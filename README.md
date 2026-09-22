# E-commerce Backend 
## Convention : pascal case : - eg . user_register,seller_register
## Backend implemented in CPP
## General Flow : -
  - Every functionalities is divided in two header files
  - 1. buyer.hpp : 
      -  Contains all the function related to buyer
      -  eg. show_all_products, register, add_favourite, add_to_cart
  - 2. seller.hpp :
       - Contains all the functions related to seller
       - eg . register, add_item, show_current_products

### User(Client) Side : -
- Internal Working :-
  - Fetch data from buyer.json file and store it in the hash map for faster access of data
  - If user want to add favourites or to cart store it in the file first and then update the map related categories and same goes with other items..
  - If user registers make a new object in json file and initialize empty values.
  - Functions : -
      - user_register()
### Seller Side : -
- Internal Working : -
    - Fetch the data from seller.json and store it in the map for faster access
    - Load all the parameters
    - On the seller side registration is same as buyer side
  

