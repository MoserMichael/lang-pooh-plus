#define __PARSER_ANALYSE__
#include "grammardef.h"

int main() {
   
    bool has_cycles = ParserChecker<prog>::check(std::cout);
    if (has_cycles) {
        printf("the grammar has cycles, very bad\n");
        exit(1);
    } else {
        printf("the grammar has no cycles, very good\n");
    }
 
	return 0;
}
