start
print/x 192
print/t 192
print/t 0x80
print/d 0x80
print/x 0b110
print/d 0b110
# Should all return 1
p "Some basic tests.."
p UTF8_CONT(128) != 0
p UTF8_2B(192) != 0
p UTF8_3B(224) != 0
p UTF8_4B(240) != 0
#
p "Testing UTF8_CONT.."
p UTF8_CONT(128 + 1) != 0
p UTF8_CONT(128 | 1) != 0
p UTF8_CONT(128 | 63) != 0
p UTF8_CONT(128 | 63) > 0
p UTF8_CONT(128 + 64) == 0
p UTF8_CONT(128 | 64) == 0
#
p "Testing UTF8_2B.."
p UTF8_2B(128 + 64) != 0
p UTF8_2B(128 | 64) != 0
p UTF8_2B(128 | 64 | 31) != 0
p UTF8_2B(128 | 64 | 31) > 0
p UTF8_2B(128 + 64 + 32) == 0
p UTF8_2B(128 | 64 | 32) == 0
#
p "Testing UTF8_3B.."
p UTF8_3B(128 + 64 + 32) != 0
p UTF8_3B(128 | 64 | 32) != 0
p UTF8_3B(128 | 64 | 32 | 15) != 0
p UTF8_3B(128 | 64 | 32 | 15) > 0
p UTF8_3B(128 + 64 + 32 + 16) == 0
p UTF8_3B(128 | 64 | 32 | 16) == 0
#
p "Testing UTF8_4B.."
p UTF8_4B(128 + 64 + 32 + 16) != 0
p UTF8_4B(128 | 64 | 32 | 16) != 0
p UTF8_4B(128 | 64 | 32 | 16 | 7) != 0
p UTF8_4B(128 | 64 | 32 | 16 | 7) > 0
p UTF8_4B(128 + 64 + 32 + 16 + 8) == 0
p UTF8_4B(128 | 64 | 32 | 16 | 8) == 0
#
p "More student tests.."
p UTF8_2B(1) == 0
p UTF8_2B(128 + 64 + 16) == 1
p UTF8_2B(128 + 64 + 32 + 16 + 8 + 4 +2 +1) == 0
p UTF8_3B(1) == 0
p UTF8_3B(128 + 64 + 32 + 8) == 1
p UTF8_3B(128 + 64 + 32 + 16 + 8 + 4 +2 +1) == 0
p UTF8_4B(1) == 0
p UTF8_4B(128 + 64 + 32 + 16 + 4) == 1
p UTF8_4B(128 + 64 + 32 + 16 + 8 + 4 +2 +1) == 0
p UTF8_CONT(1) == 0
p UTF8_CONT(128 + 32) == 1
p UTF8_CONT(128 + 64 + 32 + 16 + 8 + 4 +2 +1) == 0
# Quits GDB
q
