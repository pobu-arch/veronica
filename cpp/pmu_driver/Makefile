COMPILER := gcc

FLAGS := -O3 -g -ggdb

LFLAGS := -lm -lstdc++ -I${VERONICA}/cpp

BINARIES := pmu_read
OBJECTS  := driver.o

.PHONY: all

%.o: %.cpp
    $(COMPILER) $(FLAGS) -I${VERONICA}/cpp -c $(input) -o $(output)

all: $(OBJECTS)
	$(COMPILER) -o $(BINARIES) $(FLAGS) $(LFLAGS)

clean:	
	rm -f *.o $(BINARIES)
