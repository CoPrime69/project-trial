#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H
using namespace std;

#include "user.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

class ConnectionManager {
public:
    ConnectionManager(const  string& user_data_file);

    unordered_map< string, double> calculateBetweennessCentrality();
    unordered_map< string, double> calculateClosenessCentrality();
    unordered_map< string, double> calculatePageRank();
    vector<User*> getAllUsers();  // Function to get all users
    int getRandomPosition(int max);
    double getLuminance(const string &color);


    vector< vector<User*>> detectCommunities();
    void saveUserData(const string& file_path);
    vector< pair<User*, User*>> recommendConnectionsForNewUser(User* new_user);

    void visualizeGraph(const  string& output_file, const  unordered_map< string, double>& betweenness, const  vector< vector<User*>>& communities);
    void addUser(User* new_user);

    User* getUser(const  string& id) const;
    void addConnection(User* user1, User* user2);
    void loadUserData(const  string& file_path);
    void establishConnections();
    vector<User*> users;


private:
    unordered_map< string,  vector< string>> connections;

    
};

#endif // CONNECTION_MANAGER_H