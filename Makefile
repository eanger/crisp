crisp: crisp.cpp
	clang++ -g -Wall -Wextra -std=c++1y -stdlib=libc++ -o crisp crisp.cpp

.PHONY: clean

clean:
	-rm crisp *.o
