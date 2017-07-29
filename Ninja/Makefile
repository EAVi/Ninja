#Makefile for Ninja project for Linux
#for the libraries on debian, do "sudo apt-get install libsdl2-dev libsdl2-image-dev"
#on some others it's "sudo yum install SDL2-devel SDL2-image-devel"

#Executable name
EXEC=Ninja
#Source files
SRCS=Algorithms.cpp Enemy.cpp Game.cpp Level.cpp LTexture.cpp main.cpp Player.cpp Robot.cpp TextWriter.cpp Timer.cpp UIDrawer.cpp Zone.cpp
#Create object files from sources
OBJS=$(SRCS:.cpp=.o)
#libraries and stuff
SDLF=-lSDL2 -lSDL2_image
#The computers at my computer lab have SDL2 installed but do not have SDL2-image
#so for me, I'd build the libraries and link using the paths -I/pool/u/cse/SID#/SDL2-image/include/ -L/pool/u/cse/SID#/SDL2-image/lib/ 

#Macros are optional, you'd need to define as MCRO="-D LINUX" or whatever

$(EXEC): $(OBJS)
	g++ -std=c++11 -o $@ $(OBJS) $(SDLF) $(MCRO)
#$< evaluates to the target's dependencies,
#$@ evaluates to the target

$(OBJS):
	g++ -std=c++11 -c  $*.cpp $(SDLF) $(MCRO)

clean:
	rm $(OBJS)
