#include "UI.h"
#include "Commands.h"


#include <iostream>
#include <string>
#include <sstream>
using namespace std;

void printHelp() {
    cout << "\n===== Available Commands =====" << endl;
    cout << "help                     - Show available commands" << endl;
    cout << "insert-m <ID> <Name> <Nat> - Insert a new director (Master table)" << endl;
    cout << "insert-s <ID> <MasterID> <Title> - Insert a new movie (Slave table)" << endl;
    cout << "del-m <ID>               - Delete a director and all related movies" << endl;
    cout << "del-s <ID>               - Delete a specific movie" << endl;
    cout << "update-m <ID> <Name> <Nat> - Update director details" << endl;
    cout << "update-s <ID> <Title>    - Update movie title" << endl;
    cout << "get-m <ID>               - Get director by ID" << endl;
    cout << "get-s <ID>               - Get movie by ID" << endl;
    cout << "calc-m                   - Count all directors and their movies" << endl;
    cout << "calc-s                   - Count all movies" << endl;
    cout << "ut-m                     - Print all directors (including metadata)" << endl;
    cout << "ut-s                     - Print all movies (including metadata)" << endl;
    cout << "exit                     - Exit program\n" << endl;
}

void commandInterface() {
    loadIndexesFromFile();
    loadGarbageFromFile();

    //This is the patch that creates files if there are no for some reason
    initializeFiles();

    string line, cmd;

    cout << "Type 'help' to see available commands" << endl;

    while (true) {
        cout << "\n> ";
        getline(cin, line);

        stringstream ss(line);
        ss >> cmd;

        if (cmd == "help") {
            printHelp();
        }
        else if (cmd == "insert-m") {
            Director newDirector;
            string fullName, nationality;

            ss >> newDirector.directorID;
            ss >> ws;
            getline(ss, fullName);

            size_t lastSpace = fullName.find_last_of(' ');
            if (lastSpace == string::npos) {
                cout << "Error: Invalid input format. Use: insert-m <ID> <Full Name> <Nationality>" << endl;
                continue;
            }

            nationality = fullName.substr(lastSpace + 1);
            fullName = fullName.substr(0, lastSpace);

            if (fullName.empty() || nationality.empty()) {
                cout << "Error: Invalid input format. Use: insert-m <ID> <Full Name> <Nationality>" << endl;
                continue;
            }

            strncpy_s(newDirector.fullName, sizeof(newDirector.fullName), fullName.c_str(), _TRUNCATE);
            strncpy_s(newDirector.nationality, sizeof(newDirector.nationality), nationality.c_str(), _TRUNCATE);

            insertDirector(newDirector);
        }
        else if (cmd == "insert-s") {
            Movie newMovie;
            string movieTitle;

            ss >> newMovie.movieID >> newMovie.directorID;
            ss >> ws;
            getline(ss, movieTitle);

            if (ss.fail() || movieTitle.empty()) {
                cout << "Error: Invalid input format. Use: insert-s <ID> <MasterID> <Title>" << endl;
                continue;
            }

            strncpy_s(newMovie.MovieName, sizeof(newMovie.MovieName), movieTitle.c_str(), _TRUNCATE);
            insertMovie(newMovie);
        }
        else if (cmd == "del-m") {
            int directorID;
            ss >> directorID;
            if (ss.fail()) {
                cout << "Error: Invalid ID format!" << endl;
            }
            else {
                deleteDirector(directorID);
            }
        }
        else if (cmd == "del-s") {
            int movieID;
            ss >> movieID;
            if (ss.fail()) {
                cout << "Error: Invalid ID format!" << endl;
            }
            else {
                deleteMovie(movieID);
            }
        }
        else if (cmd == "update-m") {
            int directorID;
            string newFullName, newNationality;

            ss >> directorID;
            ss >> ws;
            getline(ss, newFullName, ' ');
            getline(ss, newNationality);

            if (ss.fail()) {
                cout << "Error: Invalid format. Use: update-m <ID> <Name> <Nationality>" << endl;
            }
            else {
                updateDirector(directorID, newFullName, newNationality);
            }
        }
        else if (cmd == "update-s") {
            int movieID;
            string newTitle;
            ss >> movieID;
            ss >> ws;
            getline(ss, newTitle);
            if (ss.fail() || newTitle.empty()) {
                cout << "Error: Invalid format. Use: update-s <ID> <Title>" << endl;
            }
            else {
                updateMovie(movieID, newTitle);
            }
        }
        else if (cmd == "get-m") {
            int directorID;
            ss >> directorID;
            if (ss.fail()) {
                cout << "Error: Invalid ID format!" << endl;
            }
            else {
                getDirector(directorID);
            }
        }
        else if (cmd == "get-s") {
            int movieID;
            ss >> movieID;
            if (ss.fail()) {
                cout << "Error: Invalid ID format!" << endl;
            }
            else {
                getMovie(movieID);
            }
        }
        else if (cmd == "calc-m") {
            calcDirectors();
        }
        else if (cmd == "calc-s") {
            calcMovies();
        }
        else if (cmd == "ut-m") {
            printAllDirectors();
        }
        else if (cmd == "ut-s") {
            printAllMovies();
        }
        else if (cmd == "exit") {
            saveIndexesToFile();
            saveGarbageToFile();

            cout << "Exiting program..." << endl;
            break;
        }
        else {
            cout << "Unknown command. Type 'help' for a list of commands." << endl;
        }
    }

}
