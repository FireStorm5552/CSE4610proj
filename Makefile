Proj: main.o sdisk.o filesys.o
	g++ main.o sdisk.o filesys.o -o Proj

filesys.o:
	g++ -c filesys.cpp

sdisk.o:
	g++ -c sdisk.cpp
	
main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o
