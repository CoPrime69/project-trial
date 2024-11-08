#include "login_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

LoginManager::LoginManager(ConnectionManager& cm) : cm(cm) {
    loadUsers();
}

void LoginManager::loadUsers() {
    vector<User*> all_users = cm.getAllUsers();
    for (auto user : all_users) {
        users[user->getID()] = user;
    }
}

void LoginManager::saveUsers() {
    ofstream file("user_data.csv", ios::out | ios::trunc);
    if (file.is_open()) {
        file << "Name,ID,Password,Category,Influence,Branch\n";
        for (const auto& user_pair : users) {
            User* user = user_pair.second;
            file << user->getName() << ","
                 << user->getID() << ","
                 << user->getPassword() << ","
                 << user->getCategory() << ","
                 << user->getInfluence() << ","
                 << user->getBranch() << "\n";
        }
        file.close();
    } else {
        cout << "Error opening file for saving user data!" << endl;
    }
}

User* LoginManager::login(const string& id, const string& password) {
    if (users.find(id) != users.end()) {
        User* user = users[id];
        if (user->getPassword() == password) {
            cout << "Login successful!" << endl;
            return user;
        } else {
            cout << "Incorrect password!" << endl;
        }
    } else {
        cout << "No user found with that ID!" << endl;
    }
    return nullptr;
}

bool LoginManager::registerUser(const string& name, const string& id, const string& password, const string& category, double influence, const string& branch) {
    if (users.find(id) != users.end()) {
        cout << "User ID already exists!" << endl;
        return false;
    }

    User* new_user = new User(name, id, password, category, influence, branch);
    cm.addUser(new_user);
    users[id] = new_user;
    saveUsers();

    cout << "User registered successfully!" << endl;
    return true;
}

bool LoginManager::isUserExist(const string& id) {
    return users.find(id) != users.end();
}

User* LoginManager::getUserById(const string& id) {
    return users.find(id) != users.end() ? users[id] : nullptr;
}
