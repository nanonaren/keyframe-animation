all:
	g++ -lglut -lGLU -o keyframe *.cpp

clean:
	rm *.o keyframe
