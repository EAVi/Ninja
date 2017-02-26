Begin Dumb legal stuff:
                All files provided are 100% made by Eric "Magnar" Villanueva. 
      Yes, even the textures; they're all mine.
                You're allowed to do whatever you want with these files except sell them or pass them 
      off as your own.
                (This games isn't impressive at all so nobody would buy it anyways).
                You are absolutely allowed to clone this project and make your own repository 
      of it, so long as your repo has a readme with a link to "https://github.com/EAVi/Ninja"
      I mean, If you have illegal intentions, this 
                document is probably not gonna stop you anyways
End Dumb legal stuff
    
    Controls:
    A - move left
    D - move right
    Spacebar - jump
    M - sword attack
    
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
