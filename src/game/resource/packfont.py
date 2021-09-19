from sys import argv

from PIL import Image

img = Image.open(argv[1])
pixmap = img.load()
width = img.size[0]
height = img.size[1]

if width != 65 or height != 37:
    raise Exception("Expected 65x37px font file.")


def isWhite(rgb):
    r, g, b = rgb
    return r > 127 or g > 127 or b > 127


data = []
for letter in range(32, 128):
    img_x = 1 + 4 * ((letter - 32) % 16)
    img_y = 1 + 6 * ((letter - 32) // 16)
    letter_short = 0
    bitpos = 32768
    for y in range(5):
        for x in range(3):
            pixel = pixmap[img_x + x, img_y + y]
            if isWhite(pixel):
                letter_short |= bitpos
            bitpos >>= 1
    data.append(hex(letter_short))
    if len(data) == 16:
        print(", ".join(data) + ",")
        data = []
