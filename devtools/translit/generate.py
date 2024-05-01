#!/usr/bin/env python3

# This script generates a Unicode character transliteration
# table into sequences of individual characters.

# For each Unicode character from decimal value 128 to 0xffff,
# its transliteration is obtained using the `unidecode` library.

from unidecode import unidecode

def unicode_table():
    chars = []
    mapChar = []
    elementCnt = 0
    pos = 0
    for i in range(128, 0xffff):
        try:
            char = chr(i)
            charU = unidecode(char)
            if charU != "" :
               charField = []
               for element in charU:
                   charField.append("'" + element.replace('\\', '\\\\').replace('\'', '\\\'').replace('\"', '\\\"') + "'")

               stringlen = len(charU)
               print (f"{stringlen}, " + ", ".join(charField) + ",", end = " " )
               elementCnt = elementCnt + 1
               mapChar.append(pos);
               pos = pos + stringlen + 1
               if elementCnt % 6 == 0:
                   print("")
            else :
                mapChar.append(-1)
        except ValueError:
            mapChar.append(-1)
    return mapChar

print ('const char Data::translitTab[] = {')
l = unicode_table()
print ('};')

print('const int Data::tranlitIndexMap[] = {')
count = 0
for charMap in l:
    print(f"{charMap}, ", end = "")
    count = count + 1;
    if count % 10 == 0:
        print("")
print('};')
