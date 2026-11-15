/* A small PNG reader for the optional rock, paper and scissors pictures:
 * non-interlaced, 8 bits per channel (grey, grey+alpha, RGB, RGBA) or
 * indexed at 1, 2, 4 or 8 bits with an optional tRNS. Inflate after
 * zlib's puff.c, with the output size known in advance from IHDR. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "png.h"

#define MAX_SIDE 1024

typedef struct {
    const uint8_t *in, *end;
    unsigned bitbuf, bitcnt;
    uint8_t *out;
    size_t len, cap;
    int err;
} Inflate;

typedef struct { short count[16], symbol[320]; } Huffman;

static int bits(Inflate *s, int need)
{
    unsigned v = s->bitbuf;
    while (s->bitcnt < (unsigned)need) {
        if (s->in >= s->end) { s->err = 1; return 0; }
        v |= (unsigned)*s->in++ << s->bitcnt;
        s->bitcnt += 8;
    }
    s->bitbuf = v >> need;
    s->bitcnt -= need;
    return (int)(v & ((1u << need) - 1));
}

static int build(Huffman *h, const short *length, int n)
{
    short offs[16];
    int len, symbol, left = 1;
    for (len = 0; len < 16; len++) h->count[len] = 0;
    for (symbol = 0; symbol < n; symbol++) h->count[length[symbol]]++;
    if (h->count[0] == n) return 0;
    for (len = 1; len < 16; len++) {
        left <<= 1;
        left -= h->count[len];
        if (left < 0) return -1;
    }
    offs[1] = 0;
    for (len = 1; len < 15; len++) offs[len + 1] = offs[len] + h->count[len];
    for (symbol = 0; symbol < n; symbol++)
        if (length[symbol]) h->symbol[offs[length[symbol]]++] = (short)symbol;
    return left;
}

static int decode(Inflate *s, const Huffman *h)
{
    int code = 0, first = 0, index = 0, len, count;
    for (len = 1; len < 16; len++) {
        code |= bits(s, 1);
        count = h->count[len];
        if (code - count < first) return h->symbol[index + (code - first)];
        index += count;
        first += count;
        first <<= 1;
        code <<= 1;
    }
    s->err = 1;
    return -1;
}

static const short LBASE[29] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
static const short LEXT[29] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
static const short DBASE[30] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
static const short DEXT[30] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

static int codes(Inflate *s, const Huffman *lencode, const Huffman *distcode)
{
    int symbol, len;
    unsigned dist;
    for (;;) {
        symbol = decode(s, lencode);
        if (s->err || symbol < 0) return -1;
        if (symbol < 256) {
            if (s->len >= s->cap) return -1;
            s->out[s->len++] = (uint8_t)symbol;
        } else if (symbol == 256) {
            return 0;
        } else {
            symbol -= 257;
            if (symbol >= 29) return -1;
            len = LBASE[symbol] + bits(s, LEXT[symbol]);
            symbol = decode(s, distcode);
            if (s->err || symbol < 0 || symbol >= 30) return -1;
            dist = (unsigned)(DBASE[symbol] + bits(s, DEXT[symbol]));
            if (dist > s->len || s->len + (size_t)len > s->cap) return -1;
            while (len--) { s->out[s->len] = s->out[s->len - dist]; s->len++; }
        }
    }
}

static int fixed(Inflate *s)
{
    static Huffman lencode, distcode;
    static int ready;
    if (!ready) {
        short lengths[288];
        int symbol;
        for (symbol = 0; symbol < 144; symbol++) lengths[symbol] = 8;
        for (; symbol < 256; symbol++) lengths[symbol] = 9;
        for (; symbol < 280; symbol++) lengths[symbol] = 7;
        for (; symbol < 288; symbol++) lengths[symbol] = 8;
        build(&lencode, lengths, 288);
        for (symbol = 0; symbol < 30; symbol++) lengths[symbol] = 5;
        build(&distcode, lengths, 30);
        ready = 1;
    }
    return codes(s, &lencode, &distcode);
}

static int dynamic(Inflate *s)
{
    static const short ORDER[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
    short lengths[320];
    Huffman lencode, distcode;
    int nlen = bits(s, 5) + 257, ndist = bits(s, 5) + 1, ncode = bits(s, 4) + 4, index, err;
    if (s->err || nlen > 286 || ndist > 30) return -1;
    for (index = 0; index < ncode; index++) lengths[ORDER[index]] = (short)bits(s, 3);
    for (; index < 19; index++) lengths[ORDER[index]] = 0;
    if (build(&lencode, lengths, 19) != 0) return -1;
    index = 0;
    while (index < nlen + ndist) {
        int symbol = decode(s, &lencode), len = 0;
        if (s->err || symbol < 0) return -1;
        if (symbol < 16) { lengths[index++] = (short)symbol; continue; }
        if (symbol == 16) {
            if (index == 0) return -1;
            len = lengths[index - 1];
            symbol = 3 + bits(s, 2);
        } else if (symbol == 17) {
            symbol = 3 + bits(s, 3);
        } else {
            symbol = 11 + bits(s, 7);
        }
        if (index + symbol > nlen + ndist) return -1;
        while (symbol--) lengths[index++] = (short)len;
    }
    if (lengths[256] == 0) return -1;
    err = build(&lencode, lengths, nlen);
    if (err < 0 || (err > 0 && nlen - lencode.count[0] != 1)) return -1;
    err = build(&distcode, lengths + nlen, ndist);
    if (err < 0 || (err > 0 && ndist - distcode.count[0] != 1)) return -1;
    return codes(s, &lencode, &distcode);
}

/* A zlib stream into exactly `cap` bytes; nonzero when it filled them. */
static int unzlib(const uint8_t *in, size_t size, uint8_t *out, size_t cap)
{
    Inflate s;
    int last, type;
    if (size < 2 || (in[0] & 15) != 8 || (in[1] & 0x20)) return 0;
    memset(&s, 0, sizeof s);
    s.in = in + 2;
    s.end = in + size;
    s.out = out;
    s.cap = cap;
    do {
        last = bits(&s, 1);
        type = bits(&s, 2);
        if (s.err) return 0;
        if (type == 0) {
            unsigned len;
            s.bitbuf = 0;
            s.bitcnt = 0;
            if (s.end - s.in < 4) return 0;
            len = s.in[0] | (unsigned)s.in[1] << 8;
            if ((s.in[2] | (unsigned)s.in[3] << 8) != (~len & 0xFFFF)) return 0;
            s.in += 4;
            if ((size_t)(s.end - s.in) < len || s.len + len > s.cap) return 0;
            memcpy(s.out + s.len, s.in, len);
            s.in += len;
            s.len += len;
        } else if (type == 1) {
            if (fixed(&s)) return 0;
        } else if (type == 2) {
            if (dynamic(&s)) return 0;
        } else {
            return 0;
        }
    } while (!last);
    return s.len == cap;
}

static uint32_t be32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

static int paeth(int a, int b, int c)
{
    int p = a + b - c, pa = abs(p - a), pb = abs(p - b), pc = abs(p - c);
    return pa <= pb && pa <= pc ? a : pb <= pc ? b : c;
}

int Png_Decode(const uint8_t *data, size_t size, PngImage *image)
{
    static const uint8_t SIGNATURE[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    uint32_t w = 0, h = 0, palette[256];
    int depth = 0, type = -1, channels = 0, have_header = 0, x, y;
    uint8_t *idat = NULL, *raw = NULL;
    size_t idat_len = 0, pos = 8, stride, bpp;
    memset(image, 0, sizeof *image);
    for (x = 0; x < 256; x++) palette[x] = 0xFF000000u;
    if (size < 8 || memcmp(data, SIGNATURE, 8)) return 0;

    while (pos + 12 <= size) {
        uint32_t len = be32(data + pos);
        const uint8_t *tag = data + pos + 4, *body = data + pos + 8;
        if (len > size - pos - 12) break;
        if (!memcmp(tag, "IHDR", 4) && len >= 13) {
            w = be32(body);
            h = be32(body + 4);
            depth = body[8];
            type = body[9];
            if (body[12] != 0) goto fail;   /* interlaced */
            have_header = 1;
        } else if (!memcmp(tag, "PLTE", 4)) {
            for (x = 0; x < 256 && (uint32_t)(x * 3 + 2) < len; x++)
                palette[x] = 0xFF000000u | (uint32_t)body[x * 3] << 16 | (uint32_t)body[x * 3 + 1] << 8 | body[x * 3 + 2];
        } else if (!memcmp(tag, "tRNS", 4) && type == 3) {
            for (x = 0; x < 256 && (uint32_t)x < len; x++)
                palette[x] = (palette[x] & 0xFFFFFFu) | (uint32_t)body[x] << 24;
        } else if (!memcmp(tag, "IDAT", 4)) {
            uint8_t *more = realloc(idat, idat_len + len);
            if (!more) goto fail;
            idat = more;
            memcpy(idat + idat_len, body, len);
            idat_len += len;
        } else if (!memcmp(tag, "IEND", 4)) {
            break;
        }
        pos += 12 + len;
    }
    if (!have_header || !idat || w == 0 || h == 0 || w > MAX_SIDE || h > MAX_SIDE) goto fail;
    switch (type) {
    case 0: channels = 1; break;
    case 2: channels = 3; break;
    case 3: channels = 1; break;
    case 4: channels = 2; break;
    case 6: channels = 4; break;
    default: goto fail;
    }
    if (type == 3 ? (depth != 1 && depth != 2 && depth != 4 && depth != 8) : depth != 8) goto fail;

    stride = ((size_t)w * channels * depth + 7) / 8;
    bpp = (size_t)(channels * depth + 7) / 8;
    raw = malloc((stride + 1) * h);
    image->pixels = malloc((size_t)w * h * 4);
    if (!raw || !image->pixels || !unzlib(idat, idat_len, raw, (stride + 1) * h)) goto fail;

    for (y = 0; y < (int)h; y++) {
        uint8_t *row = raw + (size_t)y * (stride + 1), *line = row + 1;
        uint8_t *prev = y ? row - stride : NULL;
        int filter = row[0];
        size_t i;
        for (i = 0; i < stride; i++) {
            int a = i >= bpp ? line[i - bpp] : 0, b = prev ? prev[i] : 0, c = prev && i >= bpp ? prev[i - bpp] : 0;
            switch (filter) {
            case 0: break;
            case 1: line[i] = (uint8_t)(line[i] + a); break;
            case 2: line[i] = (uint8_t)(line[i] + b); break;
            case 3: line[i] = (uint8_t)(line[i] + ((a + b) >> 1)); break;
            case 4: line[i] = (uint8_t)(line[i] + paeth(a, b, c)); break;
            default: goto fail;
            }
        }
        for (x = 0; x < (int)w; x++) {
            uint32_t argb;
            const uint8_t *p = line + (size_t)x * channels;
            switch (type) {
            case 0: argb = 0xFF000000u | (uint32_t)p[0] * 0x010101u; break;
            case 2: argb = 0xFF000000u | (uint32_t)p[0] << 16 | (uint32_t)p[1] << 8 | p[2]; break;
            case 4: argb = (uint32_t)p[1] << 24 | (uint32_t)p[0] * 0x010101u; break;
            case 6: argb = (uint32_t)p[3] << 24 | (uint32_t)p[0] << 16 | (uint32_t)p[1] << 8 | p[2]; break;
            default: {
                int per = 8 / depth, shift = 8 - depth * (x % per) - depth;
                int index = (line[x / per] >> shift) & ((1 << depth) - 1);
                argb = palette[index];
            }
            }
            image->pixels[(size_t)y * w + x] = argb;
        }
        /* The next row's filter reads this one as `prev`, unfiltered: keep it
         * where it is (the loop above only wrote in place). */
    }
    image->width = (int)w;
    image->height = (int)h;
    free(raw);
    free(idat);
    return 1;

fail:
    free(raw);
    free(idat);
    free(image->pixels);
    memset(image, 0, sizeof *image);
    return 0;
}
