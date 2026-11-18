from PIL import Image, ImageFilter
jpal = Image.open("res/jefe1.png").getpalette()
pimg = Image.new("P", (16, 16)); pimg.putpalette(jpal)
im = Image.open("res/enemigo_nuevo.jpg").convert("RGB"); px = im.load()
mk = Image.new("L", im.size, 0)
for y in range(im.height):
    for x in range(im.width):
        r, g, b = px[x, y]
        if r + g + b > 26 and not (g > r + 25 and y < im.height * 0.25): mk.putpixel((x, y), 255)
box = mk.getbbox(); im, mk = im.crop(box), mk.crop(box)
cw, ch = im.size; sc = min(44 / cw, 39 / ch)
nw, nh = max(16, int(cw * sc)), max(16, int(ch * sc))
ims = im.resize((nw, nh), Image.LANCZOS)
mks = mk.resize((nw, nh), Image.BILINEAR).point(lambda p: 255 if p > 96 else 0)
cr, cm = Image.new("RGB", (48, 48), (0,0,0)), Image.new("L", (48, 48), 0)
ox, oy = (48 - nw) // 2, 7
cr.paste(ims, (ox, oy)); cm.paste(mks, (ox, oy))
halo, core = set(), set()
for tx in (24 - 4, 24 + 4):
    for dy, wh, wc in [(0,1,1), (1,2,1), (2,2,1), (3,1,0), (4,1,0), (5,0,0)]:
        if oy - dy >= 1:
            for dx in range(-wh, wh + 1):
                if 1 <= tx + dx < 47:
                    halo.add((tx + dx, oy - dy)); cm.putpixel((tx + dx, oy - dy), 255)
                    if abs(dx) <= wc and dy <= 3: core.add((tx + dx, oy - dy))
bd = cm.filter(ImageFilter.MaxFilter(3))
out = Image.new("P", (48, 48), 0); out.putpalette(jpal)
q = cr.quantize(palette=pimg, dither=Image.Dither.NONE)
for y in range(48):
    for x in range(48):
        if (x, y) in core: out.putpixel((x, y), 15)
        elif (x, y) in halo: out.putpixel((x, y), 14)
        elif cm.getpixel((x, y)) > 0: c = q.getpixel((x, y)); out.putpixel((x, y), c if 1 < c < 14 else 2)
        elif bd.getpixel((x, y)) > 0: out.putpixel((x, y), 1)
out.save("res/enemigo.png"); print("ENEMIGO_48_RESTAURADO!")
