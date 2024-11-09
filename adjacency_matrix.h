#ifndef ADJACENCY_MATRIX_H
#define ADJACENCY_MATRIX_H

#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <random>
#include "user.h"

class AdjacencyMatrix {
public:
    AdjacencyMatrix(const std::string& matrix_file);
    ~AdjacencyMatrix();


    // Initialize matrix for first 100 users
    void initializeBaseMatrix(const std::vector<User*>& users, const std::vector<std::vector<User*>>& communities);
    
    // Add new user to matrix (with zero connections)
    void addNewUser(const std::string& userId);
    
    // Update connection weight between two users
    void updateConnection(const std::string& user1_id, const std::string& user2_id, const std::vector<std::vector<User*>>& communities);
    
    // Calculate connection weight based on user similarities
    double calculateConnectionWeight(User* user1, User* user2, const std::vector<std::vector<User*>>& communities);
    
    // Save matrix to CSV file
    void saveToFile();
    
    // Load matrix from CSV file
    void loadFromFile();
    
    // Get connection weight between two users
    double getConnectionWeight(const std::string& user1_id, const std::string& user2_id) const;
    
    // Print matrix (for debugging)
    void printMatrix() const;

private:
    std::string matrix_file_path;
    std::vector<std::string> user_ids;  // To maintain order of users
    std::unordered_map<std::string, int> user_index_map;  // Maps user IDs to matrix indices
    std::vector<std::vector<double>> matrix;  // The adjacency matrix
    int findCommunityIndex(const std::string& user_id, const std::vector<std::vector<User*>>& communities) const;
    
    bool areInSameCommunity(const std::string& user1_id, const std::string& user2_id, 
                           const std::vector<std::vector<User*>>& communities) const;

    
};

#endif // ADJACENCY_MATRIX_H