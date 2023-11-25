#pragma once
#include <math.h>
#include <iostream>
#include "recordStruct.h"

bool compareLess(Record* r1, Record* r2) {


	auto keyOneAnswer = strcmp(r1->Publisher, r2->Publisher);
	auto keyTwoAnswer = strcmp(r1->Author, r2->Author);

	if (keyOneAnswer > 0)
		return true;
	if (keyOneAnswer < 0)
		return false;
	if (keyOneAnswer == 0) {
		if (keyTwoAnswer > 0)
			return true;
		if (keyTwoAnswer < 0)
			return false;
		if (keyTwoAnswer == 0)
			return false;
	}
	return false;
}

void heapify(Record** recordArr, int L, int R) {
	auto x = recordArr[L];
	auto i = L;

	while (true) {
		auto j = 2 * i + 1;

		if (j > R) {
			break;
		}

		if ((j < R) && compareLess(recordArr[j+1],recordArr[j])) {
			j++;
		}

		if (compareLess(x,recordArr[j])) {
			break;
		}

		recordArr[i] = recordArr[j];
		i = j;
	}
	recordArr[i] = x;
}

void HeapSort(Record** recordArr,int recordsCount) {
	auto L = (int)(floor(recordsCount / 2));

	while (L >= 0) {
		heapify(recordArr, L, recordsCount - 1);
		L--;
	}

	auto R = recordsCount - 1;
	while (R > 0)
	{
		std::swap(recordArr[0],recordArr[R]);
		heapify(recordArr, 0, R - 1);
		R--;
	}
}