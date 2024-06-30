#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;

enum class ProductState { AVAILABLE, UNAVAILABLE };
enum class PaymentMethod { COD, BANKING };
enum class PaymentStatus { PAID, PENDING };
enum class AccountType { ADMIN, USER };

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void waitAndReturn(int seconds) {
    this_thread::sleep_for(chrono::seconds(seconds));
}

class Product {
public:
    string name;
    int remain;
    int price;
    ProductState product_state;
    string product_code;

    Product(const string& name, int remain, int price, ProductState state, const string& code)
        : name(name), remain(remain), price(price), product_state(state), product_code(code) {}
};

void displayProducts(const vector<Product>& products) {
    cout << left << setw(5) << "STT"
         << setw(20) << "Ten san pham"
         << setw(15) << "So luong"
         << setw(15) << "Gia"
         << setw(15) << "Trang thai"
         << setw(15) << "Ma san pham" << endl;
    cout << string(85, '-') << endl;

    for (size_t i = 0; i < products.size(); ++i) {
        const auto& product = products[i];
        cout << left << setw(5) << (i + 1)
             << setw(20) << product.name
             << setw(15) << product.remain
             << setw(15) << product.price
             << setw(15) << (product.product_state == ProductState::AVAILABLE ? "Con hang" : "Het")
             << setw(15) << product.product_code << endl;
    }
}

void addProduct(vector<Product>& products) {
    string name, code;
    int remain, price;
    ProductState state;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Nhap ten san pham: ";
    getline(cin, name);
    cout << "Nhap so luong: ";
    cin >> remain;
    cout << "Nhap gia: ";
    cin >> price;
    cout << "Nhap trang thai (0 - Het hang, 1 - Con hang): ";
    int stateChoice;
    cin >> stateChoice;
    state = stateChoice == 1 ? ProductState::AVAILABLE : ProductState::UNAVAILABLE;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Nhap ma san pham: ";
    getline(cin, code);

    for (const auto& product : products) {
        if (product.product_code == code) {
            cout << "Ma san pham da ton tai. Vui long thu lai." << endl;
            waitAndReturn(2);
            return;
        }
    }

    cout << "Xac nhan them ma san pham [Y/N]? ";
    char confirm;
    cin >> confirm;

    if (confirm == 'Y' || confirm == 'y') {
        products.push_back(Product(name, remain, price, state, code));
        cout << "Da them san pham thanh cong." << endl;
    } else {
        cout << "Da huy them san pham." << endl;
    }

    waitAndReturn(2);
}

void removeProduct(vector<Product>& products) {
    string code;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Nhap ma san pham can xoa: ";
    getline(cin, code);

    auto it = find_if(products.begin(), products.end(),
                      [&code](const Product& p) { return p.product_code == code; });

    if (it != products.end()) {
        cout << "Xac nhan xoa san pham [Y/N]? ";
        char confirm;
        cin >> confirm;

        if (confirm == 'Y' || confirm == 'y') {
            products.erase(it);
            cout << "Da xoa san pham thanh cong." << endl;
        } else {
            cout << "Da huy xoa san pham." << endl;
        }
    } else {
        cout << "Khong tim thay san pham voi ma. " << code << endl;
    }

    waitAndReturn(2);
}

void editProduct(vector<Product>& products) {
    string code;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Nhap ma san pham can sua: ";
    getline(cin, code);

    auto it = find_if(products.begin(), products.end(),
                      [&code](const Product& p) { return p.product_code == code; });

    if (it != products.end()) {
        cout << "Chon thong tin can sua:" << endl;
        cout << "1. Ten san pham" << endl;
        cout << "2. Gia san pham" << endl;
        cout << "Chon: ";
        int choice;
        cin >> choice;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1) {
            cout << "Nhap ten moi: ";
            getline(cin, it->name);
        } else if (choice == 2) {
            cout << "Nhap gia moi: ";
            cin >> it->price;
        } else {
            cout << "Lua chon khong hop le." << endl;
            waitAndReturn(2);
            return;
        }

        cout << "Xac nhan thay doi [Y/N]? ";
        char confirm;
        cin >> confirm;

        if (confirm == 'Y' || confirm == 'y') {
            cout << "Da cap nhat san pham thanh cong." << endl;
        } else {
            cout << "Da huy cap nhat san pham." << endl;
        }
    } else {
        cout << "Khong tim thay san pham voi ma. " << code << endl;
    }

    waitAndReturn(2);
}

class ShoppingCart {
public:
    vector<tuple<Product, int, int>> product_list;
    int total_price;
    string customer_code;

    void addProduct(const Product& product, int quantity) {
        product_list.push_back(make_tuple(product, quantity, product.price * quantity));
        updateTotalPrice();
    }

    void updateTotalPrice() {
        total_price = 0;
        for (const auto& item : product_list) {
            total_price += get<2>(item);
        }
    }

    void displayCart() {
        cout << left << setw(20) << "Ten san pham"
             << setw(15) << "So luong"
             << setw(15) << "Gia" << endl;
        cout << string(50, '-') << endl;

        for (const auto& item : product_list) {
            cout << left << setw(20) << get<0>(item).name
                 << setw(15) << get<1>(item)
                 << setw(15) << get<2>(item) << endl;
        }
        cout << "Tổng giá: " << total_price << endl;
    }
};

class Order {
public:
    string customer_name;
    string delivery_address;
    string phone_number;
    vector<tuple<Product, int, int>> product_list;
    int total_price;
    PaymentMethod payment_method;
    PaymentStatus payment_status;
    string order_code;

    Order(const string& name, const string& address, const string& phone,
          const vector<tuple<Product, int, int>>& products, int total,
          PaymentMethod method, PaymentStatus status, const string& code)
        : customer_name(name), delivery_address(address), phone_number(phone),
          product_list(products), total_price(total), payment_method(method),
          payment_status(status), order_code(code) {}
};

class Account {
public:
    string username;
    string password;
    string full_name;
    string phone_number;
    string email;
    int age;
    string account_code;
    AccountType account_type;

    Account(const string& username, const string& password, const string& full_name,
            const string& phone, const string& email, int age, const string& code,
            AccountType type)
        : username(username), password(password), full_name(full_name), phone_number(phone),
          email(email), age(age), account_code(code), account_type(type) {}

    virtual ~Account() = default;
};

class Admin : public Account {
public:
    Admin(const string& username, const string& password, const string& full_name,
          const string& phone, const string& email, int age, const string& code)
        : Account(username, password, full_name, phone, email, age, code, AccountType::ADMIN) {}
};

class User : public Account {
public:
    ShoppingCart shopping_cart;
    vector<Order> orders;

    User(const string& username, const string& password, const string& full_name,
         const string& phone, const string& email, int age, const string& code)
        : Account(username, password, full_name, phone, email, age, code, AccountType::USER) {}
};

void saveAccountToCSV(const Account& account) {
    string filename = (account.account_type == AccountType::ADMIN) ? "admin.csv" : "user.csv";
    ofstream file(filename, ios::app);
    if (file.is_open()) {
        file << account.username << ","
             << account.password << ","
             << account.full_name << ","
             << account.phone_number << ","
             << account.email << ","
             << account.age << ","
             << account.account_code << "\n";
        file.close();
    } else {
        cerr << "Khong the mo file " << filename << " de ghi.." << endl;
    }
}

void loadAccountsFromCSV(vector<Account*>& accounts) {
    vector<string> filenames = {"admin.csv", "user.csv"};
    
    for (const auto& filename : filenames) {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string username, password, full_name, phone_number, email, age_str, account_code;

                getline(ss, username, ',');
                getline(ss, password, ',');
                getline(ss, full_name, ',');
                getline(ss, phone_number, ',');
                getline(ss, email, ',');
                getline(ss, age_str, ',');
                getline(ss, account_code, ',');

                int age = stoi(age_str);
                AccountType account_type = (filename == "admin.csv") ? AccountType::ADMIN : AccountType::USER;

                if (account_type == AccountType::ADMIN) {
                    accounts.push_back(new Admin(username, password, full_name, phone_number, email, age, account_code));
                } else {
                    accounts.push_back(new User(username, password, full_name, phone_number, email, age, account_code));
                }
            }
            file.close();
        } else if (filename == "user.csv") {
            continue;
        } else {
            cerr << "Khong the mo file " << filename << endl;
        }
    }
}

void registerAccount() {
    string username, password, full_name, phone, email;
    int age;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Nhap ten tai khoan: ";
    getline(cin, username);
    cout << "Nhap mat khau: ";
    getline(cin, password);
    cout << "Nhap ho va ten: ";
    getline(cin, full_name);
    cout << "Nhap so dien thoai: ";
    getline(cin, phone);
    cout << "Nhap email: ";
    getline(cin, email);
    cout << "Nhap tuoi: ";
    cin >> age;

    string account_code = "U" + to_string(rand() % 10000);
    User newUser(username, password, full_name, phone, email, age, account_code);
    saveAccountToCSV(newUser);

    cout << "Dang ki thanh cong!" << endl;
    waitAndReturn(2);
}

void initializeAdminAccount() {
    ifstream file("admin.csv");
    if (!file.good()) {
        ofstream outFile("admin.csv");
        if (outFile.is_open()) {
            outFile << "admin,admin123,Admin,0123456789,admin@example.com,30,A001\n";
            outFile.close();
            cout << "File admin.csv da duoc tao voi tai khoan admin mac dinh." << endl;
        } else {
            cerr << "Khong the tao file admin.csv" << endl;
        }
    } else {
        file.close();
    }
}

Account* login(const vector<Account*>& accounts) {
    string username, password;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Nhap ten dang nhap: ";
    getline(cin, username);
    cout << "Nhap mat khau: ";
    getline(cin, password);

    for (auto account : accounts) {
        if (account->username == username && account->password == password) {
            cout << "Dang nhap thanh cong!" << endl;
            waitAndReturn(2);
            return account;
        }
    }

    cout << "Dang nhap that bai. Ten dang nhap hoạc mat khau khong trung khop" << endl;
    waitAndReturn(2);
    return nullptr;
}


void addToCart(User* user, vector<Product>& products) {
    string product_code;
    int quantity;

    cout << "Nhap ma san pham: ";
    cin >> product_code;
    cout << "Nhap so luong: ";
    cin >> quantity;

    auto it = find_if(products.begin(), products.end(),
                      [&product_code](const Product& p) { return p.product_code == product_code; });

    if (it != products.end()) {
        if (it->remain >= quantity) {
            user->shopping_cart.addProduct(*it, quantity);
            it->remain -= quantity;
            cout << "Da them san pham vao gio hang." << endl;
        } else {
            cout << "So luong san pham khong du." << endl;
        }
    } else {
        cout << "Khong tim thay san pham." << endl;
    }
    waitAndReturn(2);
}

void checkout(User* user) {
    if (user->shopping_cart.product_list.empty()) {
        cout << "Gio hang trong." << endl;
        waitAndReturn(2);
        return;
    }

    user->shopping_cart.displayCart();

    string address, payment_method_str;
    cout << "Nhap dia chi giao hang: ";
    cin.ignore();
    getline(cin, address);
    cout << "Chon phuong thuc thanh toan (COD/BANKING): ";
    cin >> payment_method_str;

    PaymentMethod payment_method = (payment_method_str == "BANKING") ? PaymentMethod::BANKING : PaymentMethod::COD;
    PaymentStatus payment_status = (payment_method == PaymentMethod::BANKING) ? PaymentStatus::PAID : PaymentStatus::PENDING;

    string order_code = "O" + to_string(rand() % 10000);
    Order new_order(user->full_name, address, user->phone_number, user->shopping_cart.product_list,
                    user->shopping_cart.total_price, payment_method, payment_status, order_code);

    user->orders.push_back(new_order);
    user->shopping_cart = ShoppingCart(); // Reset giỏ hàng

    cout << "Dat hang thanh cong. Ma don hang: " << order_code << endl;
    waitAndReturn(2);
}

void userMenu(User* user, vector<Product>& products) {
    int choice;
    do {
        clearScreen();
        cout << "Menu User" << endl;
        cout << "1. Xem danh sach san pham" << endl;
        cout << "2. Xem gio hang" << endl;
        cout << "3. Thanh toan" << endl;
        cout << "4. Dang xuat" << endl;
        cout << "Chon: ";
        cin >> choice;

        switch (choice) {
            case 1:
                clearScreen();
                displayProducts(products);
                addToCart(user, products);
                break;
            case 2:
                clearScreen();
                user->shopping_cart.displayCart();
                cout << "Nhan Enter de tiep tuc...";
                cin.ignore();
                cin.get();
                break;
            case 3:
                clearScreen();
                checkout(user);
                break;
            case 4:
                cout << "Dang xuat thanh cong." << endl;
                waitAndReturn(2);
                break;
            default:
                cout << "Lua chon khong hop le. Vui long chon lai." << endl;
                waitAndReturn(2);
        }
    } while (choice != 4);
}

void adminMenu(Admin* admin, vector<Product>& products) {
    int choice;
    do {
        clearScreen();
        cout << "Menu Admin" << endl;
        cout << "1. Xem danh sach san pham" << endl;
        cout << "2. Them san pham" << endl;
        cout << "3. Xoa san pham" << endl;
        cout << "4. Sua san pham" << endl;
        cout << "5. Dang xuat" << endl;
        cout << "Chon: ";
        cin >> choice;

        switch (choice) {
            case 1:
                clearScreen();
                displayProducts(products);
                cout << "Nhan Enter de tiep tuc...";
                cin.ignore();
                cin.get();
                break;
            case 2:
                clearScreen();
                addProduct(products);
                break;
            case 3:
                clearScreen();
                removeProduct(products);
                break;
            case 4:
                clearScreen();
                editProduct(products);
                break;
            case 5:
                cout << "Dang xuat thanh cong." << endl;
                waitAndReturn(2);
                break;
            default:
                cout << "Lua chon khong hop le. Vui long chon lai." << endl;
                waitAndReturn(2);
        }
    } while (choice != 5);
}

int main() {
    vector<Product> products;
    vector<Account*> accounts;

    // Thêm một số sản phẩm mẫu
    products.push_back(Product("Ao thun", 100, 150000, ProductState::AVAILABLE, "P001"));
    products.push_back(Product("Quan jean", 50, 350000, ProductState::AVAILABLE, "P002"));
    products.push_back(Product("Giay sneaker", 30, 500000, ProductState::AVAILABLE, "P003"));

    // Khởi tạo tài khoản admin nếu chưa có
    initializeAdminAccount();

    // Đọc tài khoản từ file CSV
    loadAccountsFromCSV(accounts);

    int choice;
    do {
        clearScreen();
        cout << "Chao mung den voi Shop pe phake!" << endl;
        cout << "1. Dang ky" << endl;
        cout << "2. Dang nhap" << endl;
        cout << "3. Thoat" << endl;
        cout << "Chon: ";
        cin >> choice;

        switch (choice) {
            case 1:
                registerAccount();
                // Sau khi đăng ký, cập nhật lại danh sách tài khoản
                accounts.clear();
                loadAccountsFromCSV(accounts);
                break;
            case 2: {
                Account* loggedInAccount = login(accounts);
                if (loggedInAccount) {
                    if (loggedInAccount->account_type == AccountType::ADMIN) {
                        adminMenu(static_cast<Admin*>(loggedInAccount), products);
                    } else {
                        userMenu(static_cast<User*>(loggedInAccount), products);
                    }
                }
                break;
            }
            case 3:
                cout << "Cam on quy khach da su dung dich vu!" << endl;
                break;
            default:
                cout << "Lua chon khong hop le. Vui long chon lai." << endl;
                waitAndReturn(2);
        }
    } while (choice != 3);

    // Giải phóng bộ nhớ
    for (auto account : accounts) {
        delete account;
    }

    return 0;
}