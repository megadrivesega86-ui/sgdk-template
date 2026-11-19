from PIL import Image

im = Image.open("res/powerup.png")
w, h = im.size
pal = im.getpalette()

cols = [(pal[i*3], pal[i*3+1], pal[i*3+2]) for i in range(16)]
c_dark = min(range(1, 16), key=lambda i: sum(cols[i]))
c_bright = max(range(1, 16), key=lambda i: sum(cols[i]))
c_gold = max(range(1, 16), key=lambda i: cols[i]*2 + cols[i] - cols[i])

# Ampliar tira horizontal de 576x24 a 1008x24 (3 nuevas armas x 6 frames x 24px = +432px)
out = Image.new("P", (w + 432, h), 0)
out.putpalette(pal)
out.paste(im, (0, 0))

letras = [
    # W (Wide 5-vias)
    ["##.....##", "##.....##", "##..#..##", "##.###.##", "###...###", ".##...##.", "..#...#.."],
    # L (Laser Rapido)
    ["###......", "###......", "###......", "###......", "###......", "#########", "#########"],
    # M (Misiles Teledirigidos)
    ["##.....##", "###...###", "####.####", "##.###.##", "##..#..##", "##.....##", "##.....##"]
]

for idx_w, pat in enumerate(letras):
    for f in range(6):
        sx = (idx_w * 6 + f) * 24
        dx0 = w + (idx_w * 6 + f) * 24
        base = im.crop((sx, 0, sx + 24, 24))
        out.paste(base, (dx0, 0))
        for dy in range(6, 18):
            for dx in range(6, 18):
                if out.getpixel((dx0 + dx, dy)) != 0:
                    out.putpixel((dx0 + dx, dy), c_dark)
        fg = c_bright if (f % 2 == 0) else c_gold
        for ry, row in enumerate(pat):
            for rx, ch in enumerate(row):
                if ch == '#':
                    out.putpixel((dx0 + 7 + rx, 8 + ry), fg)

out.save("res/powerup.png")
print("ARMAS_SPRITE_OK:", out.size)
