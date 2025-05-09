#!/usr/bin/python
from PIL import Image
import struct, sys

print("[convert-fonttiles] converting file " + sys.argv[1] + " to binary file " + sys.argv[2])

tilesheetPNG = Image.open(sys.argv[1])
tilesheetBIN = open(sys.argv[2], "wb")
tilesheetPixels = tilesheetPNG.load()

tilesheetX, tilesheetY = tilesheetPNG.size
horizTiles = int(tilesheetX/8)
vertTiles  = int(tilesheetY/8)
totalTiles = horizTiles*vertTiles

def generateTile(x,y,image):
    tile = []
    for py in range(0,8):
        row = []
        for px in range(0,8):
            pixelvalues = image[(8*x)+px,(8*y)+py]
            try:
                r,g,b,a = pixelvalues
            except ValueError:
                r,g,b = pixelvalues
            row.append(((r+g+b)>384))
        tile.append(sum([int(row[b])*(2**(7-b)) for b in range(0,8)]))
    return tile

generatedTileData = []
for y in range(0,vertTiles):
    for x in range(0,horizTiles):
        generatedTileData += generateTile(x,y,tilesheetPixels)
        
tilesheetBIN.write(bytearray(generatedTileData))

print("[convert-fonttiles] conversion complete.")