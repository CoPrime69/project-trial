#ifndef PRINT_UTILITIES_H
#define PRINT_UTILITIES_H

#include "connection_manager.h"
#include <vector>
#include <unordered_map>
#include <string>

class PrintUtilities
{
public:
    static void printResults(ConnectionManager &cm,
                             const std::unordered_map<std::string, double> &pagerank,
                             const std::vector<std::vector<User *>> &communities);

    static void printRecommendedCommunities(const std::vector<std::vector<User *>> &communities,
                                            User *new_user);

    static void printRecommendedConnections(const std::vector<std::pair<User *, User *>> &recommendations,
                                            User *new_user);

    static int selectConnection(const std::vector<std::pair<User *, User *>> &recommendations);

private:
    static std::vector<int> g_validIndices;
    static int g_currentStart;
};

#endif