#ifndef LOGIN_MANAGER_H
#define LOGIN_MANAGER_H

#include <string>
#include <unordered_map>
#include "connection_manager.h"

class LoginManager {
public:
    LoginManager(ConnectionManager& cm);
    
    // Authenticates the user with given ID and password
    bool authenticateUser(const std::string& user_id, const std::string& password);

    // Logs in the user if credentials are correct
    User* login(const std::string& id, const std::string& password);

    // Registers a new user if ID does not already exist
    bool registerUser(const std::string& name, const std::string& id, const std::string& password, const std::string& category, double influence, const std::string& branch);

    // Checks if the user exists in the system
    bool isUserExist(const std::string& id);

    // Gets user object by user ID
    User* getUserById(const std::string& id);

private:
    ConnectionManager& cm;
    std::unordered_map<std::string, User*> users;
    
    // Loads user data from the file into memory
    void loadUsers();
    
    // Saves user data to the file
    void saveUsers();
};

#endif
