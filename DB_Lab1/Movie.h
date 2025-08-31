#pragma once
#pragma pack(1) 
struct Movie{
	int movieID;
	int directorID;
	char MovieName[40];
	int prevMoviePos = -1;
	int nextMoviePos = -1;
	bool isDeleted = false;
};