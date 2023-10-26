#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <sqlite3.h>

using namespace std;

struct Product {
    int id;
     string name;
    double price;
     string description;
};

struct Order {
    int id;
     string username;
    Product product;
    time_t orderTime;
};

class ECommerceApp {
private:
    sqlite3* db;
     string currentUser;
     vector<Product> products;
     vector<Order> orders;

public:
    ECommerceApp() {
        if (sqlite3_open("ecommerce.db", &db) != SQLITE_OK) {
             cerr << "Database connection failed." <<  endl;
            exit(1);
        }
        createTables();
    }

    ~ECommerceApp() {
        sqlite3_close(db);
    }

void createTables() {
        const char* createUserTableSQL =
            "CREATE TABLE IF NOT EXISTS Users ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "Email TEXT NOT NULL,"
            "Password TEXT NOT NULL,"
            "Username TEXT NOT NULL,"
            "Role TEXT NOT NULL"
            ");";

        const char* createProductTableSQL =
            "CREATE TABLE IF NOT EXISTS Product_Info ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "Product_Name TEXT NOT NULL,"
            "Product_Price REAL NOT NULL,"
            "Product_Description TEXT"
            ");";

        const char* createOrderTableSQL =
            "CREATE TABLE IF NOT EXISTS Order_Details ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "Product_ID INTEGER NOT NULL,"
            "Username TEXT NOT NULL,"
            "Order_Time DATETIME NOT NULL,"
            "FOREIGN KEY (Product_ID) REFERENCES Product_Info(ID)"
            ");";

        executeSQL(createUserTableSQL);
        executeSQL(createProductTableSQL);
        executeSQL(createOrderTableSQL);
    }

    void executeSQL(const char* sql) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
             cerr << "SQL error: " << errMsg <<  endl;
            sqlite3_free(errMsg);
        }
    }

void registerUser() {
         string email, password, username, role;
         cout << "Enter User Name: ";
         cin >> username;
         cout << "Enter Your Email: ";
         cin >> email;
         cout << "Enter Your Password: ";
         cin >> password;
         cout << "Enter Role (admin/customer): ";
         cin >> role;

        const char* sql = "INSERT INTO Users (Email, Password, Username, Role) VALUES (?, ?, ?, ?)";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
             cerr << "Error preparing statement: " << sqlite3_errmsg(db) <<  endl;
            return;
        }

        sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, role.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
             cerr << "Error executing statement: " << sqlite3_errmsg(db) <<  endl;
        }

        sqlite3_finalize(stmt);
    }

void listProducts() {
    const char* sql = "SELECT Product_Name, Product_Price, Product_Description FROM Product_Info;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
         cerr << "Error preparing statement: " << sqlite3_errmsg(db) <<  endl;
        return;
    }

     cout << "Product List:" <<  endl;
     cout << "Name\t\tPrice\tDescription" <<  endl;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* product_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        double product_price = sqlite3_column_double(stmt, 1);
        const char* product_description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

         cout << product_name << "\t" << product_price << "\t" << product_description <<  endl;
    }

    sqlite3_finalize(stmt);
}


void orderProduct() {
    if (currentUser.empty()) {
         cout << "Register or Login first to order a product." <<  endl;
        // Implement registration and login logic here if needed.
        return;
    }

     string product_name;
    listProducts();

     cout << "Enter the product name to order: ";
     cin.ignore(); // Consume the newline character left in the buffer.
     getline( cin, product_name);

    const char* sql = "INSERT INTO Order_Details (User_ID, Product_ID, Created_Date) "
                     "SELECT (SELECT ID FROM Users WHERE Email = ?), "
                     "(SELECT ID FROM Product_Info WHERE Product_Name = ?), "
                     "datetime('now')";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
         cerr << "Error preparing statement: " << sqlite3_errmsg(db) <<  endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, currentUser.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, product_name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) {
         cout << "Order placed successfully." <<  endl;
    } else {
         cerr << "Error executing statement: " << sqlite3_errmsg(db) <<  endl;
    }

    sqlite3_finalize(stmt);
}




void listOrders() {
    if (currentUser.empty()) {
         cout << "Please log in to view your orders." <<  endl;
        return;
    }

    const char* sql = "SELECT o.Created_Date, p.Product_Name "
                     "FROM Order_Details o "
                     "INNER JOIN Product_Info p ON o.Product_ID = p.ID "
                     "WHERE o.User_ID = (SELECT ID FROM Users WHERE Email = ?)";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
         cerr << "Error preparing statement: " << sqlite3_errmsg(db) <<  endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, currentUser.c_str(), -1, SQLITE_STATIC);

     cout << "Your Orders:" <<  endl;
     cout << "Order Date\t\tProduct Name" <<  endl;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* order_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* product_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

         cout << order_date << "\t" << product_name <<  endl;
    }

    sqlite3_finalize(stmt);
}


void loginUser() {
         string email, password;
         cout << "Enter Your Email: ";
         cin >> email;
         cout << "Enter Your Password: ";
         cin >> password;

        const char* sql = "SELECT * FROM Users WHERE Email = ? AND Password = ?";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
             cerr << "Error preparing statement: " << sqlite3_errmsg(db) <<  endl;
            return;
        }

        sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            currentUser = email;
             cout << "Login successful. Welcome, " << email << "!" <<  endl;
        } else {
             cerr << "Invalid email or password." <<  endl;
        }

        sqlite3_finalize(stmt);
    }

void addProduct() {
        if (currentUser.empty()) {
             cout << "Please log in to add a product." <<  endl;
            return;
        }

         string product_name, product_description;
        double product_price;

         cout << "Enter Product Name: ";
         cin.ignore(); // Ignore the newline character left in the buffer
         getline( cin, product_name);
         cout << "Enter Product Price: ";
         cin >> product_price;
         cin.ignore(); // Ignore the newline character
         cout << "Enter Product Description: ";
         getline( cin, product_description);

        const char* sql = "INSERT INTO Product_Info (Product_Name, Product_Price, Product_Description) VALUES (?, ?, ?)";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
             cerr << "Error preparing statement: " << sqlite3_errmsg(db) <<  endl;
            return;
        }

        sqlite3_bind_text(stmt, 1, product_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, product_price);
        sqlite3_bind_text(stmt, 3, product_description.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
             cout << "Product added successfully." <<  endl;
        } else {
             cerr << "Error executing statement: " << sqlite3_errmsg(db) <<  endl;
        }

        sqlite3_finalize(stmt);
    } 

    void prompt() {
        while (true) {
             cout << "Welcome to the E-Commerce App" <<  endl;
             cout << "1. Register\n2. Login\n3. Logout\n4. Add Product\n5. List Products\n6. Order Product\n7. List Orders\n8. Exit" <<  endl;
            int choice;
             cin >> choice;

            switch (choice) {
                case 1:
                    userRegistration();
                    break;
                case 2:
                    userLogin();
                    break;
                case 3:
                    logoutUser();
                    break;
                case 4:
                    addProduct();
                    break;
                case 5:
                    listProducts();
                    break;
                case 6:
                    orderProduct();
                    break;
                case 7:
                    listOrders();
                    break;
                case 8:
                    exit(0);
                default:
                     cout << "Invalid choice. Please try again." <<  endl;
            }
        }
    }
};

int main() {
    ECommerceApp app;
    app.prompt();
    return 0;
}
