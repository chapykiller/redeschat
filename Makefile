CC=gcc
CFLAGS=-I include/ -c
SOURCES=connectionListener.c main.c
OBJECTS=$(addprefix obj/, $(SOURCES:.c=.o))
EXECUTABLE=Chat

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f obj/*.o ./$(EXECUTABLE)
