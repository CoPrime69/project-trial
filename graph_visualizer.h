#ifndef GRAPH_VISUALIZER_H
#define GRAPH_VISUALIZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "user.h"

class GraphVisualizer {
public:
    GraphVisualizer(double threshold = 17.0);  // Constructor with weight threshold
    
    // Main visualization function
    void createGraph(const std::string& matrix_file, 
                    const std::vector<std::vector<User*>>& communities,
                    const std::string& output_file);

private:
    double weight_threshold;  // Minimum weight for edge creation
    
    // Helper functions
    std::vector<std::vector<double>> readAdjacencyMatrix(const std::string& matrix_file,
                                                        std::vector<std::string>& user_ids);
    
    std::string generateDotFormat(const std::vector<std::vector<double>>& matrix,
                                const std::vector<std::string>& user_ids,
                                const std::vector<std::vector<User*>>& communities);
    
    std::string getNodeColor(int community_index);
    std::string generateEdgeStyle(double weight);
    int findCommunityIndex(const std::string& user_id, 
                          const std::vector<std::vector<User*>>& communities);
};

#endif // GRAPH_VISUALIZER_H