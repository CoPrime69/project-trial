#include "connection_manager.h"
#include "adjacency_matrix.h"
#include "login_manager.h"
#include "graph_visualizer.h"
#include "connection_handler.h"
#include "print_utilities.h"
#include <bits/stdc++.h>

int main()
{
    std::string user_data_file = "user_data.csv";
    ConnectionManager cm(user_data_file);
    LoginManager login_manager(cm);

    // Choose between login or registration
    std::cout << "1. Login\n2. Register\nChoose an option: ";
    int choice;
    std::cin >> choice;

    std::string id, password;
    std::string name, category, branch;
    double influence;

    User *user = nullptr;

    if (choice == 1)
    {
        // Login
        std::cout << "Enter User ID: ";
        std::cin >> id;
        std::cout << "Enter Password: ";
        std::cin >> password;

        user = login_manager.login(id, password);
        if (!user)
        {
            std::cout << "Invalid ID or password!" << std::endl;
            return 0;
        }
    }
    else if (choice == 2)
    {
        // Register
        std::cout << "Enter Name: ";
        std::cin >> name;
        std::cout << "Enter User ID: ";
        std::cin >> id;
        std::cout << "Enter Password: ";
        std::cin >> password;

        if (login_manager.isUserExist(id))
        {
            std::cout << "User ID already exists. Please login." << std::endl;
            return 0;
        }

        std::cout << "Enter Category: ";
        std::cin >> category;
        std::cout << "Enter Influence: ";
        std::cin >> influence;
        std::cout << "Enter Branch: ";
        std::cin >> branch;

        if (!login_manager.registerUser(name, id, password, category, influence, branch))
        {
            std::cout << "Registration failed!" << std::endl;
            return 0;
        }
        user = login_manager.getUserById(id);
    }
    // else if(choice == 3){
    //     std::vector<std::vector<User *>> communities = cm.detectCommunities();
    //     GraphVisualizer visualizer(15.0);
    //     visualizer.createGraph("adjacency_matrix.csv", communities, "social_network_graph.png");
    // }
    else
    {
        std::cout << "Invalid choice!" << std::endl;
        return 0;
    }

    // Calculate centrality measures
    std::unordered_map<std::string, double> betweenness = cm.calculateBetweennessCentrality();
    std::unordered_map<std::string, double> closeness = cm.calculateClosenessCentrality();
    std::unordered_map<std::string, double> pagerank = cm.calculatePageRank();

    // Detect communities
    std::vector<std::vector<User *>> communities = cm.detectCommunities();

    // Print the results using PrintUtilities
    PrintUtilities::printResults(cm, pagerank, communities);

    // Handle connection management using ConnectionHandler
    ConnectionHandler::handleConnectionManagement(cm, user);

    // Visualize the updated graph
    cm.visualizeGraph("updated_social_network_graph.png", betweenness, communities);

    // GraphVisualizer visualizer(15.0);
    // visualizer.createGraph("adjacency_matrix.csv", communities, "social_network_graph.png");

    std::cout << "Program completed successfully!" << std::endl;
    return 0;
}