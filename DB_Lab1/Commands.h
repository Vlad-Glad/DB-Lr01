#ifndef COMMANDS_H
#define COMMANDS_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string> 
#include <cstring> 

#include "Director.h"
#include "Movie.h"

using namespace std;

extern map<int, streampos> directorIndex;
extern map<int, streampos> movieIndex;
extern vector<streampos> garbageZoneDirectors;
extern vector<streampos> garbageZoneMovies;

void loadIndexesFromFile();
void saveIndexesToFile();  
void saveGarbageToFile();
void loadGarbageFromFile();

void getDirector(int directorID);
void getMovie(int movieID);

void deleteDirector(int directorID);
void deleteMovie(int movieID);

void updateDirector(int directorID, const string& newFullName, const string& newNationality);
void updateMovie(int movieID, const string& newMovieName);

void insertDirector(const Director& director);
void insertMovie(const Movie& movie);

void calcDirectors();
void calcMovies();

void printAllDirectors();
void printAllMovies();

void defragmentDirectors();
void defragmentMovies();

// Service functions
void initializeFiles();

#endif // COMMANDS_H


