CRC_POLY = 0x04C11DB7

def precompute_table(bits):
    lookup_table = []
    for i in xrange(2**bits):
        rr = i << (32 - bits)
        for x in xrange(bits):
            if rr & 0x80000000:
                rr = (rr << 1) ^ CRC_POLY
            else:
                rr <<= 1
        lookup_table.append(rr & 0xffffffff)
    return lookup_table

lookup_table = precompute_table(8)

def process_word(data, crc=0xffffffff):
    if (len(data) < 4):
        # The CRC data is "padded" in a very unique and confusing fashion.
        data = data[::-1] + '\0' * (4 - len(data))

    for char in reversed(data):
        b = ord(char)
        crc = ((crc << 8) ^ lookup_table[(crc >> 24) ^ b]) & 0xffffffff
    return crc

def process_buffer(buf, c=0xffffffff):
    word_count = (len(buf) + 3) / 4

    crc = c
    for i in xrange(word_count):
        crc = process_word(buf[i * 4 : (i + 1) * 4], crc)
    return crc

def crc32(data):
    return process_buffer(data)

if __name__ == '__main__':
    import sys

    assert(0x89f3bab2 == process_buffer("123 567 901 34"))
    assert(0xaff19057 == process_buffer("123456789"))
    assert(0x519b130 == process_buffer("\xfe\xff\xfe\xff"))
    assert(0x495e02ca == process_buffer("\xfe\xff\xfe\xff\x88"))

    print "All tests passed!"

    if len(sys.argv) >= 2:
        b = open(sys.argv[1]).read()
        crc = crc32(b)
        print "%u or 0x%x" % (crc, crc)
