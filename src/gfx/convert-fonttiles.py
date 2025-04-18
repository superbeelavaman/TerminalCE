#!/usr/bin/python
from PIL import Image
import struct

tilesheetPNG = Image.open("TermFont.png")
tilesheetBIN = open("TermFont.bin", "wb")
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
            r,g,b,a = image[(8*x)+px,(8*y)+py]
            row.append(((r+g+b)>384))
        tile.append(sum([int(row[b])*(2**(7-b)) for b in range(0,8)]))
    return tile

generatedTileData = []
for y in range(0,vertTiles):
    for x in range(0,horizTiles):
        generatedTileData += generateTile(x,y,tilesheetPixels)
        
tilesheetBIN.write(bytearray(generatedTileData))
