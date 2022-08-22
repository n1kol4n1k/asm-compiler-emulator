SRCS = \
./src/asmMain.cpp \
./src/asmManager.cpp \
./src/asmSymTable.cpp \
./src/asmRelocTable.cpp \
parser.c \
lexer.c

DEBUG = 1
DEBUG_FLAGS = 
ifeq ($(DEBUG), 1)
DEBUG_FLAGS+= -fdiagnostics-color=always -g
endif

all: $(SRCS)
	g++ $(DEBUG_FLAGS) $(SRCS) -o asembler

lexer.c: ./misc/lexer.l
	flex ./misc/lexer.l

parser.c: ./misc/parser.y ./misc/lexer.l
	bison -d ./misc/parser.y

clean:
	rm -rf lexer.c lexer.h parser.c parser.h asmMonka