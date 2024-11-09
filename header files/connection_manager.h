#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H
#include "adjacency_matrix.h"

using namespace std;

#include "user.h"
#include <bits/stdc++.h>

class ConnectionManager
{
public:
    ConnectionManager(const string &user_data_file);

    unordered_map<string, double> calculateBetweennessCentrality();
    unordered_map<string, double> calculateClosenessCentrality();
    unordered_map<string, double> calculatePageRank();
    vector<User *> getAllUsers(); // Function to get all users
    int getRandomPosition(int max);
    double getLuminance(const string &color);

    vector<vector<User *>> detectCommunities();
    void saveUserData(const string &file_path);
    vector<pair<User *, User *>> recommendConnectionsForNewUser(User *new_user);
    void removeConnection(User *user1, User *user2);
    void viewConnections(User *user);
    bool isValidConnection(const string &user1_id, const string &user2_id);

    void visualizeGraph(const string &output_file, const unordered_map<string, double> &betweenness, const vector<vector<User *>> &communities);
    void addUser(User *new_user);

    User *getUser(const string &id) const;
    void addConnection(User *user1, User *user2);
    void loadUserData(const string &file_path);
    void establishConnections();
    vector<User *> users;

    void initializeAdjacencyMatrix();
    double getConnectionWeight(const string &user1_id, const string &user2_id) const;

private:
    unordered_map<string, vector<string>> connections;
    AdjacencyMatrix *adjacency_matrix; // Add this line
    const int InitialNum = 100;
};

#endif // CONNECTION_MANAGER_H