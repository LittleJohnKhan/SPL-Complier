NAME = spl

LLVM_CONFIG = /usr/local/bin/llvm-config

NO_WARNING =  -Wno-return-type \
	-Wno-c++11-compat-deprecated-writable-strings \
	-Wno-deprecated-register \
	-Wno-switch \

CXXFLAGS = `$(LLVM_CONFIG) --cppflags` -std=c++11 $(NO_WARNING)
LDFLAGS = `$(LLVM_CONFIG) --ldflags`
LIBS = `$(LLVM_CONFIG) --libs --system-libs`

OBJS = parser.o tokenizer.o ast.o main.o 

all : $(NAME)

parser.cpp: ${NAME}.y
	bison -d -o parser.cpp ${NAME}.y

parser.hpp: parser.cpp

tokenizer.cpp: ${NAME}.l
	flex -o tokenizer.cpp ${NAME}.l

%.o: %.cpp ast.h CodeGenContext.h utils.h ccalc.h
	g++ -c $(CXXFLAGS) -g -o $@ $< 

$(NAME): $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

.PHONY: clean
clean:
	-rm -f *.o
	-rm -f parser.hpp parser.cpp tokenizer.cpp spl