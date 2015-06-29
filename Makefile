CC=gcc

# Release flags
CFLAGS=-O2 -lpthread -Ljansson -ljansson -Wl,-rpath=./jansson -I include/ -I jansson/include

# Debug flags
#CFLAGS=-g -Wall -lpthread -Ljansson -ljansson -Wl,-rpath=./jansson -I include/ -I jansson/include

SOURCES=broadcast.c connections.c contact.c hashTable.c interface.c jsonxstr.c main.c message.c running.c threadManagement.c
OBJECTS=$(addprefix obj/, $(SOURCES:.c=.o))
EXECUTABLE=Chat

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f obj/*.o ./$(EXECUTABLE)
