#define __PARSER_ANALYSE__
#include "grammardef.h"


struct Opts {
    bool trace_on;
    const char *script;
} opts;

void parse_cmd(int argc, char *argv[]) {
    int c;
    
    while (optind < argc) {
      if ((c = getopt(argc, argv, "x:")) != -1) {
        // Option argument
        switch (c) {
            case 'x':
                opts.trace_on = true;
                break;
            default:
                break;
        } 
      } else {
           opts.script = argv[optind++];  // Skip to the next argument
      }       
    }

    if (opts.script == nullptr) {
        printf("error: script file missing\n");
        exit(1);
    }
}  

void visit_ast(AstEntryBase *ast) {
}

int main(int argc, char *argv[]) {
   
    bool has_cycles = ParserChecker<prog>::check(std::cout);
    if (has_cycles) {
        printf("the grammar has cycles, very bad\n");
        exit(1);
    } else {
        printf("the grammar has no cycles, very good\n");
    }

    parse_cmd(argc, argv);
 
    Text_stream text_stream;
	
	bool isok = text_stream.open(opts.script);
    if (!isok) {
        printf("error: can't open %s\n", opts.script);
        exit(1);
    }

	CharParser chparser(text_stream);
	
	Parse_result res = prog::parse(chparser);
	
	if (!res.success()) {
		printf("Error: parser error at: line: %d column: %d\n", res.get_start_pos().line(), res.get_start_pos().column());
	}

    visit_ast( res.get_ast() );
	
    return 0;

}
