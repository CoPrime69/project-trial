#ifndef LOGIN_MANAGER_H
#define LOGIN_MANAGER_H

#include <bits/stdc++.h>
#include "connection_manager.h"

using namespace std;

class LoginManager
{
public:
    LoginManager(ConnectionManager &cm);

    // Authenticates the user with given ID and password
    bool authenticateUser(const  string &user_id, const  string &password);

    // Logs in the user if credentials are correct
    User *login(const  string &id, const  string &password);

    // Registers a new user if ID does not already exist
    bool registerUser(const  string &name, const  string &id, const  string &password, const  string &category, double influence, const  string &branch);

    // Checks if the user exists in the system
    bool isUserExist(const  string &id);

    // Gets user object by user ID
    User *getUserById(const  string &id);

private:
    ConnectionManager &cm;
     unordered_map< string, User *> users;

    // Loads user data from the file into memory
    void loadUsers();

    // Saves user data to the file
    void saveUsers();
};

#endif
