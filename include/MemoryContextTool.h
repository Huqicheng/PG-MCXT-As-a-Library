#ifndef MEMORYCONTEXTTOOL_H
#define MEMORYCONTEXTTOOL_H

#define TYPEALIGN(ALIGNVAL,LEN)  \
	(((intptr_t) (LEN) + ((ALIGNVAL) - 1)) & ~((intptr_t) ((ALIGNVAL) - 1)))

#define MAXALIGN(LEN)			TYPEALIGN(4, (LEN))

#endif