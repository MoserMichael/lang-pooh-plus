
OBJ_DIR:=.obj-gcc
COMP:=gcc
EXE:=plan-gcc

CPP_OPTS=-g -std=c++17  -pedantic -Wall -Wshadow -Werror 
LD_OPTS=-lstdc++ -lgtest -lpthread 

-include Makefile.common

