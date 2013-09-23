###########################################################
# Project 1 Makefile
SRC  := ./src
INC  := ./inc
CXX  := g++
CC   := g++
LEX  := flex
YACC := bison

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
CPPFLAGS := -Wall -ggdb -D__MAC__ -I${INC} -I${SRC}
LINKFLAGS := -Wall
LIBS := -framework OpenGL -framework GLUT -lpthread -lpng
else
CPPFLAGS := -Wall -ggdb -I${INC} -I${SRC}
LINKFLAGS := -Wall
LIBS := -lglut -lGLU -lGL -lpthread -lm -lpng
endif

CFLAGS := ${CPPFLAGS}

#INCLUDE = -I/usr/include
#LIBDIR = -L/usr/lib/x86_64-linux-gnu
# Libraries that use native graphics hardware --
#LIBS = -lglut -lGLU -lGL -lpthread -lm
#LIBS = -lglut -lMesaGLU -lMesaGL

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard ${SRC}/*.cpp))
LEXERS = $(patsubst %.l, %.lex.c, $(wildcard ${SRC}/*.l))
PARSERS = $(patsubst %.y, %.tab.c, $(wildcard ${SRC}/*.y))
PARSER_HEADERS = $(patsubst %.c, %.h, ${PARSERS})
OBJECTS += $(patsubst %.c, %.o, ${LEXERS})
OBJECTS += $(patsubst %.c, %.o, ${PARSERS})

.PHONEY: all clean run lines

all: canvas

clean:
	rm -f ${SRC}/*.o canvas ${PARSERS} ${PARSER_HEADERS} ${LEXERS}

run: canvas
	./canvas

lines:
	@wc -l ${SRC}/*.cpp ${SRC}/*.l ${SRC}/*.y ${INC}/*.hpp ${INC}/generic/*.hpp

canvas: ${PARSERS} ${LEXERS} ${OBJECTS}
	@echo ${OBJECTS}
	${CXX} ${LINKFLAGS} -o canvas ${OBJECTS} ${LIBS}

%.tab.c: %.y
	${YACC} -o $@ ${YACCFLAGS} $<

%.lex.c: %.l
	${LEX} -o $@ ${LEXFLAGS} $<
