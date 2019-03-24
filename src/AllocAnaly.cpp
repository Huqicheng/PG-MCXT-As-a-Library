#include <cstring>
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "MemoryContextTool.h"
#include "AllocAnaly.h"
#include "AllocSetContext.h"
#include "Row.h"


string	AllocAnaly::rowName[6] = { 
				"Chunk Size",
				"Alloced Times",
				"Wasted Space",
				"Average Wasted",
				"Free Times",
				"Hit Rate"
			       };

// Constructor
AllocAnaly::AllocAnaly(Size minChunkSize, Size allocChunkLimit)
{
	memset(this, 0, sizeof(AllocAnaly));
	this->minChunkSize = MAXALIGN(minChunkSize);
	this->allocChunkLimit = MAXALIGN(allocChunkLimit);
	freeListSize = AllocSetContext::AllocSetFreeIndex(allocChunkLimit);
}

// Record an allocation.
void AllocAnaly::allocRecord(Size size)
{
	if(size > allocChunkLimit)
	{
		alloced[freeListSize + 1]++;
	}else
	{
		// Accumulate the wasted size = chunk_size - size;
		int tids = AllocSetContext::AllocSetFreeIndex(size);
		Size chunk_size = minChunkSize << tids;
		alloced[tids]++;
		wasted[tids] += chunk_size - size;
	}
}

// Record a free operation.
void AllocAnaly::freeRecord(Size size)
{
	if(size > allocChunkLimit)
	{
		freed[freeListSize + 1]++;
	}else
	{
		int tids = AllocSetContext::AllocSetFreeIndex(size);
		freed[tids]++;
	}
}

// Reset the analytical info.
void AllocAnaly::reset()
{
	memset(alloced, 0, sizeof(alloced));
	memset(wasted, 0, sizeof(wasted));
	memset(freed, 0, sizeof(freed));
}

// Output the stat info.
void AllocAnaly::stat(int level)
{
	calculate();
	setColumnLength();

	Row row(6, level);

	for(int i = 0; i <= freeListSize + 3; ++i)
	{
		row.setWidth() << column[i];
	}
	row << endl;

	row.centre() << rowName[0];
	for(int i = 0; i <= freeListSize; ++i)
	{
		stringstream str;
		str << (minChunkSize << i) << "B";
		row << str.str();
	}
	stringstream str;
	str << ">" << (minChunkSize << freeListSize) << "B";
	row << str.str();
	row << "Total" << endl;;

	row.left() << rowName[1];
	for(int i = 0; i <= freeListSize + 2; ++i)
	{
		row << alloced[i];
	}
	row << endl;

	row.left() << rowName[2];
	for(int i = 0; i <= freeListSize + 2; ++i)
	{
		row << wasted[i];
	}
	row << endl;
	
	row.left() << rowName[3];
	for(int i = 0; i <= freeListSize + 2; ++i)
	{
		row << avgWasted[i];
	}
	row << endl;
	
	row.left() << rowName[4];
	for(int i = 0; i <= freeListSize + 2; ++i)
	{
		row << freed[i];
	}
	row << endl;
	
	row.left() << rowName[5];
	for(int i = 0; i <= freeListSize + 2; ++i)
	{
		ostringstream out;
		uint temp = (hitRate[i] * 100);
		out << ((double)temp / 100);
		row << out.str();
	}
	row << endl;
}

// Compute the info of the stat table.
void AllocAnaly::calculate()
{
	Size allocTotal = 0;
	Size wastedTotal = 0;
	Size freedTotal = 0;

	for(int i = 0; i <= freeListSize; ++i)
		header[i] = minChunkSize << i;
	
	for(int i = 0; i <= (freeListSize + 1); ++i)
	{
		allocTotal += alloced[i];
		wastedTotal += wasted[i];
		freedTotal += freed[i];
		alloced[i] == 0 ? avgWasted[i] = 0 : (avgWasted[i] = wasted[i] / alloced[i]);
	}

	/* hitRate */
	for(int i = 0; i <= freeListSize + 1; ++i)
	{
		alloced[i] == 0 ? hitRate[i] = 0 : (hitRate[i] = (double)alloced[i] / (double)allocTotal);
	}

	Size end = freeListSize + 2;
	alloced[end] = allocTotal;
	wasted[end] = wastedTotal;
	freed[end] = freedTotal;
	wastedTotal == 0 ? avgWasted[end] = 0 : (avgWasted[end] = wastedTotal / allocTotal);
	
	if(alloced[end - 1] == alloced[end])
	{
		hitRate[end] = 0;
	}else
	{
		hitRate[end] = (double)(allocTotal - alloced[end - 1]) / (double)allocTotal;
	}
}

// Width
void AllocAnaly::setColumnLength()
{
	initColumn();
	for(int i = 0; i <= freeListSize + 2; ++i)
	{
		if(getLength(header[i]) > column[i + 1])
			column[i + 1] = getLength(header[i]);
		if(getLength(alloced[i]) > column[i + 1])
			column[i + 1] = getLength(alloced[i]);
		if(getLength(wasted[i]) > column[i + 1])
			column[i + 1] = getLength(wasted[i]);
		if(getLength(avgWasted[i]) > column[i + 1])
			column[i + 1] = getLength(avgWasted[i]);
		if(getLength(freed[i]) > column[i + 1])
			column[i + 1] = getLength(freed[i]);
		if(hitRate[i] > 0 && column[i + 1] < 4)
			column[i + 1] = 4; // 2 digits for the hit rate **.**, so give it 4 units

	}
}

void AllocAnaly::initColumn()
{
	column[0] = 14;			/* average wasted */
	column[freeListSize + 3] = 5; 	/* total */

	for(int i = 1; i <= freeListSize + 1; ++i)
	{
		column[i] = getLength(minChunkSize << (i - 1)) + 1;
	}
	column[freeListSize + 2] = column[freeListSize + 1] + 1; 
}

// the length of a number, how many digits?
int	AllocAnaly::getLength(Size value)
{
	int length = 0;
	while(value > 0)
	{
		value = value / 10;
		length++;
	}

	return length == 0 ? 1 : length;
}

void AllocAnaly::setAllocChunkLimit(Size allocChunkLimit)
{
	this->allocChunkLimit = MAXALIGN(allocChunkLimit);
	freeListSize = AllocSetContext::AllocSetFreeIndex(allocChunkLimit);
}
