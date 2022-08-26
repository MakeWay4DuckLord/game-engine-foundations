main: main.o
	g++ main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm -f main.o sfml-app
