#ifndef ADJACENCY_MATRIX_H
#define ADJACENCY_MATRIX_H

#include <bits/stdc++.h>
#include "user.h"

using namespace std;

class AdjacencyMatrix
{
public:
    AdjacencyMatrix(const  string &matrix_file);
    ~AdjacencyMatrix();

    // Initialize matrix for first 100 users
    void initializeBaseMatrix(const  vector<User *> &users, const  vector< vector<User *>> &communities);

    // Add new user to matrix (with zero connections)
    void addNewUser(const  string &userId);

    // Update connection weight between two users
    void updateConnection(const  string &user1_id, const  string &user2_id, const  vector< vector<User *>> &communities);

    // Calculate connection weight based on user similarities
    double calculateConnectionWeight(User *user1, User *user2, const  vector< vector<User *>> &communities);

    void removeConnection(const  string &user1_id, const  string &user2_id);

    // Save matrix to CSV file
    void saveToFile();

    // Load matrix from CSV file
    void loadFromFile();

    // Get connection weight between two users
    double getConnectionWeight(const  string &user1_id, const  string &user2_id) const;

    const  unordered_map< string, int> &getUserIndexMap() const
    {
        return user_index_map;
    }

    const  vector< vector<double>> &getMatrix() const
    {
        return matrix;
    }

    // Print matrix (for debugging)
    void printMatrix() const;
    void forceSave();

private:
     string matrix_file_path;
     vector< string> user_ids;                   // To maintain order of users
     unordered_map< string, int> user_index_map; // Maps user IDs to matrix indices
     vector< vector<double>> matrix;             // The adjacency matrix
    int findCommunityIndex(const  string &user_id, const  vector< vector<User *>> &communities) const;

    bool areInSameCommunity(const  string &user1_id, const  string &user2_id,
                            const  vector< vector<User *>> &communities) const;
    bool hasChanges = false;
};

#endif // ADJACENCY_MATRIX_H