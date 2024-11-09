#include "connection_manager.h"
#include "adjacency_matrix.h"
#include "login_manager.h"
#include <bits/stdc++.h>
#include <algorithm>
#include "graph_visualizer.h"
vector<int> g_validIndices;
int g_currentStart = 0;

using namespace std;

void printResults(ConnectionManager& cm, const unordered_map<string, double>& pagerank, const vector<vector<User*>>& communities);
void printRecommendedCommunities(const vector<vector<User*>>& communities, User* new_user);
void printRecommendedConnections(const vector<pair<User*, User*>>& recommendations, User* new_user);
int selectConnection(const vector<pair<User*, User*>>& recommendations);
void addNewUser(ConnectionManager& cm, const string& name, const string& id, const string& password, const string& category, double influence, const string& branch);
void handleConnectionManagement(ConnectionManager& cm, User* user);

int main() {
    string user_data_file = "user_data.csv";
    ConnectionManager cm(user_data_file);
    LoginManager login_manager(cm);

    // Choose between login or registration
    cout << "1. Login\n2. Register\nChoose an option: ";
    int choice;
    cin >> choice;

    string id, password;
    string name, category, branch;
    double influence;

    User* user = nullptr;

    if (choice == 1) {
        // Login
        cout << "Enter User ID: ";
        cin >> id;
        cout << "Enter Password: ";
        cin >> password;

        user = login_manager.login(id, password);
        if (!user) {
            cout << "Invalid ID or password!" << endl;
            return 0;
        }
    } else if (choice == 2) {
        // Register
        cout << "Enter Name: ";
        cin >> name;
        cout << "Enter User ID: ";
        cin >> id;
        cout << "Enter Password: ";
        cin >> password;

        if (login_manager.isUserExist(id)) {
            cout << "User ID already exists. Please login." << endl;
            return 0;
        }

        cout << "Enter Category: ";
        cin >> category;
        cout << "Enter Influence: ";
        cin >> influence;
        cout << "Enter Branch: ";
        cin >> branch;

        if (!login_manager.registerUser(name, id, password, category, influence, branch)) {
            cout << "Registration failed!" << endl;
            return 0;
        }
        user = login_manager.getUserById(id);
    } else {
        cout << "Invalid choice!" << endl;
        return 0;
    }

    // Calculate centrality measures
    unordered_map<string, double> betweenness = cm.calculateBetweennessCentrality();
    unordered_map<string, double> closeness = cm.calculateClosenessCentrality();
    unordered_map<string, double> pagerank = cm.calculatePageRank();

    // Detect communities
    vector<vector<User*>> communities = cm.detectCommunities();

    // Print the results
    printResults(cm, pagerank, communities);

    // Handle connection management
    handleConnectionManagement(cm, user);

    // Visualize the updated graph
    cm.visualizeGraph("updated_social_network_graph.png", betweenness, communities);

    GraphVisualizer visualizer(15.0);
    visualizer.createGraph("adjacency_matrix.csv", communities, "social_network_graph.png");

    cout << "Program completed successfully!" << endl;
    return 0;
}

void handleConnectionManagement(ConnectionManager& cm, User* user) {
    while (true) {
        cout << "\nConnection Management Menu:" << endl;
        cout << "1. View your connections" << endl;
        cout << "2. Add new connection" << endl;
        cout << "3. Remove connection" << endl;
        cout << "4. Exit" << endl;
        cout << "Choose an option: ";
        
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                // View connections
                cm.viewConnections(user);
                break;
            }
            case 2: {
                // Add new connection
                do {
                    vector<pair<User*, User*>> recommendations = cm.recommendConnectionsForNewUser(user);
                    printRecommendedConnections(recommendations, user);

                    int selected_connection = selectConnection(recommendations);
                    if (selected_connection >= 0) {
                        User* selected_user = recommendations[selected_connection].second;
                        cm.addConnection(user, selected_user);
                        cout << "Successfully connected with " << selected_user->getName() << "!" << endl;
                    }

                    cout << "Add another connection? (y/n): ";
                    char more_choice;
                    cin >> more_choice;
                    if (more_choice != 'y' && more_choice != 'Y') break;
                } while (true);
                break;
            }
            case 3: {
                // Remove connection
                cout << "\nYour current connections:" << endl;
                cm.viewConnections(user);
                
                cout << "\nEnter the ID of the connection you want to remove: ";
                string remove_id;
                cin >> remove_id;
                
                User* connection_to_remove = cm.getUser(remove_id);
                if (connection_to_remove) {
                    cm.removeConnection(user, connection_to_remove);
                    cout << "Successfully removed connection with " << connection_to_remove->getName() << endl;
                } else {
                    cout << "Invalid user ID or not in your connections." << endl;
                }
                break;
            }
            case 4:
                return;
            default:
                cout << "Invalid option. Please try again." << endl;
        }
    }
}

// [Previous helper functions remain unchanged]
void printResults(ConnectionManager& cm, const unordered_map<string, double>& pagerank, const vector<vector<User*>>& communities) {
    cout << "Top 5 Influencers:" << endl;
    vector<pair<string, double>> top_influencers;
    for (const auto& pair : pagerank) {
        top_influencers.emplace_back(pair.first, pair.second);
    }
    sort(top_influencers.begin(), top_influencers.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
    for (size_t i = 0; i < 5; ++i) {
        cout << "User: " << cm.getUser(top_influencers[i].first)->getName()
             << ", Influence: " << top_influencers[i].second << endl;
    }

    cout << "\nCommunities:" << endl;
    for (size_t i = 0; i < communities.size(); ++i) {
        cout << "Community " << i + 1 << ": ";
        for (User* user : communities[i]) {
            cout << user->getName() << ", ";
        }
        cout << endl;
    }
}

void printRecommendedCommunities(const vector<vector<User*>>& communities, User* new_user) {
    int matchingCommunityCount = 0;
    for (const auto& community : communities) {
        if (matchingCommunityCount >= 3) break;
        bool is_match = false;
        for (User* user : community) {
            if (user->getCategory() == new_user->getCategory() && user->getBranch() == new_user->getBranch()) {
                is_match = true;
                break;
            }
        }
        if (is_match) {
            matchingCommunityCount++;
            cout << "Community " << matchingCommunityCount << ": ";
            for (User* user : community) {
                cout << user->getName() << ", ";
            }
            cout << endl;
        }
    }
}

void printRecommendedConnections(const vector<pair<User*, User*>>& recommendations, User* new_user) {
    cout << "\nRecommended connections based on category and influence:" << endl;
    int validIndex = 0;
    g_validIndices.clear();
    
    int skipped = 0;
    for (int i = 0; i < recommendations.size() && validIndex < 5; i++) {
        const auto& pair = recommendations[i];
        
        if ((pair.first->getCategory() == new_user->getCategory() && pair.first->getBranch() == new_user->getBranch()) ||
            (pair.second->getCategory() == new_user->getCategory() && pair.second->getBranch() == new_user->getBranch())) {
            
            if (skipped < g_currentStart) {
                skipped++;
                continue;
            }
            
            cout << validIndex << ": " << pair.first->getName() << " <-> " << pair.second->getName() 
                 << " (Category: " << pair.second->getCategory() 
                 << ", Branch: " << pair.second->getBranch() << ")" << endl;
            
            g_validIndices.push_back(i);
            validIndex++;
        }
    }
    
    if (validIndex == 0) {
        if (g_currentStart == 0) {
            cout << "No matching recommendations found." << endl;
        } else {
            cout << "No more recommendations available." << endl;
            g_currentStart = 0;
        }
    }
}

int selectConnection(const vector<pair<User*, User*>>& recommendations) {
    if (g_validIndices.empty()) {
        return -1;
    }
    
    cout << "\nSelect a connection to add (enter number 0-" << g_validIndices.size()-1 
         << ", or -1 to skip, or any other negative number to exit): ";
    int displayIndex;
    cin >> displayIndex;
    
    if (displayIndex == -1) {
        g_currentStart += 5;
        printRecommendedConnections(recommendations, recommendations[0].first);
        return selectConnection(recommendations);
    } else if (displayIndex < -1) {
        g_currentStart = 0;
        return -1;
    } else if (displayIndex >= 0 && displayIndex < g_validIndices.size()) {
        g_currentStart = 0;
        return g_validIndices[displayIndex];
    }
    
    cout << "Invalid selection. Please try again." << endl;
    return selectConnection(recommendations);
}

void addNewUser(ConnectionManager& cm, const string& name, const string& id, const string& password, const string& category, double influence, const string& branch) {
    string new_id = id;
    string new_password = password;
    cm.addUser(new User(name, new_id, new_password, category, influence, branch));
    cout << "New user added!" << endl;
}

// void printRecommendedConnections(const vector<pair<User*, User*>>& recommendations, User* new_user) {
//     cout << "\nRecommended connections based on category and influence:" << endl;
//     int counter = 0;
//     for (const auto& pair : recommendations) {
//         if (pair.first->getCategory() == new_user->getCategory() && pair.first->getBranch() == new_user->getBranch()) {
//             cout << pair.first->getName() << " <-> " << pair.second->getName() << endl;
//             counter++;
//         }
//         if(counter >= 5) {
//             break;
//         }
//     }
// }

// int selectConnection(const vector<pair<User*, User*>>& recommendations) {
//     cout << "\nSelect a connection to add (enter index): ";
//     int index;
//     cin >> index;
//     if (index >= 0 && index < recommendations.size()) {
//         return index;
//     }
//     return -1;
// }
