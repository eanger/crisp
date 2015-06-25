CPPFLAGS = -g -Wall -Wextra -std=c++1y -stdlib=libc++ -I.
SRC = \
	  eval.cpp \
	  read.cpp \
	  value.cpp

TEST = $(wildcard tests/*cpp)
MAIN = main.cpp

OBJ = $(patsubst %.cpp,%.o,$(SRC))
TEST_OBJ = $(patsubst %.cpp,%.o,$(TEST))
MAIN_OBJ = $(patsubst %.cpp,%.o,$(MAIN))

%.o: %.cpp
	clang++ $(CPPFLAGS) -c -o $@ $<

crisp: $(MAIN_OBJ) $(OBJ)
	clang++ -g -Wall -Wextra -std=c++1y -stdlib=libc++ -o $@ $+

.PHONY: clean check

test: $(TEST_OBJ) $(OBJ)
	clang++ -g -Wall -Wextra -std=c++1y -stdlib=libc++ -o $@ $+

check: test
	./test

clean:
	-rm test crisp *.o tests/*.o
