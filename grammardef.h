#pragma once

#include "parse.h"

using namespace pparse;

enum Rules {

    RegexEscapedChar,
    RegexCharToken,
    RegexChar,
    RegexCharRange,
    RegexSetItems,
    RegexPositiveSet,
    RegexNegativeSet,
    RegexSet,
    RegexGroup,
    RegexEofToken,
    RegexAnyCharToken,
    RegexStarToken,
    RegexPlusToken,
    RegexPlusOrStar,
    Regex,
    RegexExpr,

    Hash_lookup_expr,
    Array_lookup_expr,
    Quoted_string_multiline,
    Literal_string_multiline,
    Identifier_lookup_expr,
    Nested_expr,
    Base_expr,

    Arithm_power,
    Unary_op,
    Arithm_unary_expr,
    Arithm_mult_expr,
    Arithm_add_expr,

    Bit_shift_expr,
    Bit_and_expr,
    Bit_xor_expr,
    Bit_or_expr,
    Comp_expr,
    Not_test_expr,
    And_test_expr,

    Test_expr,
	
	Array_val_list,
    Array_val,
    Hash_val_list,
	Hash_val,

    Rhs_expr,

	Single_val_assign_stmt,
	Multi_val_assign_stmt,
	Assign_stmt,
    GlobalVarAccess,
	StatementInner,
	StatementInnerList,
	FuncDef,
	FuncCall,
	IfStmt,
	ForeverStmt,
	ForWhileStmt,
	ForClikeStmt,
	StatementOuter,
	StatementOuterList,
};


//*** hash name (custom rule) ***

inline Char_checker_result pparse_hash_name(Char_t current_char, bool iseof, std::string &matched_so_far) {

	ssize_t slen = matched_so_far.size();
	if (iseof) {
		if (slen > 0 && current_char == ':') {
			return Char_checker_result::acceptNow;
		}
	}

	// first char.
	if (slen == 0) {
		return isalpha(current_char) ? Char_checker_result::proceed : Char_checker_result::error;
	}

	if (current_char == ':') {
		return Char_checker_result::acceptNow;
	}

	//subsequent chars
    if (isalnum(current_char) || current_char == '_')  {
        return Char_checker_result::proceed;
    }
    return Char_checker_result::error;
}

template<RuleId ruleId>
struct hash_val_name : PTokVar<ruleId, pparse_hash_name>  { 
};

inline Char_checker_result parse_quoted_str_chrs(Char_t current_char, bool iseof, std::string &matched_so_far) {
    if (current_char ==  '$' && matched_so_far.size() != 0) {
		return Char_checker_result::acceptUnget;
    }
    if (matched_so_far != "" && matched_so_far[ matched_so_far.size() - 1] == '\\') {
        matched_so_far.resize( matched_so_far.size() - 1 );
    } else if (current_char == '"') {
        return Char_checker_result::acceptUnget; 
    }
    return Char_checker_result::proceed;
}


template<RuleId ruleId>
struct QuotedStringContent: PTokVar<ruleId, parse_quoted_str_chrs>  { 
};

inline Char_checker_result parse_variable_quote_imp(Char_t current_char, bool iseof, std::string &matched_so_far, Char_t start, Char_t end) {

    if (matched_so_far == "") {
        if (current_char == '$') {
            return Char_checker_result::proceed;
        }
        return Char_checker_result::error;
    }
    if (matched_so_far == "$") {
        if (current_char == start) {
            return Char_checker_result::proceed;
        }
        return Char_checker_result::error;
    }

    if (matched_so_far.size() == 2) {
       if (isalpha(current_char)) {
            return Char_checker_result::proceed;
       }
       return Char_checker_result::error;
    }

    if (isalnum(current_char) || current_char == '_') {
       return Char_checker_result::proceed;
    }
    if (current_char == end) {
       return Char_checker_result::acceptNow;
    }

    return Char_checker_result::error;
}

inline Char_checker_result parse_variable_quote(Char_t current_char, bool iseof, std::string &matched_so_far) {

    return parse_variable_quote_imp(current_char, iseof, matched_so_far, '{', '}' );
}

inline Char_checker_result parse_command_quote(Char_t current_char, bool iseof, std::string &matched_so_far) {

    return parse_variable_quote_imp(current_char, iseof, matched_so_far, '(', ')' );
}

   
template<RuleId ruleId>
struct QuotedStringVariableQuote: PTokVar<ruleId, parse_variable_quote>  { 
};

template<RuleId ruleId>
struct QuotedStringCommandQuote : PTokVar<ruleId, parse_command_quote>  { 
};
 

inline Char_checker_result pparse_quoted_string_content(Char_t current_char, bool iseof, std::string &matched_so_far) {
    if (matched_so_far != "" && matched_so_far[ matched_so_far.size() - 1] == '\\') {
        matched_so_far.resize( matched_so_far.size() - 1 );
    } else if (current_char == '\'') {
        return Char_checker_result::acceptUnget; 
    }
    return Char_checker_result::proceed;
}

template<RuleId ruleId>
struct LiteralStringContent : PTokVar<ruleId, pparse_quoted_string_content>  { 
};
 

struct funcToken : PTok<0, CSTR4("func")> {};



/*** forward declaration of rules ***/

struct regex;

struct test_expr;

struct rhs_expr;

struct funcdefAnonymous; 

struct funccall;

struct stmt;

struct stmtOneInner;

struct stmtlistInner;

// regular expressions 


struct Metachar : PAny<0, PTok<0, CSTR1("\\")>,PTok<0, CSTR1("|")>,PTok<0, CSTR1("*")>,PTok<0, CSTR1("+")>,PTok<0, CSTR1(".")>,PTok<0, CSTR1("[")>,PTok<0, CSTR1("]")>,PTok<0, CSTR1("\\")>, PTok<0, CSTR1("`")> > {};       

struct escapedChar : PSeq<RegexEscapedChar, PTok<0, CSTR1("\\")>, PTokChar<0> > {};

struct nonMetaChar :  POnPreconditionFails< Metachar, PTokChar<RegexCharToken> > {};

struct regexChar : PAny<RegexChar, escapedChar, nonMetaChar> {};

struct range : PSeq<RegexCharRange, regexChar, PTok<0, CSTR1("-")>, regexChar > {};

struct setItems : PStar<RegexSetItems, PAny<0, range, regexChar> > {};

struct positiveSet : PSeq<RegexPositiveSet, PTok<0, CSTR1("[")>, setItems, PTok<0, CSTR1("]")> > {};

struct negativeSet : PSeq<RegexNegativeSet, PTok<0, CSTR2("[^")>, setItems, PTok<0, CSTR1("]")> > {};

struct set : PAny<RegexSet, positiveSet, negativeSet> {};

struct group : PSeq<RegexGroup, PTok<0, CSTR1("(")>, regex, PTok<0, CSTR1(")")> > {};

struct elementaryRegex : PAny<0, group, set,  PTok<RegexAnyCharToken, CSTR1(".") >, PTok<RegexEofToken, CSTR1("$")> > {};

struct starOrPlus : PAny<0, PTok<RegexPlusToken, CSTR1("+")>, PTok<RegexStarToken, CSTR1("*")> >  {};

struct regexPlusOrStar : PSeq<RegexPlusOrStar, elementaryRegex, starOrPlus> {};

struct basicRE: PAny<0, regexPlusOrStar, elementaryRegex> {};

struct regex: PSeq<Regex, basicRE, POpt<0, PTok<0, CSTR1("|")>>, regex> {};

struct poptRegex : POpt<0, PSeq<0, PTok<0, CSTR1("/")>, regex > > {};

struct regexMatch : PSeq<0, PTok<0, CSTR1("/")>, regex, poptRegex, PTok<0, CSTR1("/")> > {};

struct regex_expr : PSeq<RegexExpr, test_expr, PTok<0, CSTR2("~=")>, regexMatch> {};


// ** array / hashes **

//array_val_list :=  <rhs_expr> ( , <rhs_expr> )*
struct array_val_list : PSeq< Array_val_list, rhs_expr, PStar<0, PSeq<0, PTok<0, CSTR1(",")>, rhs_expr > > >  {};

// array_val := [ <array_val_list> ]
struct array_val : PSeq< Array_val, PTok<0, CSTR1("[")>, array_val_list,  PTok<0, CSTR1("]")> > {};

// hash_val_entry := /name:/ <rhs_expr>
struct hash_val_entry : PSeq<0, hash_val_name<0>, rhs_expr> {};

// hash_val_list = hash_val_entry ( , hash_val_entry )*
struct hash_val_list : PSeq<Hash_val_list, hash_val_entry, PStar<0, PSeq<0, PTok<0, CSTR1(",")>, hash_val_entry> > > {};

//hash_val :=  '{' <hash_val_list> '}'
struct hash_val : PSeq< Hash_val, PTok<0, CSTR1("{")>,  hash_val_list,  PTok<0, CSTR1("}")>  > {};

// ** expressions **

struct hash_lookup  : PSeq<Hash_lookup_expr, PTok<0, CSTR1("{")>, test_expr, PTok<0, CSTR1("}")> > {};

struct array_lookup : PSeq<Array_lookup_expr, PTok<0, CSTR1("[")>, test_expr, PTok<0, CSTR1("]")> > {};

struct quoted_string : PSeq<Quoted_string_multiline, PTok<0, CSTR1("\"")>, PSeq<0, PAny<0, QuotedStringVariableQuote<1>, QuotedStringCommandQuote<2>, QuotedStringContent<0>> > , PTok<0, CSTR1("\"")> >  {};

struct literal_string : PSeq<Literal_string_multiline, PTok<0, CSTR1("'")>, LiteralStringContent<0>, PTok<0, CSTR1("'")> > {};

struct identifier_cont : PAny<0, PTokIdentifierCStyle<0>, array_lookup, hash_lookup > {};

struct idenifier_lookup : PSeq<Identifier_lookup_expr, PTokIdentifierCStyle<0>, PStar<0, PSeq<0, PTok<0, CSTR1(".")>, identifier_cont> > > {};

struct nested_expr : PSeq<Nested_expr, PTok<0, CSTR1("(")>, test_expr ,  PTok<0, CSTR1(")") > > {};

struct base_expr : PAny<Base_expr, nested_expr, funccall, idenifier_lookup, quoted_string, literal_string > {};

struct arithm_unary_expr;

struct arithm_power : PSeq<Arithm_power, base_expr, POpt<Arithm_power, PSeq<0,  PTok<0, CSTR2("**")>, arithm_unary_expr > > > {}; 

struct unary_op : PAny<Unary_op, PTok<0, CSTR1("+")>, PTok<0, CSTR1("-")>, PTok<0, CSTR1("~")>> {};

struct arithm_unary_expr : PAny<Arithm_unary_expr, PSeq<0,  unary_op, arithm_unary_expr>, arithm_power> {};

struct arithm_mult_op : PAny<0, PTok<0, CSTR1("*")>, PTok<0, CSTR1("/")>, PTok<0, CSTR1("%")>> {};

struct arithm_mult_expr : PAny<Arithm_mult_expr, arithm_unary_expr,  PStar<0, PSeq<0, arithm_mult_op, arithm_unary_expr> > > {};

struct arith_add_op : PAny<0, PTok<0, CSTR1("+")>, PTok<0, CSTR1("-")> > {};

struct arithm_add_expr :  PAny<0, arithm_add_expr,  PStar<0,  PSeq<0, arith_add_op, arithm_mult_expr> > > {};

struct bit_shift_op : PAny<0, PTok<0, CSTR2("<<")>, PTok<0, CSTR2(">>")> > {};

struct bit_shift_expr :  PAny<Bit_shift_expr, arithm_add_expr,  PStar<0, PSeq<0, bit_shift_op, arithm_add_expr> > > {};

struct bit_and_expr : PAny<Bit_and_expr, bit_shift_expr,  PStar<0, PSeq<0, PTok<0, CSTR1("&")>, bit_shift_expr> > > {};

struct bit_xor_expr : PAny<0, bit_xor_expr,  PStar<0, PSeq<0, PTok<0, CSTR1("^")>, bit_and_expr> > > {};
 
struct bit_or_test_expr : PAny<Bit_or_expr, bit_xor_expr,  PStar<0, PSeq<0, PTok<0, CSTR1("|")>, bit_xor_expr> > > {};

struct comp_op : PAny<0, PTok<0, CSTR1("<")>, PTok<0, CSTR1(">")>, PTok<0, CSTR1("<")>, PTok<0, CSTR2("==")>, PTok<0, CSTR2("<=")>,  PTok<0, CSTR2(">=")>, PTok<0, CSTR2("!=")>, PTok<0, CSTR2("<>")> >  {};     
struct comp_opr_test :  PAny<Comp_expr, bit_or_test_expr, PStar<0, PSeq<0, comp_op, bit_or_test_expr, regex_expr> > > {};

struct not_test : PAny<Not_test_expr, comp_opr_test, PSeq<0, PTok<0, CSTR1("!")>, comp_opr_test> > {};

struct and_test : PAny<And_test_expr, not_test,  PStar<0, PSeq<0, PTok<0, CSTR2("&&")>, not_test> > > {};

struct test_expr : PAny<Test_expr, and_test, PStar<0, PSeq<0, PTok<0, CSTR2("||")>, and_test> > > {};

// rhs_expr := array_val | hash_val | ...
struct rhs_expr : PAny<Rhs_expr, test_expr, array_val, hash_val, funcdefAnonymous > {};

// *** statements ***

// single_val_assign_stmt PTokIdentifierCStyle := rhs_expr
struct single_val_assign_stmt : PSeq<Single_val_assign_stmt, PTokIdentifierCStyle<0>, PTok<0, CSTR1("=") >, rhs_expr> {};

// multi_val_assin_stmt := <array_val>  '=' <rhs_expr>
struct multi_val_assign_stmt : PSeq<Multi_val_assign_stmt, array_val, PTok<0, CSTR1("=") >, rhs_expr> {};

// assign_stmt := <single_val_assign_stmt> | <multi_val_assign_stmt>
struct assign_stmt : PAny<Assign_stmt, single_val_assign_stmt, multi_val_assign_stmt> {};

// funcDefParam := PTokIdentifierCStyle (, PTokIdentifierCStyle ) *
struct funcDefParams : PSeq<0, PTokIdentifierCStyle<0>, PStar<0, PSeq<0, PTok<0, CSTR1(",") >, PTokIdentifierCStyle<0> >>> {};

// funcallParamList := '(' <funcDefParams> ')'
struct funcDefParamList : PSeq<0, PTok<0, CSTR1("(")>,  funcDefParams, PTok<0, CSTR1(")")> > {}; 

// funcdef : 'func' PTokIdentifierCStyle <funcDefParamList>
struct funcdef : PSeq<FuncDef, funcToken, PTokIdentifierCStyle<0>,  funcDefParamList, stmtlistInner > {};


// funcdefAnonymous : 'func' PTokIdentifierCStyle <funcDefParamList>
struct funcdefAnonymous : PSeq<FuncDef, funcToken,  funcDefParamList, stmtlistInner > {};


// funcCallParam := rhs_expr (, rhs_expr ) *
struct funcCallParams : PSeq<0, rhs_expr, PStar<0, PSeq<0, PTok<0, CSTR1(",") >, rhs_expr >>> {};

// funCallParamList := '(' <funcCallParams> ')'
struct funCallParamList : PSeq<0, PTok<0, CSTR1("(")>,  funcCallParams, PTok<0, CSTR1(")")> > {}; 

// funccall : 'func' PTokIdentifierCStyle <funcCallParamList>
struct funccall : PSeq<FuncCall, funcToken, PTokIdentifierCStyle<0>,  funCallParamList, stmtlistInner > {};

struct stmtlistInner;

struct elifclauses : PStar<0, PSeq<0, PTok<0, CSTR4("elif") >, rhs_expr, stmtlistInner > > {};

struct ifStmt : PSeq<IfStmt,  PTok<0,CSTR2("if")>, rhs_expr, stmtlistInner, POpt<0, PSeq<0, elifclauses, PTok<0, CSTR4("else")>, stmtlistInner > > > {};

// forEver := /for/ <stmtlistInner>
struct forEverStmt : PSeq<ForeverStmt, PTok<0,CSTR3("for")>, stmtlistInner > {};

// forWhile := /for/ <stmtlistInner>
struct forWhileStmt : PSeq<ForWhileStmt, PTok<0,CSTR3("for")>, rhs_expr, stmtlistInner > {};

// forClikeStmt := /for/ <stmtlistInner>
struct forClikeStmt : PSeq<ForClikeStmt, PTok<0,CSTR3("for")>, stmtOneInner, PTok<0,CSTR1(";")>, rhs_expr , PTok<0,CSTR1(";")>,  stmtlistInner  > {};

struct globalVarAccess : PSeq<GlobalVarAccess, PTok<0, CSTR6("global")>, PTokIdentifierCStyle<0> > {};

// stmtOneInner := <assign_stmt> | <ifStmt> | <forEverStmt> | <forWhileStmt> | <forCLikeStmt> | <funcall>
struct stmtOneInner: PAny<StatementInner, assign_stmt, ifStmt, forEverStmt, forWhileStmt, forClikeStmt, funccall, globalVarAccess> {};

//stmtlistInner := '{' <stmtOneInner>* '}'
struct stmtlistInner : PSeq<StatementInnerList, PTok<0, CSTR1("{")>, PStar<0, stmtOneInner> , PTok<0, CSTR1("}")> > {};

// stmOneOuter := <funcdef> | <assign_stmt> | <ifStmt> | <forEverStmt> | <forWhileStmt> | <forClikeStmt> | <funccall>
struct stmtOneOuter : PAny<StatementOuter, funcdef, assign_stmt, ifStmt, forEverStmt, forWhileStmt, forClikeStmt, funccall > {};

struct stmtlistOuter : PSeq<StatementOuterList,  PStar<0, stmtOneOuter> > {};

struct prog : PRequireEof<stmtlistOuter> {};



