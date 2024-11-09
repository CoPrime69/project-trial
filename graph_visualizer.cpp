#include "graph_visualizer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <array>
#include <iomanip>

GraphVisualizer::GraphVisualizer(double threshold) : weight_threshold(threshold) {}

std::vector<std::vector<double>> GraphVisualizer::readAdjacencyMatrix(
    const std::string& matrix_file,
    std::vector<std::string>& user_ids) {
    
    std::ifstream file(matrix_file);
    if (!file.is_open()) {
        std::cerr << "Could not open matrix file: " << matrix_file << std::endl;
        return std::vector<std::vector<double>>();
    }
    
    std::vector<std::vector<double>> matrix;
    std::string line;
    
    // Read header to get user IDs
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::getline(ss, cell, ','); // Skip empty cell
        
        while (std::getline(ss, cell, ',')) {
            if (!cell.empty()) {
                user_ids.push_back(cell);
            }
        }
    }
    
    // Read matrix data
    while (std::getline(file, line)) {
        std::vector<double> row;
        std::stringstream ss(line);
        std::string cell;
        
        std::getline(ss, cell, ','); // Skip row header
        
        while (std::getline(ss, cell, ',')) {
            if (!cell.empty()) {
                try {
                    row.push_back(std::stod(cell));
                } catch (const std::exception& e) {
                    std::cerr << "Error converting value: " << cell << std::endl;
                    row.push_back(0.0);
                }
            }
        }
        
        if (!row.empty()) {
            matrix.push_back(row);
        }
    }
    
    file.close();
    return matrix;
}

void GraphVisualizer::createGraph(const std::string& matrix_file,
                                const std::vector<std::vector<User*>>& communities,
                                const std::string& output_file) {
    std::vector<std::string> user_ids;
    std::vector<std::vector<double>> matrix = readAdjacencyMatrix(matrix_file, user_ids);
    
    if (matrix.empty()) {
        std::cerr << "Failed to read adjacency matrix" << std::endl;
        return;
    }
    
    std::string dot_file = "temp_graph.dot";
    std::ofstream dot_out(dot_file);
    dot_out << generateDotFormat(matrix, user_ids, communities);
    dot_out.close();
    
    std::string command = "dot -Kneato"
                         " -Tpng"
                         " -Gdpi=300"
                         " -Goverlap=scale"
                         " -Gsplines=true"
                         " -Gstart=random"
                         " -Gepsilon=0.0001"
                         " -Gmaxiter=1000"
                         " -Gresolution=\"1920,1080\""
                         " " + dot_file + " -o " + output_file;
    
    int result = system(command.c_str());
    
    if (result == 0) {
        std::cout << "Graph visualization created successfully: " << output_file << std::endl;
        std::remove(dot_file.c_str());
    } else {
        std::cerr << "Failed to create graph visualization" << std::endl;
    }
}

// std::string GraphVisualizer::generateDotFormat(
//     const std::vector<std::vector<double>>& matrix,
//     const std::vector<std::string>& user_ids,
//     const std::vector<std::vector<User*>>& communities) {
    
//     std::stringstream dot;
//     dot << "graph Network {\n";
    
//     dot << "    graph [\n"
//         << "        overlap=false\n"
//         << "        splines=true\n"
//         << "        K=2\n"
//         << "        layout=neato\n"
//         << "        mindist=1.0\n"
//         << "        outputorder=nodesfirst\n"
//         << "        pad=0.5\n"
//         << "    ];\n\n";
    
//     dot << "    node [\n"
//         << "        shape=circle\n"
//         << "        style=filled\n"
//         << "        width=0.6\n"
//         << "        height=0.6\n"
//         << "        penwidth=1.5\n"
//         << "        fontname=\"Arial\"\n"
//         << "        fontsize=10\n"
//         << "    ];\n\n";
    
//     dot << "    edge [\n"
//         << "        penwidth=1.5\n"
//         << "        color=\"#333333\"\n"
//         << "    ];\n\n";

//     const std::array<std::string, 10> colors = {{
//         "\"#ff7f7f\"", "\"#7fbfff\"", "\"#7fff7f\"", "\"#ff7fff\"", "\"#ffff7f\"",
//         "\"#7fffff\"", "\"#ff9966\"", "\"#66ff99\"", "\"#9966ff\"", "\"#ff9966\""
//     }};

//     for (size_t i = 0; i < communities.size(); i++) {
//         for (const User* user : communities[i]) {
//             dot << "    \"" << user->getID() << "\" ["
//                 << "fillcolor=" << colors[i % colors.size()] 
//                 << ", label=\"" << user->getName() << "\"];\n";
//         }
//     }
//     dot << "\n";

//     for (size_t i = 0; i < matrix.size(); i++) {
//         for (size_t j = i + 1; j < matrix[i].size(); j++) {
//             if (matrix[i][j] >= weight_threshold) {
//                 double len = 5.0 / matrix[i][j];
//                 dot << "    \"" << user_ids[i] << "\" -- \"" << user_ids[j] 
//                     << "\" [len=" << len << "];\n";
//             }
//         }
//     }
    
//     dot << "}\n";
//     return dot.str();
// }

std::string GraphVisualizer::generateDotFormat(
    const std::vector<std::vector<double>>& matrix,
    const std::vector<std::string>& user_ids,
    const std::vector<std::vector<User*>>& communities) {
    
    std::stringstream dot;
    dot << "graph Network {\n";
    
    // Graph attributes remain the same
    dot << "    graph [\n"
        << "        overlap=false\n"
        << "        splines=true\n"
        << "        K=2\n"
        << "        layout=neato\n"
        << "        mindist=1.0\n"
        << "        outputorder=nodesfirst\n"
        << "        pad=0.5\n"
        << "    ];\n\n";
    
    dot << "    node [\n"
        << "        shape=circle\n"
        << "        style=filled\n"
        << "        width=0.6\n"
        << "        height=0.6\n"
        << "        penwidth=1.5\n"
        << "        fontname=\"Arial\"\n"
        << "        fontsize=10\n"
        << "    ];\n\n";
    
    dot << "    edge [\n"
        << "        penwidth=1.5\n"
        << "        color=\"#333333\"\n"
        << "    ];\n\n";

    // Define colors for different communities
    const std::array<std::string, 10> colors = {{
        "\"#ff7f7f\"", "\"#7fbfff\"", "\"#7fff7f\"", "\"#ff7fff\"", "\"#ffff7f\"",
        "\"#7fffff\"", "\"#ff9966\"", "\"#66ff99\"", "\"#9966ff\"", "\"#ff9966\""
    }};

    // Create a map to store user_id to community index mapping
    std::unordered_map<std::string, size_t> userCommunityMap;
    for (size_t communityIdx = 0; communityIdx < communities.size(); communityIdx++) {
        for (const User* user : communities[communityIdx]) {
            userCommunityMap[user->getID()] = communityIdx;
        }
    }

    // Add nodes with community colors
    for (size_t i = 0; i < communities.size(); i++) {
        for (const User* user : communities[i]) {
            dot << "    \"" << user->getID() << "\" ["
                << "fillcolor=" << colors[i % colors.size()] 
                << ", label=\"" << user->getName() << "\"];\n";
        }
    }
    dot << "\n";

    // Add edges only if nodes are in the same community OR weight exceeds threshold
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = i + 1; j < matrix[i].size(); j++) {
            // Get community indices for both users
            auto user1_comm_it = userCommunityMap.find(user_ids[i]);
            auto user2_comm_it = userCommunityMap.find(user_ids[j]);
            
            // Skip if either user is not found in communities
            if (user1_comm_it == userCommunityMap.end() || 
                user2_comm_it == userCommunityMap.end()) {
                continue;
            }

            bool inSameCommunity = (user1_comm_it->second == user2_comm_it->second);
            bool exceedsThreshold = (matrix[i][j] >= weight_threshold);

            // Add edge if either condition is met
            if (inSameCommunity || exceedsThreshold) {
                double len = 5.0 / matrix[i][j];
                dot << "    \"" << user_ids[i] << "\" -- \"" << user_ids[j] 
                    << "\" [len=" << len;
                
                // Optionally, we can style edges differently based on the connection type
                if (inSameCommunity && exceedsThreshold) {
                    dot << ", penwidth=2.5"; // Thicker edges for strong in-community connections
                } else if (inSameCommunity) {
                    dot << ", style=dashed"; // Dashed for same community but weak connection
                }
                
                dot << "];\n";
            }
        }
    }
    
    dot << "}\n";
    return dot.str();
}