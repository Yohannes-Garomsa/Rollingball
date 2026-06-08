all:
	g++ main.cpp -o app -lGL -lGLU -lglut

run:
	./app

clean:
	rm -f app
