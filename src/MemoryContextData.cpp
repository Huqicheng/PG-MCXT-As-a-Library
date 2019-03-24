#include <cassert>
#include <cstdlib>
#include <cstring>
#include "MemoryContextData.h"
#include "AllocSetContext.h"

MemoryContextData* TopMemoryContext;

MemoryContextData::MemoryContextData(MemoryContextData* prt,string mName):
	isReset(true),
	name(mName),
	parent(prt),
	firstchild(NULL),
	nextchild(NULL)
{
	/* Type-specific routine finishes any other essential initialization */
	init();

	/* OK to link node to parent (if any) */
	/* Could use MemoryContextSetParent here, but doesn't seem worthwhile */
	if (parent)
	{
		nextchild = parent->firstchild;
		parent->firstchild = this;
	}
}

void* MemoryContextData::jalloc(size_t size)
{
	isReset = false;
	return malloc(size);
}
		
void* MemoryContextData::jrealloc(void* point, size_t size)
{
	return realloc(point,size);
}

void MemoryContextData::jfree(void* point)
{
	free(point);
}
	
void MemoryContextData::jdelete()
{
	//do nothing
}

void MemoryContextData::jreset()
{
	isReset = true;
}

void MemoryContextData::jstats(int level)
{
	//do nothing
}

bool MemoryContextData::isEmpty()
{
	return isReset;
}

void MemoryContextData::init()
{
	//do nothing
}


void MemoryContextData::memoryContextInit()
{
	assert(TopMemoryContext == NULL);
	TopMemoryContext = AllocSetContext::allocSetContextCreate((MemoryContextData*) NULL,
																"TopMemoryContext",
																0,
																8 * 1024,
																8 * 1024);
}

void MemoryContextData::memoryContextReset()
{
	if(firstchild != NULL)
		memoryContextResetChildren();

	if(isReset == false)
	{
		jreset();
		isReset = true;
	}
}

void MemoryContextData::memoryContextResetChildren()
{
	MemoryContextData* child;
	for(child = firstchild; child != NULL; child = child->getNextchild())
		child->memoryContextReset();
}

void MemoryContextData::memoryContextDelete()
{
	assert(this != TopMemoryContext);

	memoryContextDeleteChildren();
	memoryContextSetParent(NULL);

	jdelete();
	
	TopMemoryContext->jfree(this);
}

void MemoryContextData::memoryContextDeleteChildren()
{
	while(firstchild != NULL)
		firstchild->memoryContextDelete();
}

void MemoryContextData::memoryContextStats()
{
	memoryContextStatsInternal(0);
}

void MemoryContextData::memoryContextStatsInternal(int level)
{
	MemoryContextData* child;

	jstats(level);
	for (child = firstchild; child != NULL; child = child->nextchild)
		child->memoryContextStatsInternal(level + 1);
}

void MemoryContextData::memoryContextSetParent(MemoryContextData* newparent)
{
	/* Delink from existing parent, if any */
	if (parent)
	{
		MemoryContextData* tparent = parent;

		if (this == tparent->firstchild)
			tparent->firstchild = nextchild;
		else
		{
			MemoryContextData* child;

			for (child = tparent->firstchild; child; child = child->nextchild)
			{
				if (this == child->nextchild)
				{
					child->nextchild = nextchild;
					break;
				}
			}
		}
	}

	/* And relink */
	if (newparent)
	{
		parent = newparent;
		nextchild = newparent->firstchild;
		newparent->firstchild = this;
	}
	else
	{
		parent = NULL;
		nextchild = NULL;
	}
}
