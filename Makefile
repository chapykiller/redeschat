CC=gcc
CFLAGS=-lpthread -ljansson -I include/ -c
CFLAGS2=-lpthread -ljansson -I include/
SOURCES=connections.c contact.c hashTable.c interface.c jsonxstr.c message.c main.c
OBJECTS=$(addprefix obj/, $(SOURCES:.c=.o))
EXECUTABLE=Chat

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS2) $(OBJECTS) -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f obj/*.o ./$(EXECUTABLE)
