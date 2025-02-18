#pragma once
#pragma pack(1) 
struct Director {
	int directorID = 0;
	char fullName[40] = { 0 };
	char nationality[20] = { 0 };
	int firstMovieId = -1;
	int lastMovieId = -1;
	bool isDeleted = false;
};
