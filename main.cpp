#include "connection_manager.h"
#include <bits/stdc++.h>
#include <algorithm>
using namespace std;

void printResults(ConnectionManager& cm, const  unordered_map< string, double>& pagerank, const  vector< vector<User*>>& communities);
void printRecommendedCommunities(const  vector< vector<User*>>& communities, User* new_user);
void printRecommendedConnections(const  vector< pair<User*, User*>>& recommendations, User* new_user);
int selectConnection(const  vector< pair<User*, User*>>& recommendations);
void addNewUser(ConnectionManager& cm, const string& name, const string& id, const string& password, const string& category, double influence, const string& branch);

int main() {
    string user_data_file = "user_data.csv";
    ConnectionManager cm(user_data_file);

    // Calculate centrality measures
    unordered_map< string, double> betweenness = cm.calculateBetweennessCentrality();
    unordered_map< string, double> closeness = cm.calculateClosenessCentrality();
    unordered_map< string, double> pagerank = cm.calculatePageRank();

    // Detect communities
    vector< vector<User*>> communities = cm.detectCommunities();

    // Print the results
    printResults(cm, pagerank, communities);

    // Add a new user
    string new_user_name = "Prakhar";
    string new_user_id = "user123";
    string new_user_password = "password123";
    string new_user_category = "Engineering";
    double new_user_influence = 75;
    string new_user_branch = "Electrical";
    addNewUser(cm, new_user_name, new_user_id, new_user_password, new_user_category, new_user_influence, new_user_branch);

    // Recommend communities for the new user
    cout << "\nRecommended communities for the new user:" << endl;
    printRecommendedCommunities(communities, cm.getUser(new_user_id));

    // Recommend connections for the new user
    vector< pair<User*, User*>> recommendations = cm.recommendConnectionsForNewUser(cm.getUser(new_user_id));
    cout << "\nRecommended connections for the new user:" << endl;
    printRecommendedConnections(recommendations, cm.getUser(new_user_id));

    // Allow the user to select a connection to add
    int selected_connection = selectConnection(recommendations);

    if (selected_connection >= 0) {
        // Add the connection
        User* selected_user = recommendations[selected_connection].second;
        cm.addConnection(cm.getUser(new_user_id), selected_user);

        // Visualize the updated graph
        cm.visualizeGraph("updated_social_network_graph.png", betweenness, communities);
    }
    cout << "\nUpdated Community Structure:" << endl;
    printResults(cm, pagerank, communities);

    return 0;
}

void printResults(ConnectionManager& cm, const  unordered_map< string, double>& pagerank, const  vector< vector<User*>>& communities) {
    cout << "Top 5 Influencers:" << endl;
    vector< pair< string, double>> top_influencers;
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

void printRecommendedCommunities(const  vector< vector<User*>>& communities, User* new_user) {
    int matchingCommunityCount = 0;
    for (const auto& community : communities) {
        // We stop once we've found 3 matching communities
        if (matchingCommunityCount >= 3) break;

        bool is_match = false;
        // Search for at least one user in the community matching the new user’s category and branch
        for (User* user : community) {
            if (user->getCategory() == new_user->getCategory() && user->getBranch() == new_user->getBranch()) {
                is_match = true;
                break;
            }
        }

        // If a match is found, print the community and stop after recommending up to 3
        if (is_match) {
            cout << "Recommended Community " << matchingCommunityCount + 1 << ": ";
            for (size_t i = 0; i < community.size(); ++i) {
                cout << community[i]->getName();
                if (i != community.size() - 1) cout << ", ";  // Avoid trailing commas
            }
            cout << endl;
            matchingCommunityCount++;
        }
    }

    if (matchingCommunityCount == 0) {
        cout << "No matching communities found." << endl;
    }
}

void printRecommendedConnections(const  vector< pair<User*, User*>>& recommendations, User* new_user) {
    for (size_t i = 0; i < min<size_t>(recommendations.size(), 5); ++i) {
        cout << "Recommend connection between " << new_user->getName() << " and " << recommendations[i].second->getName() << endl;
    }
}

int selectConnection(const  vector< pair<User*, User*>>& recommendations) {
    cout << "Select a connection to add (-1 to stop): ";
    int selection;
    cin >> selection;
    if (selection < -1 || selection >= recommendations.size()) {
        cout << "Invalid selection. Stopping." << endl;
        return -1;
    }
    return selection;
}

void addNewUser(ConnectionManager& cm, const string& name, const string& id, const string& password, const string& category, double influence, const string& branch) {
    User* new_user = new User(name, id, password, category, influence, branch);
    cm.users.push_back(new_user);
    cm.addUser(new_user);
    cm.saveUserData("user_data.csv");
}