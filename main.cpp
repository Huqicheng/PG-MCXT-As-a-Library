#include <iostream>
#include <cstdlib>
#include "MemoryContextData.h"
#include "AllocSetContext.h"

using namespace std;

int main()
{
	MemoryContextData::memoryContextInit();

	MemoryContextData* myContext = 
				AllocSetContext::allocSetContextCreate(TopMemoryContext, "myContext", 1000, 4000, 32* 1024);

	MemoryContextData* errorContext = 
				AllocSetContext::allocSetContextCreate(TopMemoryContext, "errorContext", 1024, 2048, 8092);

	MemoryContextData* normalErrorContext =
				AllocSetContext::allocSetContextCreate(errorContext, "normalErrorContext", 1024, 2048, 4096);

	
	int times = 10000;
	while(times--)
	{
		int memory_size = rand() % (8192);
		myContext->jalloc(memory_size);
	}
	
	void* memory = myContext->jalloc(1024);
	void* error = errorContext->jalloc(2098);
	void* normalerror = normalErrorContext->jalloc(1025);

	errorContext->jfree(error);
	
	cout << "-----------------------------All contexts are children of TopMemoryContext--------------------------------"<< endl;
	TopMemoryContext->memoryContextStats();

	cout << "-----------------------------context stat info------------------------------------"<< endl;
	myContext->jstats(0);

	myContext->jreset();
	cout << "-----------------------------after reseting--------------------------------------"<< endl;
	myContext->jstats(0);
	
	myContext->jdelete();
	cout << "-----------------------------after deleting the context--------------------------------------"<< endl;
	myContext->jstats(0);

	return 0;
}
