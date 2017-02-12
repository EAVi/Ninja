#Makefile for Ninja project for Linux

EXEC=Ninja
SRCS=Algorithms.cpp Block.cpp Enemy.cpp Level.cpp LTexture.cpp main.cpp Player.cpp TextWriter.cpp UIDrawer.cpp
OBJS=$(SRCS:.cpp=.o)
#libraries and stuff
SDLF=-lSDL2 -lSDL2_image
#The computers at my computer lab have SDL2 installed but do not have SDL2-image
#so for me, I'd build the libraries and link using the paths -I/pool/u/cse/SID#/SDL2-image/include/ -L/pool/u/cse/SID#/SDL2-image/lib/ 

#For macros, you'd need to define as MCRO="-D LINUX" or whatever

$(EXEC): $(OBJS)
	g++ -std=c++11 $(SDLF) -o $@ $(OBJS) $(MCRO)
#$< evaluates to the target's dependencies,
#$@ evaluates to the target

$(OBJS):
	g++ -std=c++11 -c $(SDLF)  $*.cpp $(MCRO)

clean:
	rm $(OBJS)
