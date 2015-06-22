CPPFLAGS = -g -Wall -Wextra -std=c++1y -stdlib=libc++ -I.

OBJS = $(patsubst %.cpp,%.o,$(wildcard *cpp))

%.o: %.cpp
	clang++ $(CPPFLAGS) -c -o $@ $<

crisp: $(OBJS)
	clang++ -g -Wall -Wextra -std=c++1y -stdlib=libc++ -o crisp $+

.PHONY: clean

clean:
	-rm crisp *.o
