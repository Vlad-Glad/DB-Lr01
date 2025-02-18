#pragma once
#pragma pack(1) 
struct Movie{
	int movieID = 0;
	int directorID = 0;
	char MovieName[40] = { 0 };
	int prevMoviePos = -1;
	int nextMoviePos = -1;
	bool isDeleted = false;
};