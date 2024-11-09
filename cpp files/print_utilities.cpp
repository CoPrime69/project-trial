#include "print_utilities.h"
#include <bits/stdc++.h>
#include <algorithm>

std::vector<int> PrintUtilities::g_validIndices;
int PrintUtilities::g_currentStart = 0;

void PrintUtilities::printResults(ConnectionManager &cm,
                                  const std::unordered_map<std::string, double> &pagerank,
                                  const std::vector<std::vector<User *>> &communities)
{
    std::cout << "Top 5 Influencers:" << std::endl;
    std::vector<std::pair<std::string, double>> top_influencers;

    for (const auto &pair : pagerank)
    {
        top_influencers.emplace_back(pair.first, pair.second);
    }

    std::sort(top_influencers.begin(), top_influencers.end(),
              [](const auto &a, const auto &b)
              { return a.second > b.second; });

    for (size_t i = 0; i < 5 && i < top_influencers.size(); ++i)
    {
        std::cout << "User: " << cm.getUser(top_influencers[i].first)->getName()
                  << ", Influence: " << top_influencers[i].second << std::endl;
    }

    std::cout << "\nCommunities:" << std::endl;
    for (size_t i = 0; i < communities.size(); ++i)
    {
        std::cout << "Community " << i + 1 << ": ";
        for (User *user : communities[i])
        {
            std::cout << user->getName() << ", ";
        }
        std::cout << std::endl;
    }
}

void PrintUtilities::printRecommendedCommunities(const std::vector<std::vector<User *>> &communities,
                                                 User *new_user)
{
    int matchingCommunityCount = 0;
    for (const auto &community : communities)
    {
        if (matchingCommunityCount >= 3)
            break;

        bool is_match = false;
        for (User *user : community)
        {
            if (user->getCategory() == new_user->getCategory() &&
                user->getBranch() == new_user->getBranch())
            {
                is_match = true;
                break;
            }
        }

        if (is_match)
        {
            matchingCommunityCount++;
            std::cout << "Community " << matchingCommunityCount << ": ";
            for (User *user : community)
            {
                std::cout << user->getName() << ", ";
            }
            std::cout << std::endl;
        }
    }
}

void PrintUtilities::printRecommendedConnections(const std::vector<std::pair<User *, User *>> &recommendations,
                                                 User *new_user)
{
    std::cout << "\nRecommended connections based on category and influence:" << std::endl;
    int validIndex = 0;
    g_validIndices.clear();

    int skipped = 0;
    for (int i = 0; i < recommendations.size() && validIndex < 5; i++)
    {
        const auto &pair = recommendations[i];

        if ((pair.first->getCategory() == new_user->getCategory() &&
             pair.first->getBranch() == new_user->getBranch()) ||
            (pair.second->getCategory() == new_user->getCategory() &&
             pair.second->getBranch() == new_user->getBranch()))
        {

            if (skipped < g_currentStart)
            {
                skipped++;
                continue;
            }

            std::cout << validIndex << ": " << pair.first->getName() << " <-> "
                      << pair.second->getName() << " (Category: " << pair.second->getCategory()
                      << ", Branch: " << pair.second->getBranch() << ")" << std::endl;

            g_validIndices.push_back(i);
            validIndex++;
        }
    }

    if (validIndex == 0)
    {
        if (g_currentStart == 0)
        {
            std::cout << "No matching recommendations found." << std::endl;
        }
        else
        {
            std::cout << "No more recommendations available." << std::endl;
            g_currentStart = 0;
        }
    }
}

int PrintUtilities::selectConnection(const std::vector<std::pair<User *, User *>> &recommendations)
{
    if (g_validIndices.empty())
    {
        return -1;
    }

    std::cout << "\nSelect a connection to add (enter number 0-" << g_validIndices.size() - 1
              << ", or -1 to skip, or any other negative number to exit): ";
    int displayIndex;
    std::cin >> displayIndex;

    if (displayIndex == -1)
    {
        g_currentStart += 5;
        printRecommendedConnections(recommendations, recommendations[0].first);
        return selectConnection(recommendations);
    }
    else if (displayIndex < -1)
    {
        g_currentStart = 0;
        return -1;
    }
    else if (displayIndex >= 0 && displayIndex < g_validIndices.size())
    {
        g_currentStart = 0;
        return g_validIndices[displayIndex];
    }

    std::cout << "Invalid selection. Please try again." << std::endl;
    return selectConnection(recommendations);
}