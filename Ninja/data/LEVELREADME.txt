How to make a level, by Magnar

Note that when I use the word "currently",
I mean "as of 1:25 AM 5/22/2017"
------------------------------

Creating a level has 5 parts:
	1) Level attributes
	2) Backgrounds
	3) Blocks
	4) Enemies
	5) Doors

	You'll probably notice that in the text files, there 
are a bunch of numbers in there from 0-255. That's 8 bits. 
This game runs on a lot of 8-bit logic, because that's
what I was going for. Every structure in a level is also 
constructed with 8-bit numbers. 
	So, if you put any number larger than 255 in these 
text files, the computer will either poop itself or will 
truncate some bits. Just don't do it, kay?

Separating the 5 parts:
		When you want to signal the level loader 
	that you want to go on to the next part, put a 255
	as a signal, and the next part will begin, and also
	put a 255 at the end of file, just because.
		NOTE that the level attributes are 
	an exception, they do not need the 255 signal.
		
	IE: If you put 255 as the first argument to any 
	    of the parts, it will ignore that byte and skip 
	    to the next part. Except for Part 1.

Parsing/Separating your numbers:
		There are only two parse characters, SPACE
	and newline, these can be used to separate your numbers
	from the next numbersanything else will not work, no 
	commas, no semicolons.

-----------------------------
1) Level Attributes (8 arguments, 8 bytes)
	Level Width - the number of blocks you want to fit on the x axis
	Level Heigth - width, but on y axis.
	Ambient Red - will adjust lighting, does not work yet
	Ambient Green - ""
	Ambient Blue - ""
	Player Spawn X - Only required on the first level of the zone*
	Player Spawn Y - ""
	Song - check the SFX/MUS folder to see what you like.

	*The spawn position is a respawn point if you fall off the map,
	it is also the starting position for the first level.
-----------------------------
2) Backgrounds (8 arguments,  10 bytes)
	Texture Number - there's currently 11 bgs to choose from:
		 0 - "GFX/BG/bluesky.png",
		 1 - "GFX/BG/snowmtn.png",
		 2 - "GFX/BG/grasshills.png",
		 3 - "GFX/BG/nightynight.png",
		 4 - "GFX/BG/skyline.png",
		 5 - "GFX/BG/redsky.png",
		 6 - "GFX/BG/snow.png",
		 7 - "GFX/BG/castlehill.png",
		 8 - "GFX/BG/castlewall.png",
		 9 - "GFX/BG/stormclouds.png",
		10 - "GFX/BG/moonskull.png",
		11 - "GFX/BG/beachhill.png",
		12 - "GFX/BG/seasky.png",
	X position - is 16 bits, takes 2 bytes of memory
		   - first part of X position is Coarse.
		   - second part is fine position.
	Y position - "" Note that the origin is the top-left corner
	Depth - Parallax, how much the background follows the camera.
		255 means the background follows completely, 
		0 means the background does not follow at all.
	X Velocity - Signed 8-bit integer, every frame will move this direction
		   - will move that much regardless of depth
	Y Velocity - "" 
		   - Remember that positive y velocity will go downward 
	TileX - 1 bit, second-rightmost bit, if true, the 
		background will repeat on the x axis.
	TileY - 1 bit, rightmost bit, y axis.
------------------------------
3) Blocks (5 arguments, 4 bytes)
	Block Number - there are currently 9 blocks that have 
			been textured, to see which block represents 
			what number, look at the GFX/BlockSheet.png
	Block X - This will be the X position of the block, 
		  multiplied by the tile size.
	Block Y - ""
	TileDimensions Y - 4 bits, four leftmost bits of the last bit*
	TileDimensions X - 4 bits, four rightmost bits of the last bit*
	
	       *imagine the byte in binary form (add 1) and split it into two parts:
		0000 0000
		Y    X
		You can tile up to 16x16 blocks. 
		"0" will give you a 1x1 tile
		"15" will give you a 16x1 tile.
		"16" will give you a 1x16 tile.
		"255" will give you a 16x16 tile.
-----------------------------
4) Enemies (3 arguments, 3 bytes)
	Enemy Type - there are currently two types of enemy:
			0 - Robo Pirate
			1 - Space Ghost
	X Position - as usual, multiplied by tilesize
	Y Position - ""
-----------------------------
5) Door (7 arguments, 7 bytes)
*note that a door is invisible and doesn't actually look like a door
*portal or warp is more accurate, I guess.
	Collision Box(4 bytes)
		X - multiplied by tilesize
		Y - ""
		Width - ""
		Height - ""
	Destination Level - if you're currently in "debug_0.txt" and 
				want to go to "debug_1.txt", just put "1"
				as the number.
	Destination X - X position on the destination 
			level you'd like to go to*
	Destination Y - Y ""

	*For Destination X or Y, if you don't want to specify a 
	 position, just put 255.
-----------------------------
CLOSING NOTES:
-Every level you make should start with debug_
 followed by a number, folowed by .txt
-if you have a gap in the numbering, any number 
 after that gap will be ignored. For example:
	debug_0.txt
	debug_1.txt
	debug_3.txt
	debug_4.txt
 Since debug_2.txt is missing, 3 and 4 will 
	not be counted.
	

Alright, have fun making maps!
Aww who am I kidding, I wrote this for myself, not for some nonexistant audience.
Alright, have fun making maps!