#Makefile for Ninja project for Linux
#for the libraries on debian, do "sudo apt-get install libsdl2-dev libsdl2-image-dev"
#on some others it's "sudo yum install SDL2-devel SDL2-image-devel"

#Executable name
EXEC=Ninja
#Source files
SRCS=Algorithms.cpp Cannon.cpp Cutscene.cpp Demon.cpp Enemy.cpp Game.cpp LAudio.cpp Level.cpp LevelID.cpp LTexture.cpp main.cpp Menu.cpp Player.cpp Robot.cpp Samurai.cpp TextWriter.cpp Timer.cpp UIDrawer.cpp Zone.cpp
#Create object files from sources
OBJS=$(SRCS:.cpp=.o)
#libraries and stuff
SDLF=-lSDL2 -lSDL2_image -lSDL2_mixer

#Macros are optional, you'd need to define as MCRO="-D LINUX" or whatever

$(EXEC): $(OBJS)
	g++ -std=c++11 -o $@ $(OBJS) $(SDLF) $(MCRO)
#$< evaluates to the target's dependencies,
#$@ evaluates to the target

$(OBJS):
	g++ -std=c++11 -c  $*.cpp $(SDLF) $(MCRO)

clean:
	rm $(OBJS)
