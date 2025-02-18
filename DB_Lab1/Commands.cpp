#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <map>

#include "Commands.h"
#include "Constants.h"

using namespace std;

map<int, streampos> directorIndex;
map<int, streampos> movieIndex;
vector<streampos> garbageZoneDirectors;
vector<streampos> garbageZoneMovies;



void saveIndexesToFile() {
    ofstream dFile("Director.ind", ios::binary | ios::trunc);
    if (!dFile) {
        cerr << "Error: Unable to open Director.ind for writing!" << endl;
        return;
    }

    for (const auto& entry : directorIndex) {
        dFile.write(reinterpret_cast<const char*>(&entry.first), sizeof(int));
        dFile.write(reinterpret_cast<const char*>(&entry.second), sizeof(streampos));
    }
    dFile.close();
    cout << "Director index saved successfully. Entries: " << directorIndex.size() << endl;

    ofstream mFile("Movie.ind", ios::binary | ios::trunc);
    if (!mFile) {
        cerr << "Error: Unable to open Movie.ind for writing!" << endl;
        return;
    }

    for (const auto& entry : movieIndex) {
        mFile.write(reinterpret_cast<const char*>(&entry.first), sizeof(int));
        mFile.write(reinterpret_cast<const char*>(&entry.second), sizeof(streampos));
    }
    mFile.close();
    cout << "Movie index saved successfully. Entries: " << movieIndex.size() << endl;
}

void loadIndexesFromFile() {
    ifstream dFile("Director.ind", ios::binary);
    if (dFile) {
        directorIndex.clear();
        int directorID;
        streampos pos;

        while (dFile.read(reinterpret_cast<char*>(&directorID), sizeof(int))) {
            dFile.read(reinterpret_cast<char*>(&pos), sizeof(streampos));
            directorIndex[directorID] = pos;
        }
        dFile.close();
        cout << "Director index loaded successfully. Entries: " << directorIndex.size() << endl;
    }
    else {
        cerr << "Warning: Director.ind not found or cannot be opened!" << endl;
    }

    ifstream mFile("Movie.ind", ios::binary);
    if (mFile) {
        movieIndex.clear();

        int movieID;
        streampos pos;

        while (mFile.read(reinterpret_cast<char*>(&movieID), sizeof(int))) {
            mFile.read(reinterpret_cast<char*>(&pos), sizeof(streampos));
            movieIndex[movieID] = pos;
        }
        mFile.close();
        cout << "Movie index loaded successfully. Entries: " << movieIndex.size() << endl;
    }
    else {
        cerr << "Warning: Movie.ind not found or cannot be opened!" << endl;
    }
}

void saveGarbageToFile() {
    ofstream gMovieFile("Movie.garbage", ios::binary | ios::trunc);
    if (!gMovieFile) {
        cerr << "Error: Unable to open Movie.garbage for writing!" << endl;
        return;
    }

    size_t size = garbageZoneMovies.size();
    gMovieFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (streampos& pos : garbageZoneMovies) {
        gMovieFile.write(reinterpret_cast<const char*>(&pos), sizeof(streampos));
    }
    gMovieFile.close();
    cout << "Movie garbage saved successfully. Entries: " << garbageZoneMovies.size() << endl;

    ofstream gDirectorFile("Director.garbage", ios::binary | ios::trunc);
    if (!gDirectorFile) {
        cerr << "Error: Unable to open Director.garbage for writing!" << endl;
        return;
    }

    size = garbageZoneDirectors.size();
    gDirectorFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (streampos& pos : garbageZoneDirectors) {
        gDirectorFile.write(reinterpret_cast<const char*>(&pos), sizeof(streampos));
    }
    gDirectorFile.close();
    cout << "Director garbage saved successfully. Entries: " << garbageZoneDirectors.size() << endl;
}

void loadGarbageFromFile() {
    ifstream gMovieFile("Movie.garbage", ios::binary);
    if (gMovieFile) {
        garbageZoneMovies.clear();
        size_t size;
        gMovieFile.read(reinterpret_cast<char*>(&size), sizeof(size));
        for (size_t i = 0; i < size; ++i) {
            streampos pos;
            gMovieFile.read(reinterpret_cast<char*>(&pos), sizeof(streampos));
            garbageZoneMovies.push_back(pos);
        }
        gMovieFile.close();
        cout << "Movie garbage loaded successfully. Entries: " << garbageZoneMovies.size() << endl;
    }
    else {
        cout << "Warning: Movie.garbage not found or cannot be opened!" << endl;
    }

    ifstream gDirectorFile("Director.garbage", ios::binary);
    if (gDirectorFile) {
        garbageZoneDirectors.clear();
        size_t size;
        gDirectorFile.read(reinterpret_cast<char*>(&size), sizeof(size));
        for (size_t i = 0; i < size; ++i) {
            streampos pos;
            gDirectorFile.read(reinterpret_cast<char*>(&pos), sizeof(streampos));
            garbageZoneDirectors.push_back(pos);
        }
        gDirectorFile.close();
        cout << "Director garbage loaded successfully. Entries: " << garbageZoneDirectors.size() << endl;
    }
    else {
        cout << "Warning: Director.garbage not found or cannot be opened!" << endl;
    }
}


void defragmentDirectors() {
    if (garbageZoneDirectors.size() < MAX_DIRECTOR_GARBAGE) {
        return;
    }

    ifstream oldFile("Director.fl", ios::binary);
    ofstream newFile("Director_new.fl", ios::binary | ios::trunc);

    if (!oldFile || !newFile) {
        cerr << "Error: Unable to open files for defragmentation!" << endl;
        return;
    }

    map<int, streampos> newDirectorIndex;
    streampos newPos = 0;
    Director director;

    while (oldFile.read(reinterpret_cast<char*>(&director), sizeof(Director))) {
        if (!director.isDeleted) {
            newFile.seekp(newPos);
            newFile.write(reinterpret_cast<const char*>(&director), sizeof(Director));
            newDirectorIndex[director.directorID] = newPos;
            newPos += sizeof(Director);
        }
    }

    oldFile.close();
    newFile.close();

    remove("Director.fl");
    rename("Director_new.fl", "Director.fl");

    directorIndex = newDirectorIndex;

    garbageZoneDirectors.clear();

    saveIndexesToFile();
    cout << "Defragmentation of Director.fl complete. Remaining directors: " << directorIndex.size() << endl;
}

void defragmentMovies() {
    if (garbageZoneMovies.size() < MAX_MOVIE_GARBAGE) {
        return;
    }

    ifstream oldFile("Movie.fl", ios::binary);
    ofstream newFile("Movie_new.fl", ios::binary | ios::trunc);

    if (!oldFile || !newFile) {
        cerr << "Error: Unable to open files for defragmentation!" << endl;
        return;
    }

    map<int, streampos> newMovieIndex;
    streampos newPos = 0;
    Movie movie;

    while (oldFile.read(reinterpret_cast<char*>(&movie), sizeof(Movie))) {
        if (!movie.isDeleted) {
            newFile.seekp(newPos);
            newFile.write(reinterpret_cast<const char*>(&movie), sizeof(Movie));
            newMovieIndex[movie.movieID] = newPos;
            newPos += sizeof(Movie);
        }
    }

    oldFile.close();
    newFile.close();

    remove("Movie.fl");
    rename("Movie_new.fl", "Movie.fl");

    movieIndex = newMovieIndex;

    garbageZoneMovies.clear();

    saveIndexesToFile();
    cout << "Defragmentation of Movie.fl complete. Remaining movies: " << movieIndex.size() << endl;
}


void getDirector(int directorID) {
	ifstream fileDirFL("Director.fl", ios::app | ios::binary);
	if (!fileDirFL) {
		cerr << "Error opening Director.fl for writing\n";
		return;
	}

	auto it = directorIndex.find(directorID);
	if (it == directorIndex.end()) {
		cout << "Director with ID " << directorID << " not found.\n";
		return;
	}

	fileDirFL.seekg(it->second, ios::beg);

	Director director;
	fileDirFL.read(reinterpret_cast<char*>(&director), sizeof(Director));

	if (fileDirFL.gcount() == sizeof(Director)) {
		if (!director.isDeleted) {
			cout << "Director ID: " << director.directorID << "\n";
			cout << "Full Name: " << director.fullName << "\n";
			cout << "Nationality: " << director.nationality << "\n";
		}
		else {
			cout << "Director with ID " << directorID << " has been deleted.\n";
		}
	}
	else {
		cout << "Error reading director data.\n";
	}
}

void getMovie(int movieID) {
    fstream fileMovFL("Movie.fl", ios::binary | ios::in);

    if (!fileMovFL) {
        cerr << "Error opening Movie.fl for reading!" << endl;
        return;
    }

    auto movieIt = movieIndex.find(movieID);
    if (movieIt == movieIndex.end()) {
        cerr << "Error: Movie with ID " << movieID << " not found!" << endl;
        return;
    }
    streampos moviePos = movieIt->second;
    fileMovFL.seekg(moviePos);

    Movie movie;
    fileMovFL.read(reinterpret_cast<char*>(&movie), sizeof(Movie));

    if (movie.isDeleted) {
        cerr << "Error: Movie with ID " << movieID << " is marked as deleted!" << endl;
        return;
    }

    cout << "Movie ID: " << movie.movieID << endl;
    cout << "Director ID: " << movie.directorID << endl;
    cout << "Title: " << movie.MovieName << endl;

    fileMovFL.close();
}

void deleteMovie(int movieID) {
    fstream fileMovFL("Movie.fl", ios::binary | ios::in | ios::out);
    fstream fileDirFL("Director.fl", ios::binary | ios::in | ios::out);

    if (!fileMovFL || !fileDirFL) {
        cerr << "Error: Unable to open files for deletion!" << endl;
        return;
    }

    auto movieIt = movieIndex.find(movieID);
    if (movieIt == movieIndex.end()) {
        cerr << "Error: Movie with ID " << movieID << " not found in index!" << endl;
        return;
    }

    streampos moviePos = movieIt->second;
    fileMovFL.seekg(moviePos);

    Movie movie; 
    fileMovFL.read(reinterpret_cast<char*>(&movie), sizeof(Movie));

    if (movie.isDeleted) {
        cerr << "Error: Movie with ID " << movieID << " is already deleted!" << endl;
        return;
    }

    movie.isDeleted = true;
    fileMovFL.seekp(moviePos);
    fileMovFL.write(reinterpret_cast<const char*>(&movie), sizeof(Movie));
    
    movieIndex.erase(movieID);

    garbageZoneMovies.push_back(moviePos);

    if (movie.prevMoviePos != -1) {
        fileMovFL.seekg(movie.prevMoviePos);
        Movie prevMovie;
        fileMovFL.read(reinterpret_cast<char*>(&prevMovie), sizeof(Movie));
        prevMovie.nextMoviePos = movie.nextMoviePos;

        fileMovFL.seekp(movie.prevMoviePos);
        fileMovFL.write(reinterpret_cast<const char*>(&prevMovie), sizeof(Movie));
    }

    if (movie.nextMoviePos != -1) {
        fileMovFL.seekg(movie.nextMoviePos);
        Movie nextMovie;
        fileMovFL.read(reinterpret_cast<char*>(&nextMovie), sizeof(Movie));
        nextMovie.prevMoviePos = movie.prevMoviePos;

        fileMovFL.seekp(movie.nextMoviePos);
        fileMovFL.write(reinterpret_cast<const char*>(&nextMovie), sizeof(Movie));
    }

    auto dirIt = directorIndex.find(movie.directorID);
    if (dirIt != directorIndex.end()) {
        streampos dirPos = dirIt->second;
        fileDirFL.seekg(dirPos);

        Director director;
        fileDirFL.read(reinterpret_cast<char*>(&director), sizeof(Director));

        if (director.firstMovieId == movieID) {
            if (movie.nextMoviePos == -1) {
                director.firstMovieId = -1;  
            }
            else {
                fileMovFL.seekg(movie.nextMoviePos);
                Movie nextMovie;
                fileMovFL.read(reinterpret_cast<char*>(&nextMovie), sizeof(Movie));

                director.firstMovieId = nextMovie.movieID; 
            }
        }
        if (director.lastMovieId == movieID) {
            if (movie.prevMoviePos == -1) {
                director.lastMovieId = -1;  
            }
            else {
                fileMovFL.seekg(movie.prevMoviePos);
                Movie prevMovie;
                fileMovFL.read(reinterpret_cast<char*>(&prevMovie), sizeof(Movie));
                
                director.lastMovieId = prevMovie.movieID;
            }
        }
        fileDirFL.seekp(dirPos);
        fileDirFL.write(reinterpret_cast<const char*>(&director), sizeof(Director));
    }
    fileMovFL.close();
    fileDirFL.close();

    cout << "Movie deleted successfully: " << movieID << endl;

    if (garbageZoneMovies.size() >= MAX_MOVIE_GARBAGE) {
        cout << "Garbage limit reached. Starting defragmentation..." << endl;
        defragmentMovies();
    }
}

void deleteDirector(int directorID) {
    fstream fileDirFL("Director.fl", ios::binary | ios::in | ios::out);
    fstream fileMovFL("Movie.fl", ios::binary | ios::in | ios::out);

    if (!fileDirFL || !fileMovFL) {
        cerr << "Error: Unable to open files for deletion!" << endl;
        return;
    }

    auto dirIt = directorIndex.find(directorID);
    if (dirIt == directorIndex.end()) {
        cerr << "Error: Director with ID " << directorID << " not found in index!" << endl;
        return;
    }

    streampos dirPos = dirIt->second;
    fileDirFL.seekg(dirPos);

    Director director;
    fileDirFL.read(reinterpret_cast<char*>(&director), sizeof(Director));

    if (director.isDeleted) {
        cerr << "Error: Director with ID " << directorID << " is already deleted!" << endl;
        return;
    }
    
    while (director.firstMovieId != -1) { 
        int movieID = director.firstMovieId;
        deleteMovie(movieID); 
        fileDirFL.seekg(dirPos);
        fileDirFL.read(reinterpret_cast<char*>(&director), sizeof(Director));
    }

    director.isDeleted = true;
    fileDirFL.seekp(dirPos);
    fileDirFL.write(reinterpret_cast<const char*>(&director), sizeof(Director));
    
    directorIndex.erase(directorID);
    garbageZoneDirectors.push_back(dirPos);

    fileDirFL.close();
    fileMovFL.close();

    cout << "Director deleted successfully, Id: " << directorID << endl;

    if (garbageZoneDirectors.size() >= MAX_DIRECTOR_GARBAGE) {
        cout << "Garbage limit reached. Starting defragmentation..." << endl;
        defragmentDirectors();
    }
}


void updateDirector(int directorID, const string& newFullName, const string& newNationality) {
    fstream fileDirFL("Director.fl", ios::binary | ios::in | ios::out);
    if (!fileDirFL) {
        cerr << "Error: Unable to open Director.fl for updating!" << endl;
        return;
    }

    auto dirIt = directorIndex.find(directorID);
    if (dirIt == directorIndex.end()) {
        cerr << "Error: Director with ID " << directorID << " not found in index!" << endl;
        return;
    }

    streampos dirPos = dirIt->second;
    fileDirFL.seekg(dirPos);

    Director director;
    fileDirFL.read(reinterpret_cast<char*>(&director), sizeof(Director));

    if (director.isDeleted) {
        cerr << "Error: Director with ID " << directorID << " is marked as deleted!" << endl;
        return;
    }

    if (!newFullName.empty()) {
        strncpy_s(director.fullName, sizeof(director.fullName), newFullName.c_str(), _TRUNCATE);
    }

    if (!newNationality.empty()) {
        strncpy_s(director.nationality, sizeof(director.nationality), newNationality.c_str(), _TRUNCATE);
    }

    fileDirFL.seekp(dirPos);
    fileDirFL.write(reinterpret_cast<const char*>(&director), sizeof(Director));

    fileDirFL.close();
    cout << "Director with Id "<<directorID<<" updated successfully!" <<endl;
}

void updateMovie(int movieID, const string& newMovieName) {
    fstream fileMovFL("Movie.fl", ios::binary | ios::in | ios::out);
    if (!fileMovFL) {
        cerr << "Error: Unable to open Movie.fl for updating!" << endl;
        return;
    }

    auto movieIt = movieIndex.find(movieID);
    if (movieIt == movieIndex.end()) {
        cerr << "Error: Movie with ID " << movieID << " not found in index!" << endl;
        return;
    }

    streampos moviePos = movieIt->second;
    fileMovFL.seekg(moviePos);

    Movie movie;
    fileMovFL.read(reinterpret_cast<char*>(&movie), sizeof(Movie));

    if (movie.isDeleted) {
        cerr << "Error: Movie with ID " << movieID << " is marked as deleted!" << endl;
        return;
    }

    if (!newMovieName.empty()) {
        strncpy_s(movie.MovieName, sizeof(movie.MovieName), newMovieName.c_str(), _TRUNCATE);
    }

    fileMovFL.seekp(moviePos);
    fileMovFL.write(reinterpret_cast<const char*>(&movie), sizeof(Movie));

    fileMovFL.close();
    cout << "Movie with Id "<< movieID<< " updated successfully!" << endl;
}

void insertDirector(const Director& director) {
    if (directorIndex.find(director.directorID) != directorIndex.end()) {
        cerr << "Error: Director with ID " << director.directorID << " already exists!" << endl;
        return;
    }

    fstream fileDirFL("Director.fl", ios::binary | ios::in | ios::out);
	if (!fileDirFL) {
		cerr << "Error opening Director.fl for writing\n";
		return;
	}

	streampos pos;
	if (!garbageZoneDirectors.empty()) {
		pos = garbageZoneDirectors.back();
		garbageZoneDirectors.pop_back();
		fileDirFL.seekp(pos);
	}
	else {
		fileDirFL.seekp(0, ios::end);
		pos = fileDirFL.tellp();
	}

	fileDirFL.write(reinterpret_cast<const char*>(&director), sizeof(Director));
	directorIndex[director.directorID] = pos;
	fileDirFL.close();
    cout << "Director inserted successfully!"<<endl;
}

void insertMovie(const Movie& movie) {
    if (movieIndex.find(movie.movieID) != movieIndex.end()) {
        cerr << "Error: Movie with ID " << movie.directorID << " already exists!" << endl;
        return;
    }

    fstream fileMovFL("Movie.fl", ios::binary | ios::in | ios::out);
    fstream fileDirFL("Director.fl", ios::binary | ios::in | ios::out);

    if (!fileMovFL || !fileDirFL) {
        cerr << "Error opening files!" << endl;
        return;
    }
    auto dirIt = directorIndex.find(movie.directorID);
    if (dirIt == directorIndex.end()) {
        cerr << "Error: Director with ID " << movie.directorID << " not found!" << endl;
        return;
    }

    streampos dirPos = dirIt->second;
    fileDirFL.seekg(dirPos);
    Director director;
    fileDirFL.read(reinterpret_cast<char*>(&director), sizeof(Director));

    if (director.isDeleted) {
        cerr << "Error: Director is marked as deleted!" << endl;
        return;
    }

    streampos pos;
    if (!garbageZoneMovies.empty()) {
        pos = garbageZoneMovies.back();
        garbageZoneMovies.pop_back();
        fileMovFL.seekp(pos);
    }
    else {
        fileMovFL.seekp(0, ios::end);
        pos = fileMovFL.tellp();
    }

    Movie newMovie = movie;
    newMovie.prevMoviePos = -1;
    newMovie.nextMoviePos = -1;

    if (director.firstMovieId == -1) {
        director.firstMovieId = movie.movieID;
        director.lastMovieId = movie.movieID;
    }
    else {
        auto movieIt = movieIndex.find(director.lastMovieId);
        if (movieIt == movieIndex.end()) {
            cerr << "Error: Last movie with ID " << director.lastMovieId << " not found in index!" << endl;
            return;
        }

        streampos lastMoviePos = movieIt->second;
        fileMovFL.seekg(lastMoviePos);
        Movie lastMovie;
        fileMovFL.read(reinterpret_cast<char*>(&lastMovie), sizeof(Movie));

        lastMovie.nextMoviePos = pos;

        fileMovFL.seekp(lastMoviePos + static_cast<streamoff>(offsetof(Movie, nextMoviePos)), ios::beg);
        fileMovFL.write(reinterpret_cast<const char*>(&pos), sizeof(streampos));

        newMovie.prevMoviePos = lastMoviePos;
        director.lastMovieId = movie.movieID;
    }

    fileMovFL.seekp(pos);
    fileMovFL.write(reinterpret_cast<const char*>(&newMovie), sizeof(Movie));

    if (!fileMovFL) {
        cerr << "Error: Failed to write movie to file!" << endl;
        return;
    }
    movieIndex[movie.movieID] = pos;

    fileDirFL.seekp(dirPos);
    fileDirFL.write(reinterpret_cast<const char*>(&director), sizeof(Director));

    fileMovFL.close();
    fileDirFL.close();
    cout << "Movie inserted successfully!"<< endl;
}

void calcDirectors() {
    cout << directorIndex.size() << endl;
}

void calcMovies() {
    cout << movieIndex.size() << endl;
}

void printAllDirectors() {
    ifstream fileDirFL("Director.fl", ios::binary);
    if (!fileDirFL) {
        cerr << "Error: Unable to open Director.fl for reading!" << endl;
        return;
    }

    Director director;
    while (fileDirFL.read(reinterpret_cast<char*>(&director), sizeof(Director))) {
        cout << "Director ID: " << director.directorID;
        cout << " |Name: " << director.fullName;
        cout << " |Nationality: " << director.nationality;
        cout << " |First Movie ID: " << (director.firstMovieId == -1 ? "None" : to_string(director.firstMovieId));
        cout << " |Last Movie ID: " << (director.lastMovieId == -1 ? "None" : to_string(director.lastMovieId));
        cout << " |Is Deleted: " << director.isDeleted << endl;
    }
    fileDirFL.close();
}

void printAllMovies() {
    ifstream fileMovFL("Movie.fl", ios::binary);
    if (!fileMovFL) {
        cerr << "Error: Unable to open Movie.fl for reading!" << endl;
        return;
    }

    Movie movie;
    while (fileMovFL.read(reinterpret_cast<char*>(&movie), sizeof(Movie))) {
        cout << "Movie ID: " << movie.movieID;
        cout << " |Director ID: " << movie.directorID;
        cout << " |Title: " << movie.MovieName;
        cout << " |Previous Movie Position: " << (movie.prevMoviePos == -1 ? "None" : to_string(movie.prevMoviePos));
        cout << " |Next Movie Position: " << (movie.nextMoviePos == -1 ? "None" : to_string(movie.nextMoviePos));
        cout << " |Is Deleted: " << movie.isDeleted << endl;
    }
    fileMovFL.close();
}


// Service functions

void initializeFiles() {
    // Масив імен файлів
    const char* files[] = {
        "Movie.fl", "Director.fl",
        "Movie.ind", "Director.ind",
        "Movie.garbage", "Director.garbage"
    };

    for (const char* filename : files) {
        ifstream file(filename);
        if (!file) {
            cout << "File " << filename << " not found. Creating an empty file..." << endl;
            ofstream newFile(filename, ios::binary);
            newFile.close();
        }
    }
}
