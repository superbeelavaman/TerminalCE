# TerminalCE
Serial console for the TI 84 Plus CE
When running, sets USB port be seen as a USB-Serial adapter by a host device.
on Linux systems running systemd, run  
    `# systemctl start serial-getty@ttyUSB0.service`  
    to start a terminal through the emulated serial port on the calc.

## Planned features
40 column terminal (hopefully)  
30 rows, top 2 and bottom 2 rows being used for status bars with black on white text  
  
example UI:
```
+----------------------------------------+
| Window Title                           |
|                                        |
| ... actual console area ...            |
|                                        |
| 01, 17                                 |
+----------------------------------------+
```

for colours, or together colour bits from 4 bit rgbi input: (setting color not properly implemented yet)  
```
RRRBBGGG Hex Name
10100000 A0  Red
00000101 05  Green
00010000 10  Blue
01001010 4A  Intensity
```
Keyboard: (not implemented yet)  
(top: key label, left: shift state)  

|    | f1    | f2    | f3    | f4    | f5    |
|----|-------|-------|-------|-------|-------|
|0   |       |       |       |       |       |
|^   |       |       |       |       |       |
|A   |       |       |       |       |       |
|a   |       |       |       |       |       |
|    | 14    | 13    | 12    | 11    | 10    |

|    | 2nd   | mode  | del   | left  | up    |
|----|-------|-------|-------|-------|-------|
|0   |       |       | del   |       |       |
|^   |       | ^C    | ins   |       |       |
|A   |       |       |       |       |       |
|a   |       |       |       |       |       |
|    | 15    | 16    | 17    | 71    | 73    |

|    | alpha | XT0n  | stat  | down  | right |
|----|-------|-------|-------|-------|-------|
|0   |       |       |       |       |       |
|^   |       |       |       |       |       |
|A   |       | /     |       |       |       |
|a   |       |       |       |       |       |
|    | 27    | 37    | 47    | 70    | 72    |

|    | math  | apps  | prgm  | vars  | clear |
|----|-------|-------|-------|-------|-------|
|0   |       |       |       |       |       |
|^   |       |       |       |       |       |
|A   | A     | B     | C     |       |       |
|a   | a     | b     | c     |       |       |
|    | 26    | 36    | 46    | 56    | 66    |

|    | x^-1  | sin   | cos   | tan   | ^     |
|----|-------|-------|-------|-------|-------|
|0   |       |       |       |       | ^     |
|^   |       |       |       |       | pi    |
|A   | D     | E     | F     | G     | H     |
|a   | d     | e     | f     | g     | h     |
|    | 25    | 35    | 45    | 55    | 65    |

|    | x^2   | ,     | (     | )     | /     |
|----|-------|-------|-------|-------|-------|
|0   |       | ,     | (     | )     | /     |
|^   |       |       | {     | }     |       |
|A   | I     | J     | K     | L     | M     |
|a   | i     | j     | k     | l     | m     |
|    | 24    | 34    | 44    | 54    | 64    |

|    | log   | 7     | 8     | 9     | *     |
|----|-------|-------|-------|-------|-------|
|0   |       | 7     | 8     | 9     | *     |
|^   |       | &     | *     |       | [     |
|A   | N     | O     | P     | Q     | R     |
|a   | n     | o     | p     | q     | r     |
|    | 23    | 33    | 43    | 53    | 63    |

|    | ln    | 4     | 5     | 6     | -     |
|----|-------|-------|-------|-------|-------|
|0   |       | 4     | 5     | 6     | -     |
|^   |       | $     | %     | ^     | ]     |
|A   | S     | T     | U     | V     | W     |
|a   | s     | t     | u     | v     | w     |
|    | 22    | 32    | 42    | 52    | 62    |

|    | sto-> | 1     | 2     | 3     | +     |
|----|-------|-------|-------|-------|-------|
|0   |       | 1     | 2     | 3     | +     |
|^   |       | !     | @     | #     |       |
|A   | X     | Y     | Z     | theta | "     |
|a   | x     | y     | z     |       |       |
|    | 21    | 31    | 41    | 51    | 61    |

|    | on    | 0     | .     | (-)   | enter |
|----|-------|-------|-------|-------|-------|
|0   |       | 0     | .     |       |       |
|^   |       | ~     | <     | >     |       |
|a   |       | space | ;     | ?     |       |
|A   |       | space | :     | !     |       |
|    |       | 30    | 40    | 50    | 60    |