Begin Dumb legal stuff
               -Don't steal my stuff. Everything here is my stuff -> Dont steal anything here.
               -Give credit where it is due, reused code and other work should link back to this page:
                  https://github.com/EAVi/Ninja
End Dumb legal stuff
    
    Controls:
    A - move left
    D - move right
    Spacebar - jump
    M - sword attack
    
    Debug Controls:
    (Tilde key) - enable debug features
    ESC(while debug is active) - spawn a ghost
    
    To compile on Linux:
    -Get the sdl2 development libraries.
    -Get the sdl2-image development libraries.
    -Use the makefile.
    
    To compile on Windows:
    -I use Visual Studio 2015 with Git extension, I compile using the SDL2 and SDL2-image x86 libraries. 
    You can compile however you want though.
    -Figure it out yourself lol.
    
    
    Misc:
    If you look at the files, you'll probably notice a directory "data" with a file "debug.txt"
    the file contains a bunch of 8-bit numbers. This file is actually the map file that is loaded in order to construct the map. 
    In essence, it's just an array 8-bit numbers used to read in arguments of background objects and block tile objects.
    Eventually I plan on making a map builder/compiler, but for now you'll have to think like a computer.
    Take a look at Level::Loadmap() in Level.cpp to understand how it works. 
      Remember that the X,Y coordinate system begins at the top-left of the screen.
    Any feedback on the clarity of my comments are appreciated. 
    
    This project was begun on 25 Aug, 2016 and has been worked on on/off since. 
    As of 2 Feb, 2017, this project is ~2000 lines of code (with like 99% blank lines)
    
    I've tested the files in both Windows, Debian. You'll need to download the 
        SDL2 and SDL2-image libraries in order to compile. This game will eventually use SDL2-mixer, 
        so you might be interested in that as well. 
