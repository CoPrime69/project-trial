#include "adjacency_matrix.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>

AdjacencyMatrix::AdjacencyMatrix(const std::string& matrix_file) : matrix_file_path(matrix_file) {
    loadFromFile();
}

AdjacencyMatrix::~AdjacencyMatrix() {
    saveToFile();
}

static std::mt19937& getRNG() {
    static std::mt19937 rng(std::time(nullptr));
    return rng;
}
    
    // Helper function to get a random weight from normal distribution
static double getRandomWeight(double mean, double stddev = 2.5) {
    std::normal_distribution<double> distribution(mean, stddev);
    return distribution(getRNG());
}    

void AdjacencyMatrix::initializeBaseMatrix(const std::vector<User*>& users, 
                                         const std::vector<std::vector<User*>>& communities) {
    // Only initialize if matrix is empty (first time)
    if (!matrix.empty()) return;

    // Process first 100 users or all users if less than 100
    size_t num_users = std::min(size_t(100), users.size());
    
    // Initialize matrix size
    matrix.resize(num_users, std::vector<double>(num_users, 0.0));
    user_ids.clear();
    user_index_map.clear();

    // Store user IDs and create index mapping
    for (size_t i = 0; i < num_users; ++i) {
        user_ids.push_back(users[i]->getID());
        user_index_map[users[i]->getID()] = i;
    }

    // Calculate weights for all pairs
    for (size_t i = 0; i < num_users; ++i) {
        for (size_t j = i + 1; j < num_users; ++j) {
            double weight = calculateConnectionWeight(users[i], users[j], communities);
            matrix[i][j] = weight;
            matrix[j][i] = weight;  // Matrix is symmetric
        }
    }

    saveToFile();
}

void AdjacencyMatrix::addNewUser(const std::string& userId) {
    // Check if user already exists
    if (user_index_map.find(userId) != user_index_map.end()) {
        return;
    }

    // Add new row and column to matrix
    size_t new_index = matrix.size();
    user_index_map[userId] = new_index;
    user_ids.push_back(userId);

    // Add new row
    matrix.push_back(std::vector<double>(new_index, 0.0));
    
    // Add new column to all existing rows
    for (auto& row : matrix) {
        row.push_back(0.0);
    }

    saveToFile();
}

void AdjacencyMatrix::updateConnection(const std::string& user1_id, const std::string& user2_id,
                                     const std::vector<std::vector<User*>>& communities) {
    // Get matrix indices
    auto it1 = user_index_map.find(user1_id);
    auto it2 = user_index_map.find(user2_id);

    if (it1 == user_index_map.end() || it2 == user_index_map.end()) {
        return;
    } 

    int idx1 = it1->second;
    int idx2 = it2->second;

    // Calculate similarity score
    double weight = getRandomWeight(13.5);  // Start with minimum weight

    // Base weight if in same community
    if (areInSameCommunity(user1_id, user2_id, communities)) {
        weight += getRandomWeight(7.0);  // Increase by 7 for same community
    }

    // Additional weight based on community structure
    int community1 = findCommunityIndex(user1_id, communities);
    int community2 = findCommunityIndex(user2_id, communities);

    if (community1 != -1 && community2 != -1) {
        if (community1 == community2) {
            weight += getRandomWeight(5.0);  // Additional weight for same community
        } else {
            // Calculate inter-community similarity
            // You could add more sophisticated similarity metrics here
            weight += rand() % 4;  // Small boost for different but connected communities
        }
    }

    // Ensure weight is between 13 and 20
    weight = std::min(20.0, std::max(13.0, getRandomWeight(weight)));

    // Update matrix (symmetric)
    matrix[idx1][idx2] = weight;
    matrix[idx2][idx1] = weight;

    saveToFile();
}

double AdjacencyMatrix::calculateConnectionWeight(User* user1, User* user2,
                                                const std::vector<std::vector<User*>>& communities) {
    double weight = 0.0;

    // Base similarity checks
    if (user1->getCategory() == user2->getCategory()) weight += getRandomWeight(4.0);
    if (user1->getBranch() == user2->getBranch()) weight += getRandomWeight(6.0);

    // Community based weight
    if (areInSameCommunity(user1->getID(), user2->getID(), communities)) {
        weight += getRandomWeight(10.0);
    }

    // Influence similarity (normalized)
    double influence_diff = std::abs(user1->getInfluence() - user2->getInfluence());
    weight += std::max(0.0, 5.0 * (1.0 - influence_diff / 100.0));

    // Ensure weight is between 0 and 25
    return std::min(25.0, std::max(0.0, getRandomWeight(weight)));
}

void AdjacencyMatrix::saveToFile() {
    std::ofstream file(matrix_file_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open matrix file for writing." << std::endl;
        return;
    }

    // Write header (user IDs)
    file << ",";  // Empty cell for row headers
    for (const auto& id : user_ids) {
        file << id << ",";
    }
    file << "\n";

    // Write matrix rows
    for (size_t i = 0; i < matrix.size(); ++i) {
        file << user_ids[i] << ",";  // Row header
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            file << matrix[i][j];
            if (j < matrix[i].size() - 1) file << ",";
        }
        file << "\n";
    }

    file.close();
}

void AdjacencyMatrix::loadFromFile() {
    std::ifstream file(matrix_file_path);
    if (!file.is_open()) {
        std::cout << "No existing matrix file found. Will create new matrix." << std::endl;
        return;
    }

    matrix.clear();
    user_ids.clear();
    user_index_map.clear();

    std::string line;
    
    // Read header
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::getline(ss, cell, ',');  // Skip empty cell
        
        int idx = 0;
        while (std::getline(ss, cell, ',')) {
            if (!cell.empty()) {
                user_ids.push_back(cell);
                user_index_map[cell] = idx++;
            }
        }
    }

    // Read matrix data
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::getline(ss, cell, ',');  // Skip row header
        
        std::vector<double> row;
        while (std::getline(ss, cell, ',')) {
            if (!cell.empty()) {
                row.push_back(std::stod(cell));
            }
        }
        if (!row.empty()) {
            matrix.push_back(row);
        }
    }

    file.close();
}

double AdjacencyMatrix::getConnectionWeight(const std::string& user1_id, const std::string& user2_id) const {
    auto it1 = user_index_map.find(user1_id);
    auto it2 = user_index_map.find(user2_id);
    
    if (it1 == user_index_map.end() || it2 == user_index_map.end()) {
        return 0.0;
    }

    return matrix[it1->second][it2->second];
}

void AdjacencyMatrix::printMatrix() const {
    // Print header
    std::cout << "\t";
    for (const auto& id : user_ids) {
        std::cout << id << "\t";
    }
    std::cout << "\n";

    // Print matrix rows
    for (size_t i = 0; i < matrix.size(); ++i) {
        std::cout << user_ids[i] << "\t";
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            std::cout << matrix[i][j] << "\t";
        }
        std::cout << "\n";
    }
}

int AdjacencyMatrix::findCommunityIndex(const std::string& user_id,
                                      const std::vector<std::vector<User*>>& communities) const {
    for (size_t i = 0; i < communities.size(); ++i) {
        for (const auto& user : communities[i]) {
            if (user->getID() == user_id) {
                return i;
            }
        }
    }
    return -1;
}

bool AdjacencyMatrix::areInSameCommunity(const std::string& user1_id, const std::string& user2_id,
                                        const std::vector<std::vector<User*>>& communities) const {
    int community1 = findCommunityIndex(user1_id, communities);
    int community2 = findCommunityIndex(user2_id, communities);
    return (community1 != -1 && community1 == community2);
}