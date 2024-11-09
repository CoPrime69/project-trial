#include "connection_manager.h"
#include "adjacency_matrix.h"
#include "login_manager.h"
#include <bits/stdc++.h>
#include <algorithm>
#include "graph_visualizer.h"

using namespace std;

void printResults(ConnectionManager& cm, const unordered_map<string, double>& pagerank, const vector<vector<User*>>& communities);
void printRecommendedCommunities(const vector<vector<User*>>& communities, User* new_user);
void printRecommendedConnections(const vector<pair<User*, User*>>& recommendations, User* new_user);
int selectConnection(const vector<pair<User*, User*>>& recommendations);
void addNewUser(ConnectionManager& cm, const string& name, const string& id, const string& password, const string& category, double influence, const string& branch);

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
            return 0; // Login failed
        }
    } else if (choice == 2) {
        // Register
        cout << "Enter Name: ";
        cin >> name;
        cout << "Enter User ID: ";
        cin >> id;
        cout << "Enter Password: ";
        cin >> password;

        // Check if the user ID already exists
        if (login_manager.isUserExist(id)) {
            cout << "User ID already exists. Please login." << endl;
            return 0; // If ID exists, exit
        }

        cout << "Enter Category: ";
        cin >> category;
        cout << "Enter Influence: ";
        cin >> influence;
        cout << "Enter Branch: ";
        cin >> branch;

        if (!login_manager.registerUser(name, id, password, category, influence, branch)) {
            cout << "Registration failed!" << endl;
            return 0;  // Registration failed, exit
        }
        user = login_manager.getUserById(id); // Retrieve the registered user
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

    // Ask if the user wants to join a community by making a connection
    char join_community_choice;
    cout << "\nDo you want to make a connection and join a community? (y/n): ";
    cin >> join_community_choice;

    if (join_community_choice == 'y' || join_community_choice == 'Y') {
        // Recommend connections for the user
        vector<pair<User*, User*>> recommendations = cm.recommendConnectionsForNewUser(user);
        // cout << "\nRecommended connections for you:" << endl;
        printRecommendedConnections(recommendations, user);

        // Allow the user to select a connection to add
        int selected_connection = selectConnection(recommendations);
        if (selected_connection >= 0) {
            // Add the connection
            User* selected_user = recommendations[selected_connection].second;
            cm.addConnection(user, selected_user);

            // Visualize the updated graph
            cm.visualizeGraph("updated_social_network_graph.png", betweenness, communities);

            cout << "You have successfully joined the community!" << endl;
        }
    } else {
        cout << "You will not be added to any community." << endl;
    }
    GraphVisualizer visualizer(15.0); // Create visualizer with threshold of 15.0
    visualizer.createGraph("adjacency_matrix.csv", communities, "social_network_graph.png");

    cout << "Okie! End of Code" << endl;

    return 0;
}

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
    int counter = 0;
    for (const auto& pair : recommendations) {
        if (pair.first->getCategory() == new_user->getCategory() && pair.first->getBranch() == new_user->getBranch()) {
            cout << pair.first->getName() << " <-> " << pair.second->getName() << endl;
            counter++;
        }
        if(counter >= 5) {
            break;
        }
    }
}

int selectConnection(const vector<pair<User*, User*>>& recommendations) {
    cout << "\nSelect a connection to add (enter index): ";
    int index;
    cin >> index;
    if (index >= 0 && index < recommendations.size()) {
        return index;
    }
    return -1;
}

void addNewUser(ConnectionManager& cm, const string& name, const string& id, const string& password, const string& category, double influence, const string& branch) {
    string new_id = id; // Ensure new user ID is unique
    string new_password = password;
    cm.addUser(new User(name, new_id, new_password, category, influence, branch));
    cout << "New user added!" << endl;
}
