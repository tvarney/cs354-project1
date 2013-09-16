###########################################################
# Project 1 Makefile
SRC := ./src
INC := ./inc
CXX := g++

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
CPPFLAGS := -Wall -ggdb -D__MAC__ -I${INC}
LFLAGS := -Wall
LIBS := -framework OpenGL -framework GLUT -lpthread -lpng
else
CPPFLAGS := -Wall -ggdb -I${INC}
LFLAGS := -Wall
LIBS := -lglut -lGLU -lGL -lpthread -lm -lpng
endif

FLAGS := ${CPPFLAGS} ${LFLAGS}

#INCLUDE = -I/usr/include
#LIBDIR = -L/usr/lib/x86_64-linux-gnu
# Libraries that use native graphics hardware --
#LIBS = -lglut -lGLU -lGL -lpthread -lm

OBJECTS=$(patsubst %.cpp, %.o, $(wildcard ${SRC}/*.cpp))

.PHONEY: all clean

all: canvas

clean:
	rm -f ${SRC}/*.o canvas ${SRC}/lex.yy.c ${SRC}/wavefront.tab.h ${SRC}/wavefront.tab.c

canvas: ${OBJECTS} ${SRC}/parse.o ${SRC}/lex.o
	${CXX} ${LFLAGS} -o canvas ${OBJECTS} ${LIBS} ${SRC}/lex.o ${SRC}/parse.o

${SRC}/lex.o: ${SRC}/lex.yy.c
	${CXX} ${FLAGS} -c -I${SRC} -o ${SRC}/lex.o ${SRC}/lex.yy.c ${LIBS}

${SRC}/parse.o: ${SRC}/wavefront.tab.c
	${CXX} ${FLAGS} -c -I${SRC} -o ${SRC}/parse.o ${SRC}/wavefront.tab.c ${LIBS}

${SRC}/wavefront.tab.c: ${SRC}/wavefront.y
	bison -o ${SRC}/wavefront.tab.c ${SRC}/wavefront.y

${SRC}/lex.yy.c: ${SRC}/wavefront.l
	flex -o ${SRC}/lex.yy.c ${SRC}/wavefront.l
