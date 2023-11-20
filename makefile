cg03: main.o
	gcc main.o -lGL -lGLU -lglut -lm -o SpriteAnimation

main.o: main.cpp
	gcc -c main.cpp

clean:
	rm *.o SpriteAnimation