# E-Commerce App Documentation

## Overview

This documentation provides an overview of an E-Commerce App implemented in C++ using SQLite for database management. The E-Commerce App allows users to register, log in, add products, list products, place orders, list their orders, and log out.

## Design Choices

- Programming Language: C++ is chosen for its efficiency and the availability of the SQLite library for database management.

- Database: SQLite is used as the database management system due to its lightweight and embedded nature.

- Data Structures: The application uses the following data structures:
    - struct Product: Represents product information with fields for ID, name, price, and description.
    - struct Order: Represents order information with fields for ID, username, product, and order time.
    - class ECommerceApp: The main class that encapsulates the application's functionality.

## Database Schema

The app uses three tables in the SQLite database:

1. Users: Stores user information, including email, password, username, and role.
2. Product_Info: Stores product information, including product name, price, and description.
3. Order_Details: Stores order information, including product ID, user ID, and order time. It has foreign key references to the Users and Product_Info tables.

## How to Run and Use the System

1. Compile the Application:

    You can compile the application using a C++ compiler. Ensure you have the SQLite3 library installed or linked during compilation.

    
shell

    g++ -o ecommerceapp main.cpp -lsqlite3
    

2. Run the Application:

    Run the compiled executable.

    
shell

    ./ecommerceapp
    

3. Application Usage:

    - When you run the application, you will be presented with a menu.
    - You can choose options from the menu to interact with the system.

## Usage


 Steps for using the E-Commerce App:

1. Register a new user:
   - Choose option 1.
   - Enter user details.

2. Log in:
   - Choose option 2.
   - Enter the email and password you used for registration.

3. Add a product:
   - Choose option 4.
   - Enter product details.

4. List products:
   - Choose option 5.
   - View the list of available products.

5. Order a product:
   - Choose option 6.
   - Select a product from the list.

6. List orders:
   - Choose option 7.
   - View your order history.

7. Log out:
   - Choose option 3 to log out.

8. Exit the application:
   - Choose option 8 to exit the application.


## Team Members

   -Bayush Kibert.

   -Ruth Demise.

   -Endemariam Mehari.

   -Firdews Abdulmalik.

   -Natnael Tesfaye.

   -Natinael  Adefris .

