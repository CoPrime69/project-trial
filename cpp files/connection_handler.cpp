#include "connection_handler.h"
#include "print_utilities.h"
#include <iostream>

void ConnectionHandler::handleConnectionManagement(ConnectionManager &cm, User *user)
{
    while (true)
    {
        int choice = getMenuChoice();

        switch (choice)
        {
        case 1:
            viewConnections(cm, user);
            break;
        case 2:
            addNewConnections(cm, user);
            break;
        case 3:
            removeConnection(cm, user);
            break;
        case 4:
            return;
        default:
            std::cout << "Invalid option. Please try again." << std::endl;
        }
    }
}

int ConnectionHandler::getMenuChoice()
{
    std::cout << "\nConnection Management Menu:" << std::endl;
    std::cout << "1. View your connections" << std::endl;
    std::cout << "2. Add new connection" << std::endl;
    std::cout << "3. Remove connection" << std::endl;
    std::cout << "4. Exit" << std::endl;
    std::cout << "Choose an option: ";

    int choice;
    std::cin >> choice;
    return choice;
}

void ConnectionHandler::viewConnections(ConnectionManager &cm, User *user)
{
    cm.viewConnections(user);
}

void ConnectionHandler::addNewConnections(ConnectionManager &cm, User *user)
{

        std::vector<std::pair<User *, User *>> recommendations = cm.recommendConnectionsForNewUser(user);
        PrintUtilities::printRecommendedConnections(recommendations, user);

        int selected_connection = PrintUtilities::selectConnection(recommendations);
        if (selected_connection >= 0)
        {
            User *selected_user = recommendations[selected_connection].second;
            cm.addConnection(user, selected_user);
            std::cout << "Successfully connected with " << selected_user->getName() << "!" << std::endl;
        }

        // std::cout << "Add another connection? (y/n): ";
        // char more_choice;
        // std::cin >> more_choice;
        // if (more_choice != 'y' && more_choice != 'Y')
    
}

void ConnectionHandler::removeConnection(ConnectionManager &cm, User *user)
{
    std::cout << "\nYour current connections:" << std::endl;
    cm.viewConnections(user);

    std::cout << "\nEnter the ID of the connection you want to remove: ";
    std::string remove_id;
    std::cin >> remove_id;

    User *connection_to_remove = cm.getUser(remove_id);
    if (connection_to_remove)
    {
        cm.removeConnection(user, connection_to_remove);
        std::cout << "Successfully removed connection with " << connection_to_remove->getName() << std::endl;
    }
    else
    {
        std::cout << "Invalid user ID or not in your connections." << std::endl;
    }
}