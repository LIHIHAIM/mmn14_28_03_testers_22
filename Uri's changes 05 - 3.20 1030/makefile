main: main.o interface.h
	$(CC) -g $(CFLAGS) $(CPPFLAGS) main.c -o main
compiler.o: compiler.c ineterface.h files.h line.h util.h label.h instructionHandeling.h dataHandeling.h symbolTable.h statmentType.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) compiler.c -o compiler.o
buildWord.o: buildWord.o buildWord.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) buildWord.c -o buildWord.o
dataHandeling.o: util.h word.h label.h privateDataFuncs.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) dataHandeling.c -o dataHandeling.o
files: files.h util.h privateDataFuncs.h privateInstFuncs.h privateSymTabFuncs.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) files.c -o files.o
instructionHandeling.o: instructionStat.h label.h util.h word.h buildWord.h privateInstFuncs.h privateSymTabFuncs.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) instructionHandeling.c -o instructionHandeling.o
label.o: label.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) label.c -o label.o
line.o: line.h util.h label.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS)line.c -o line.o
statmentType.o: bool.h statmentType.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) statmentType.c -o statmentType.o
symbolTable.o: symbolTable.h util.h label.h privateSymTabFuncs.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) symbolTable.c -o symbolTable.o
util.o: util.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) util.c -o util.o

clean :
	rm -f *.o 