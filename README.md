
# the plan

current unix shells don't work well with structured data; i think that a next gen shell needs to focus on structured data - hacks like jq and yq need to be made redundant as being part of the shell. 

The shell needs to be paired with an interpreted programming language; the following requirements are identified:

- shell must work with structured data, so it needs built in support for arrays and hash maps.
- ability to trace execution: bash has set -x to trace execution of a script; think that is very important.
- still keep the ability to execute shell commands organically (that's why it is a shell ;-); so the syntax of the language can't be too complicated.

problems:

environment variables only work as strings; how do you export an array or hash? 
In bash you can't export an array, as it is hard to have an equivalent representation as an environment variable. [link](https://stackoverflow.com/questions/5564418/exporting-an-array-in-bash-script). I think that array/hashes should be exported as structured text (yaml or json or xml, depending on the type of export)


# the problem to solve

Bash can work with structured data by using utilities like jq and yq, and the bash shell scripting language can work with hashes and arrays, but it is a stretch due to the following reasons:

- the bash scripting language shows its age
- I managed to write a reasonably complicated bash programs that work/don't work on slightly different versions of the shell
- complicated bash scripts look awkward and out of place; i think the shell scripting language is just designed to glue togather programs in cases that the unix pipeline doesn't cut it.
- needs slightly more practical support for expression with numbers.
- performant: bash is quite slow; probably needs to be a bit faster.
- bash scripting language syntax is strictly LALR (it uses yacc) so that parsing is fast; but i think that's too limiting in this day and age (hope that parsing doesn't get too slow for my tools) 

# required features

a set of features that identify the niche of a shell scripting language

- lack of typing
- forgiving syntax: a line is either a shell command or a statement of the language (can't have a special syntax for running commands); the difference between command and scripting line must be obvious on first glance.
- tracability: set -x (or equivalent command) needs to trace the execution of the script (i personally think that this is the killer feature of shell languages)
- all shell scripting languages examined by me seem to be implemented as AST walking interpreters; (i don't know think that's a must)

a set of fatures requirements for the scripting language in order to work better with structured data:

- built-in parsing of json/yaml (maybe xml at some stage) into a nested structure of arrays and hashes.
- ability to work with structured data/traverse the stuff.
- ability to write more complicated scripts
    * better support for scripting libraries: it needs an import statement (simplified version of what python has; i think that python import has too many features)
- spome support of objects. i thing a javascrip like object system with a hash that can have lambda field values is enough for this case.
- a nice base syntax. I think that it should/could combine an eclectic set of language features;
    * from golang: base syntax for most statements (i don't like the indenation business of python) (with exclusion of go structures & interfaces for lack of typing)
    * from python: import statement
    * from perl: regex integration
    * from shell/perl: here documents vs literal strings.
    * relatively simple expressions syntax: i think i will adap test (without the if..else form) from [here](https://docs.python.org/3.6/reference/grammar.html) 
- typing: duct typing with nuances a(
    * use of undeclared variables is not allowed (I like what python is doing). Bash can do that as an option; i think it should be the only option.
    * a variable can be treated as a number if it conforms to the number syntax.
    * a variable can have only one type (can't use the same variable name both as a string and a hash)
    * i don't like the prefix thing: (don't want $variable for use of variable in a script, but will use that form for here documents/quoted string)
- need builtin regular expressions

# other stuff

- reference counting garbage collection?
- no left recursion?
- green threads? co-routines?

# some details

i think one simplify here documents a bit:
- all quoted strings start with " and can span multiple lines.
- within a quoted string you can have ${Var} - this one looks up a scalar variable Var
- within a quoted string you can have $(Var} - this one uses the value of scalar variable Var and shells out, take the standard output and standard error the result and inserts it into the string
- global variable access from functions should be like in python.
