#include "connection_manager.h"
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <random>
#include <queue>
#include <limits>
#include <omp.h>
using namespace std;

ConnectionManager::ConnectionManager(const string &user_data_file)
{
    loadUserData(user_data_file);
    establishConnections();
}

void ConnectionManager::addConnection(User *user1, User *user2)
{
    connections[user1->getID()].push_back(user2->getID());
    connections[user2->getID()].push_back(user1->getID());
}

void ConnectionManager::addUser(User *new_user)
{
    // users.push_back(new_user);
    connections[new_user->getID()] = {};
    for (User *user : users)
    {
        if (user != new_user && user->getCategory() == new_user->getCategory() && user->getBranch() == new_user->getBranch())
        {
            if (connections[new_user->getID()].size() < 5 && connections[user->getID()].size() < 5)
            {
                connections[new_user->getID()].push_back(user->getID());
                connections[user->getID()].push_back(new_user->getID());
            }
        }
    }
}

unordered_map<string, double> ConnectionManager::calculateBetweennessCentrality()
{
    unordered_map<string, double> betweenness;
    unordered_map<string, unordered_map<string, double>> shortest_paths;

    for (const auto &user : users)
    {
        queue<string> queue;
        unordered_map<string, double> distance;
        unordered_map<string, vector<string>> predecessors;
        unordered_map<string, double> dependency;

        for (const auto &u : users)
        {
            distance[u->getID()] = numeric_limits<double>::max();
            dependency[u->getID()] = 0.0;
        }

        distance[user->getID()] = 0.0;
        queue.push(user->getID());

        while (!queue.empty())
        {
            string curr = queue.front();
            queue.pop();

            for (const auto &neighbor_id : connections[curr])
            {
                User *neighbor = getUser(neighbor_id);
                if (distance[neighbor->getID()] == numeric_limits<double>::max())
                {
                    distance[neighbor->getID()] = distance[curr] + 1;
                    queue.push(neighbor->getID());
                    predecessors[neighbor->getID()].push_back(curr);
                    if (distance[neighbor->getID()] == distance[curr] + 1)
                    {
                        shortest_paths[user->getID()][neighbor->getID()] += 1.0;
                    }
                }
            }
        }

        stack<string> stack;
        for (const auto &u : users)
        {
            if (u->getID() != user->getID())
            {
                dependency[u->getID()] = 0.0;
                if (distance[u->getID()] != numeric_limits<double>::max())
                {
                    stack.push(u->getID());
                }
            }
        }

        while (!stack.empty())
        {
            string w = stack.top();
            stack.pop();
            for (const auto &v : predecessors[w])
            {
                dependency[v] += (1.0 + dependency[w]) * shortest_paths[user->getID()][w] / shortest_paths[user->getID()][v];
            }
            if (w != user->getID())
            {
                betweenness[w] += dependency[w];
            }
        }
    }

    return betweenness;
}

unordered_map<string, double> ConnectionManager::calculateClosenessCentrality()
{
    unordered_map<string, double> closeness;
    unordered_map<string, double> total_distance;

    for (const auto &user : users)
    {
        queue<string> queue;
        unordered_map<string, double> distance;

        for (const auto &u : users)
        {
            distance[u->getID()] = numeric_limits<double>::max();
        }

        distance[user->getID()] = 0.0;
        queue.push(user->getID());

        while (!queue.empty())
        {
            string curr = queue.front();
            queue.pop();

            for (const auto &neighbor_id : connections[curr])
            {
                User *neighbor = getUser(neighbor_id);
                if (distance[neighbor->getID()] == numeric_limits<double>::max())
                {
                    distance[neighbor->getID()] = distance[curr] + 1;
                    queue.push(neighbor->getID());
                }
            }
        }

        double total_dist = 0.0;
        for (const auto &u : users)
        {
            total_dist += distance[u->getID()];
        }
        closeness[user->getID()] = users.size() / total_dist;
        total_distance[user->getID()] = total_dist;
    }

    return closeness;
}

// unordered_map<string, double> ConnectionManager::calculatePageRank()
// {
//     unordered_map<string, double> pagerank;
//     unordered_map<string, double> out_degree;

//     for (const auto &user : users)
//     {
//         pagerank[user->getID()] = 1.0 / users.size();
//         out_degree[user->getID()] = connections[user->getID()].size();
//     }

//     double damping_factor = 0.85;
//     double tolerance = 1e-6;

//     bool converged = false;
//     while (!converged)
//     {
//         unordered_map<string, double> new_pagerank;
//         double total_pagerank = 0.0;

//         for (const auto &user : users)
//         {
//             double sum = 0.0;
//             for (const auto &neighbor_id : connections[user->getID()])
//             {
//                 sum += pagerank[neighbor_id] / out_degree[neighbor_id];
//             }
//             new_pagerank[user->getID()] = (1.0 - damping_factor) / users.size() + damping_factor * sum;
//             total_pagerank += new_pagerank[user->getID()];
//         }

//         converged = true;
//         for (const auto &user : users)
//         {
//             new_pagerank[user->getID()] /= total_pagerank;
//             if (abs(new_pagerank[user->getID()] - pagerank[user->getID()]) > tolerance)
//             {
//                 converged = false;
//                 break;
//             }
//         }

//         pagerank = move(new_pagerank);
//     }

//     return pagerank;
// }

unordered_map<string, double> ConnectionManager::calculatePageRank()
{
    unordered_map<string, double> pagerank;
    unordered_map<string, double> out_degree;
    vector<string> dangling_nodes; // Stores nodes with no outgoing edges

    // Initialize pagerank and calculate out-degrees
    for (const auto &user : users)
    {
        string user_id = user->getID();
        pagerank[user_id] = 1.0 / users.size();
        out_degree[user_id] = connections[user_id].size();

        if (out_degree[user_id] == 0)
        {
            dangling_nodes.push_back(user_id); // Mark dangling nodes
        }
    }

    double damping_factor = 0.85;
    double tolerance = 1e-4;      // Start with a larger tolerance for faster initial iterations
    double min_tolerance = 1e-6;  // Minimum tolerance to tighten as convergence improves
    double tolerance_decay = 0.9; // Factor to reduce tolerance per iteration
    bool converged = false;

    while (!converged)
    {
        unordered_map<string, double> new_pagerank;
        double total_pagerank = 0.0;
        double dangling_sum = 0.0; // Contribution from dangling nodes

        // Sum contributions from dangling nodes
        for (const auto &dangling_id : dangling_nodes)
        {
            dangling_sum += pagerank[dangling_id];
        }
        dangling_sum *= damping_factor / users.size();

// Parallelize the computation of new pagerank values
#pragma omp parallel for reduction(+ : total_pagerank)
        for (size_t i = 0; i < users.size(); ++i)
        {
            User *user = users[i];
            string user_id = user->getID();
            double sum = 0.0;

            // Sum up the contributions from neighbors
            for (const auto &neighbor_id : connections[user_id])
            {
                sum += pagerank[neighbor_id] / out_degree[neighbor_id];
            }

            // Calculate new PageRank for the current user, including dangling node contribution
            new_pagerank[user_id] = (1.0 - damping_factor) / users.size() + damping_factor * sum + dangling_sum;
            total_pagerank += new_pagerank[user_id];
        }

        // Normalize new PageRank values and check for convergence
        converged = true;
#pragma omp parallel for
        for (size_t i = 0; i < users.size(); ++i)
        {
            User *user = users[i];
            string user_id = user->getID();

            new_pagerank[user_id] /= total_pagerank;

            // Check convergence
            if (fabs(new_pagerank[user_id] - pagerank[user_id]) > tolerance)
            {
#pragma omp atomic write
                converged = false;
            }
        }

        pagerank = move(new_pagerank);

        // Tighten the tolerance for the next iteration if not yet at min_tolerance
        tolerance = max(min_tolerance, tolerance * tolerance_decay);
    }

    return pagerank;
}

vector<vector<User *>> ConnectionManager::detectCommunities()
{
    vector<vector<User *>> communities;
    unordered_map<string, int> community_map;
    int num_communities = 0;

    for (User *user : users)
    {
        bool found_community = false;
        for (int i = 0; i < communities.size() && i < 20; ++i)
        {
            bool can_join = true;
            for (User *neighbor : communities[i])
            {
                if (find(connections[user->getID()].begin(), connections[user->getID()].end(), neighbor->getID()) == connections[user->getID()].end())
                {
                    can_join = false;
                    break;
                }
            }
            if (can_join)
            {
                communities[i].push_back(user);
                community_map[user->getID()] = i;
                found_community = true;
                break;
            }
        }
        if (!found_community)
        {
            if (num_communities < 20)
            {
                communities.emplace_back(vector<User *>{user});
                community_map[user->getID()] = num_communities++;
            }
            else
            {
                // Assign user to the community with the least members
                int min_community = 0;
                for (int i = 1; i < 20; ++i)
                {
                    if (communities[i].size() < communities[min_community].size())
                    {
                        min_community = i;
                    }
                }
                communities[min_community].push_back(user);
                community_map[user->getID()] = min_community;
            }
        }
    }

    for (int i = 0; i < communities.size(); ++i)
    {
        string community_name = "Community " + to_string(i + 1);
        for (User *user : communities[i])
        {
            if (user->getBranch() == "electrical")
            {
                community_name = "Electrical Engineers";
                break;
            }
        }
        for (User *user : communities[i])
        {
            community_map[user->getID()] = i;
        }
    }

    return communities;
}

vector<User *> ConnectionManager::getAllUsers()
{
    return users; // Assuming 'users' is a vector that stores all User pointers
}

vector<pair<User *, User *>> ConnectionManager::recommendConnectionsForNewUser(User *new_user)
{
    vector<pair<User *, User *>> recommendations;
    unordered_map<string, int> community_map;
    vector<vector<User *>> communities = detectCommunities();

    for (int i = 0; i < communities.size(); ++i)
    {
        for (User *user : communities[i])
        {
            community_map[user->getID()] = i;
        }
    }

    vector<pair<double, pair<User *, User *>>> sortedRecommendations;
    for (User *user : users)
    {
        if (user != new_user && community_map[user->getID()] != community_map[new_user->getID()])
        {
            double score = 0.0;
            if (user->getCategory() == new_user->getCategory())
            {
                score += 0.5;
            }
            if (user->getBranch() == new_user->getBranch())
            {
                score += 0.5;
            }
            sortedRecommendations.emplace_back(-score, make_pair(new_user, user));
        }
    }

    sort(sortedRecommendations.begin(), sortedRecommendations.end());
    for (const auto &pair : sortedRecommendations)
    {
        recommendations.emplace_back(pair.second);
    }

    return recommendations;
}

void ConnectionManager::visualizeGraph(const string &output_file, const unordered_map<string, double> &betweenness, const vector<vector<User *>> &communities)
{
}

int ConnectionManager::getRandomPosition(int max)
{
}

double ConnectionManager::getLuminance(const string &color)
{
    
}

User *ConnectionManager::getUser(const string &id) const
{
    for (User *user : users)
    {
        if (user->getID() == id)
        {
            return user;
        }
    }
    return nullptr;
}

void ConnectionManager::loadUserData(const string &file_path)
{
    ifstream file(file_path);
    string line;
    getline(file, line); // Skip header row

    while (getline(file, line))
    {
        stringstream ss(line);
        string name, id, password, category, branch;
        double influence;
        getline(ss, name, ',');
        getline(ss, id, ',');
        getline(ss, password, ',');
        getline(ss, category, ',');
        ss >> influence;
        ss.ignore();
        getline(ss, branch, ',');

        users.push_back(new User(name, id, password, category, influence, branch));
    }
}

void ConnectionManager::establishConnections()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    for (User *user1 : users)
    {
        for (User *user2 : users)
        {
            if (user1 != user2 && user1->getCategory() == user2->getCategory() && user1->getBranch() == user2->getBranch())
            {
                if (dis(gen) < 0.5)
                {
                    connections[user1->getID()].push_back(user2->getID());
                    connections[user2->getID()].push_back(user1->getID());
                }
            }
        }
    }

    for (User *user1 : users)
    {
        for (User *user2 : users)
        {
            if (user1 != user2 && (user1->getCategory() != user2->getCategory() || user1->getBranch() != user2->getBranch()))
            {
                if (dis(gen) < 0.2)
                {
                    connections[user1->getID()].push_back(user2->getID());
                    connections[user2->getID()].push_back(user1->getID());
                }
            }
        }
    }
}

void ConnectionManager::saveUserData(const string &file_path)
{
    ofstream file(file_path);
    file << "name,id,password,category,influence,branch\n";
    for (User *user : users)
    {
        file << user->getName() << "," << user->getID() << "," << user->getPassword() << "," << user->getCategory() << "," << user->getInfluence() << "," << user->getBranch() << "\n";
    }
    file.close();
}