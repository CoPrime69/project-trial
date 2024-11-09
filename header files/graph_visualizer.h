#ifndef GRAPH_VISUALIZER_H
#define GRAPH_VISUALIZER_H

#include <bits/stdc++.h>
#include "user.h"

using namespace std;

class GraphVisualizer
{
public:
    GraphVisualizer(double threshold = 15.0); // Constructor with weight threshold

    // Main visualization function
    void createGraph(const  string &matrix_file,
                     const  vector< vector<User *>> &communities,
                     const  string &output_file);

private:
    double weight_threshold; // Minimum weight for edge creation

    // Helper functions
     vector< vector<double>> readAdjacencyMatrix(const  string &matrix_file,
                                                          vector< string> &user_ids);

     string generateDotFormat(const  vector< vector<double>> &matrix,
                                  const  vector< string> &user_ids,
                                  const  vector< vector<User *>> &communities);

     string getNodeColor(int community_index);
     string generateEdgeStyle(double weight);
    int findCommunityIndex(const  string &user_id,
                           const  vector< vector<User *>> &communities);
};

#endif // GRAPH_VISUALIZER_H