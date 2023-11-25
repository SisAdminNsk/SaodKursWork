#include "recordStruct.h"
#include <iostream>
#include <string>


std::ostream& operator<<(std::ostream& out, const Record& r1)
{
	out << "\t" << r1.Author << "\t" << r1.Tittle << "\t" << r1.Publisher << "\t" <<
		r1.Year << "\t" << r1.PageNumber << "\n";

	return out;
}