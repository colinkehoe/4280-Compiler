CC = g++
CFLAGS = -std=c++11 -Wall

TARGET = P5
OBJS = main.o scanner.o parser.o testTree.o semantics.o cg.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp scanner.h parser.h testTree.h semantics.h cg.h
	$(CC) $(CFLAGS) -c main.cpp

scanner.o: scanner.cpp scanner.h token.h
	$(CC) $(CFLAGS) -c scanner.cpp

parser.o: parser.cpp parser.h token.h scanner.h
	$(CC) $(CFLAGS) -c parser.cpp

testTree.o: testTree.cpp testTree.h parser.h
	$(CC) $(CFLAGS) -c testTree.cpp

semantics.o: semantics.cpp semantics.h parser.h
	$(CC) $(CFLAGS) -c semantics.cpp

cg.o: cg.cpp cg.h semantics.h parser.h
	$(CC) $(CFLAGS) -c cg.cpp

clean:
	rm -f *.o $(TARGET)
