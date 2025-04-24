main: thalia_ysassi_project3.o
	g++ thalia_ysassi_project3.o -o main

thalia.ysassi-project3.o: thalia_ysassi_project3.cpp
	g++ -c thalia_ysassi_project3.cpp

clean:
	rm *.o main