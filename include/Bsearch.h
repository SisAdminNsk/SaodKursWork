#pragma once
#include <string>
#include <vector>
#include "recordStruct.h"

std::string setSpecificKey(Record** whatField,size_t recordIndex,const std::string& key) {
	auto specificField = std::string(whatField[recordIndex]->Publisher);
	specificField.resize(key.length());
	return specificField;
}

int BSearch(Record* collection[],const size_t size,const std::string& target)
{
	int left = 0, right = size - 1, isfind = -1;
	std::string guess = "";

	while (left < right)
	{	
		auto mid = (left + right) / 2;

		guess = setSpecificKey(collection, mid, target);

		if (guess < target)
			left = mid + 1;
		else right = mid;
	}

	guess = setSpecificKey(collection, right, target);

	if (guess == target)
		isfind = right;
	else
		isfind = -1;

	return isfind;
}

