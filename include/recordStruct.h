#pragma once
#include <iostream>

struct Record {
	char Author[12];// this 
	char Tittle[32];
	char Publisher[16]; // this
	short int Year;
	short int PageNumber;

	friend std::ostream& operator<<(std::ostream& out,const Record& r1);
};