from PIL import Image

colors = [
    0xFF000000,
    0xFF555555,
    0xFFAAAAAA,
    0xFFFFFFFF,
    0xFF5555FF,
    0xFF0000AA,
    0xFF0055AA,
    0xFF55FFFF,
    0xFF55FF55,
    0xFF00AA00,
    0xFFFFFF55,
    0xFFAAAA00,
    0xFFFF55FF,
    0xFFFF5555,
    0xFFAA00AA,
    0xFFAA0000,
]

palette = {((c >> 0) & 255, (c >> 8) & 255, (c >> 16) & 255): i for i, c in enumerate(colors)}


def toColorIndex(c):
    return palette[c]


img = Image.open("sprites.png")
# print("loaded image ({}x{})".format(*img.size))
pixmap = img.load()
width = img.size[0]
height = img.size[1]
data = []
for y in range(height):
    for x in range(0, width, 2):
        p1 = toColorIndex(pixmap[x, y])
        p2 = toColorIndex(pixmap[x + 1, y])
        c = (p1 << 4) | p2
        data.append(hex(c))

print(", ".join(data))
