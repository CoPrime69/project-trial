#include "graph_visualizer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <array>
#include <iomanip>
// #include <filesystem>

GraphVisualizer::GraphVisualizer(double threshold) : weight_threshold(threshold) {}

std::vector<std::vector<double>> GraphVisualizer::readAdjacencyMatrix(
    const std::string &matrix_file,
    std::vector<std::string> &user_ids)
{

    std::ifstream file(matrix_file);
    if (!file.is_open())
    {
        std::cerr << "Could not open matrix file: " << matrix_file << std::endl;
        return std::vector<std::vector<double>>();
    }

    std::vector<std::vector<double>> matrix;
    std::string line;

    // Read header to get user IDs
    if (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cell;
        std::getline(ss, cell, ','); // Skip empty cell

        while (std::getline(ss, cell, ','))
        {
            if (!cell.empty())
            {
                user_ids.push_back(cell);
            }
        }
    }

    // Read matrix data
    while (std::getline(file, line))
    {
        std::vector<double> row;
        std::stringstream ss(line);
        std::string cell;

        std::getline(ss, cell, ','); // Skip row header

        while (std::getline(ss, cell, ','))
        {
            if (!cell.empty())
            {
                try
                {
                    row.push_back(std::stod(cell));
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error converting value: " << cell << std::endl;
                    row.push_back(0.0);
                }
            }
        }

        if (!row.empty())
        {
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

    // Construct the output path by combining png_graphs directory with output filename
    std::string output_path = "png_graphs/" + output_file;

    // Modified command for 4K resolution (3840 x 3840) with path to png_graphs folder
    std::string command = "dot -Kneato"
                         " -Tpng"
                         " -Gdpi=96"
                         " -Gsize=\"40,40!\""  // 3840/96 = 40 inches
                         " -Goverlap=scale"
                         " -Gsplines=true"
                         " -Gstart=random"
                         " -Gepsilon=0.0001"
                         " -Gmaxiter=1000"
                         " " + dot_file + " -o " + output_path;

    int result = system(command.c_str());

    if (result == 0) {
        std::cout << "Graph visualization created successfully: " << output_path << std::endl;
        std::remove(dot_file.c_str());
    } else {
        std::cerr << "Failed to create graph visualization" << std::endl;
    }
}

// void GraphVisualizer::createGraph(const std::string& matrix_file,
//                                 const std::vector<std::vector<User*>>& communities,
//                                 const std::string& output_file) {
//     // Create png_graphs directory if it doesn't exist
//     std::filesystem::path dir_path = "png_graphs";
//     if (!std::filesystem::exists(dir_path)) {
//         std::filesystem::create_directory(dir_path);
//     }
    
//     std::vector<std::string> user_ids;
//     std::vector<std::vector<double>> matrix = readAdjacencyMatrix(matrix_file, user_ids);
    
//     if (matrix.empty()) {
//         std::cerr << "Failed to read adjacency matrix" << std::endl;
//         return;
//     }
    
//     std::string dot_file = "temp_graph.dot";
//     std::ofstream dot_out(dot_file);
//     dot_out << generateDotFormat(matrix, user_ids, communities);
//     dot_out.close();
    
//     // Modify output path to use png_graphs folder
//     std::string output_path = (dir_path / std::filesystem::path(output_file)).string();
    
//     // Modified command for 4K resolution (3840 x 2160) with additional space for legend
//     std::string command = "dot -Kneato"
//                          " -Tpng"
//                          " -Gdpi=96"
//                          " -Gsize=\"40,24!\""  // Increased height for legend
//                          " -Goverlap=scale"
//                          " -Gsplines=true"
//                          " -Gstart=random"
//                          " -Gepsilon=0.0001"
//                          " -Gmaxiter=1000"
//                          " " + dot_file + " -o " + output_path;
    
//     int result = system(command.c_str());
    
//     if (result == 0) {
//         std::cout << "Graph visualization created successfully: " << output_path << std::endl;
//         std::remove(dot_file.c_str());
//     } else {
//         std::cerr << "Failed to create graph visualization" << std::endl;
//     }
// }

// std::string GraphVisualizer::generateDotFormat(
//     const std::vector<std::vector<double>> &matrix,
//     const std::vector<std::string> &user_ids,
//     const std::vector<std::vector<User *>> &communities)
// {

//     std::stringstream dot;
//     dot << "graph Network {\n";

//     // Graph attributes remain the same
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

//     // Define colors for different communities
//     const std::array<std::string, 10> colors = {{"\"#ff7f7f\"", "\"#7fbfff\"", "\"#7fff7f\"", "\"#ff7fff\"", "\"#ffff7f\"",
//                                                  "\"#7fffff\"", "\"#ff9966\"", "\"#66ff99\"", "\"#9966ff\"", "\"#ff9966\""}};

//     // Create a map to store user_id to community index mapping
//     std::unordered_map<std::string, size_t> userCommunityMap;
//     for (size_t communityIdx = 0; communityIdx < communities.size(); communityIdx++)
//     {
//         for (const User *user : communities[communityIdx])
//         {
//             userCommunityMap[user->getID()] = communityIdx;
//         }
//     }

//     // Add nodes with community colors
//     for (size_t i = 0; i < communities.size(); i++)
//     {
//         for (const User *user : communities[i])
//         {
//             dot << "    \"" << user->getID() << "\" ["
//                 << "fillcolor=" << colors[i % colors.size()]
//                 << ", label=\"" << user->getName() << "\"];\n";
//         }
//     }
//     dot << "\n";

//     // Add edges only if nodes are in the same community OR weight exceeds threshold
//     for (size_t i = 0; i < matrix.size(); i++)
//     {
//         for (size_t j = i + 1; j < matrix[i].size(); j++)
//         {
//             // Get community indices for both users
//             auto user1_comm_it = userCommunityMap.find(user_ids[i]);
//             auto user2_comm_it = userCommunityMap.find(user_ids[j]);

//             // Skip if either user is not found in communities
//             // if (user1_comm_it == userCommunityMap.end() ||
//             //     user2_comm_it == userCommunityMap.end()) {
//             //     continue;
//             // }

//             // bool inSameCommunity = (user1_comm_it->second == user2_comm_it->second);
//             bool exceedsThreshold = (matrix[i][j] >= weight_threshold);

//             // Add edge if either condition is met
//             // if (inSameCommunity || exceedsThreshold) {
//             if (exceedsThreshold)
//             {

//                 double len = 5.0 / matrix[i][j];
//                 dot << "    \"" << user_ids[i] << "\" -- \"" << user_ids[j]
//                     << "\" [len=" << len;

//                 // Optionally, we can style edges differently based on the connection type
//                 // if (inSameCommunity && exceedsThreshold) {
//                 //     dot << ", penwidth=2.5"; // Thicker edges for strong in-community connections
//                 // // } else if (inSameCommunity) {
//                 // //     dot << ", style=dashed"; // Dashed for same community but weak connection
//                 // // }

//                 if (exceedsThreshold)
//                     dot << ", penwidth=2.5"; // Thicker edges for strong connections

//                 dot << "];\n";
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
    
    // Graph attributes - Modified to constrain layout area
    dot << "    graph [\n"
        << "        overlap=false\n"
        << "        splines=true\n"
        << "        K=2\n"
        << "        layout=neato\n"
        << "        mindist=1.0\n"
        << "        outputorder=nodesfirst\n"
        << "        pad=0.5\n"
        << "        margin=2.0\n"      // Increased margin
        << "        size=\"35,24!\"\n"  // Restrict main graph size to leave room for legend
        << "    ];\n\n";
    
    // Node attributes
    dot << "    node [\n"
        << "        shape=circle\n"
        << "        style=filled\n"
        << "        width=0.6\n"
        << "        height=0.6\n"
        << "        penwidth=1.5\n"
        << "        fontname=\"Arial\"\n"
        << "        fontsize=10\n"
        << "    ];\n\n";
    
    // Edge attributes
    dot << "    edge [\n"
        << "        penwidth=1.5\n"
        << "        color=\"#333333\"\n"
        << "    ];\n\n";

    // Define colors for different communities
    const std::array<std::string, 10> colors = {{
        "\"#ff7f7f\"", "\"#7fbfff\"", "\"#7fff7f\"", "\"#ff7fff\"", "\"#ffff7f\"",
        "\"#7fffff\"", "\"#ff9966\"", "\"#66ff99\"", "\"#9966ff\"", "\"#ff9966\""
    }};

    // Add legend container first
    dot << "    // Legend box\n";
    dot << "    subgraph cluster_legend {\n";
    dot << "        margin=20;\n"
        << "        label=\"\";\n"
        << "        fontsize=14;\n"
        << "        style=filled;\n"
        << "        color=white;\n"
        << "        bgcolor=white;\n"
        << "        pos=\"35.5,18!\";\n";  // Adjusted position further right
    
    // Add title for the legend
    dot << "        \"legend_title\" [shape=none;\n"
        << "            label=\"Communities\";\n"
        << "            fontsize=12;\n"
        << "            fontname=\"Arial Bold\";\n"
        << "            pos=\"36.5,17!\";\n"
        << "        ];\n";
    
    // Create vertical legend entries with adjusted positioning
    float y_pos = 16.5;  // Start below the title
    for (size_t i = 0; i < communities.size(); i++) {
        std::string legendNodeId = "legend_" + std::to_string(i);
        std::string legendLabelId = "legend_label_" + std::to_string(i);
        
        // Add legend node (colored square)
        dot << "        \"" << legendNodeId << "\" [\n"
            << "            shape=square;\n"
            << "            style=filled;\n"
            << "            fillcolor=" << colors[i % colors.size()] << ";\n"
            << "            width=0.3;\n"
            << "            height=0.3;\n"
            << "            label=\"\";\n"
            << "            pos=\"35.8," << y_pos << "!\";\n"
            << "        ];\n";
        
        // Add legend label with adjusted position
        dot << "        \"" << legendLabelId << "\" [\n"
            << "            shape=none;\n"
            << "            fontsize=10;\n"
            << "            label=\" Community " << i + 1 << "\";\n"
            << "            pos=\"37.2," << y_pos << "!\";\n"
            << "        ];\n";
        
        y_pos -= 0.6;
    }
    dot << "    }\n\n";

    // Create a map to store user_id to community index mapping
    std::unordered_map<std::string, size_t> userCommunityMap;
    for (size_t communityIdx = 0; communityIdx < communities.size(); communityIdx++) {
        for (const User* user : communities[communityIdx]) {
            userCommunityMap[user->getID()] = communityIdx;
        }
    }

    // Add graph nodes with community colors
    for (size_t i = 0; i < communities.size(); i++) {
        for (const User* user : communities[i]) {
            dot << "    \"" << user->getID() << "\" ["
                << "fillcolor=" << colors[i % colors.size()] 
                << ", label=\"" << user->getName() << "\"];\n";
        }
    }
    dot << "\n";

    // Add edges with constraints to keep within main graph area
    for (size_t i = 0; i < matrix.size(); i++) {
        for (size_t j = i + 1; j < matrix[i].size(); j++) {
            bool exceedsThreshold = (matrix[i][j] >= weight_threshold);

            if (exceedsThreshold) {
                double len = 5.0 / matrix[i][j];
                dot << "    \"" << user_ids[i] << "\" -- \"" << user_ids[j] 
                    << "\" [len=" << len
                    << ", penwidth=2.5];\n";
            }
        }
    }

    // Add constraint to keep nodes within main graph area
    dot << "    {rank=same; ";
    for (const auto& user_id : user_ids) {
        dot << "\"" << user_id << "\"; ";
    }
    dot << "}\n";
    
    dot << "}\n";
    return dot.str();
}