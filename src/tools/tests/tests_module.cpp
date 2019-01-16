#ifndef TESTS_MODULE_CPP
#define TESTS_MODULE_CPP

#include "../../common/com_module.h"

#define TEST_ASSERT(expression, msg) \
if (!(expression)) { printf("Failed: %s\n", msg##); } \
else { printf("Passed: %s\n", msg##); }

void Tests_Run(i32 argc, char* argv[])
{
    
}

#endif
