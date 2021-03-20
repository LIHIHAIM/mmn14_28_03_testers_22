main: main.o interface.h
	$(CC) -g $(CFLAGS) $(CPPFLAGS) -o main
compiler.o: compiler.c ineterface.h files.h line.h util.h label.h instructionHandeling.h dataHandeling.h symbolTable.h statmentType.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o compiler.o
buildWord.o: buildWord.o buildWord.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o buildWord.o


clean :
	rm -f *.o 