#!/usr/bin/env bash

# Exit immediately if any command below fails.
set -e

make


echo "Generating a test_files directory.."
mkdir -p test_files
rm -f test_files/*


echo "Generating test files.."
printf "Hello, World!\n" > test_files/ascii.input
printf "Hello, World!" > test_files/ascii2.input
printf "{Hello, World}" > test_files/ascii3.input
printf "{Hello@, World@" > test_files/ascii4.input
printf "{Hello ~ World}" > test_files/ascii5.input
printf "Hello****World!\n" > test_files/ascii6.input
printf "Hello ---- World!\n" > test_files/ascii7.input
printf "Hello >< World!\n" > test_files/ascii8.input
printf "Hello [] World!\n" > test_files/ascii9.input
printf "Hello #### World!\n" > test_files/ascii10.input
printf "Hello ; : ; World!\n" > test_files/ascii11.input
printf "Hello ||| World!\n" > test_files/ascii12.input
printf "Hello ^^ World!\n" > test_files/ascii13.input
printf "Hello,\x7D World!\n" > test_files/ascii14.input
printf "Hello,\x00World!\n" > test_files/data.input
printf "Hello,\x01 World!\n" > test_files/data2.input
printf "Hello,\x02 World!\n" > test_files/data3.input
printf "Hello,\x03 World!\n" > test_files/data4.input
printf "Hello,\x0E World!\n" > test_files/data5.input
printf "Hello,\x04 World!\n" > test_files/data6.input
printf "Hello,\x1D World!\n" > test_files/data7.input
printf "Hello,\x19 World!\n" > test_files/data8.input
printf "Hello,\x05 World!\n" > test_files/data9.input
printf "Hello,\x06 World!\n" > test_files/data10.input
printf "Hello,\x18 World!\n" > test_files/data11.input
printf "Hello,\x17 World!\n" > test_files/data12.input
printf "Hello,\x16 World!\n" > test_files/data13.input
printf "Hello,\x15 World!\n" > test_files/data14.input
printf "Hello,\x14 World!\n" > test_files/data15.input
printf "Hello,\x13 World!\n" > test_files/data16.input
printf "" > test_files/empty.input
printf "Hello,\xC6 World!\n" > test_files/ISO.input
printf "Hello,\xBD World!\n" > test_files/ISO2.input
printf "Hello,\xC1 World!\n" > test_files/ISO3.input
printf "Hello,\xC2 World!\n" > test_files/ISO4.input
printf "Hello,\xC3 World!\n" > test_files/ISO5.input
printf "Hello,\xC4 World!\n" > test_files/ISO6.input
printf "Hello,\xC5 World!\n" > test_files/ISO7.input
printf "Hello,\xA2 World!\n" > test_files/ISO8.input
printf "Hello,\xA3 World!\n" > test_files/ISO9.input
printf "Hello,\xA4 World!\n" > test_files/ISO10.input
printf "Hello,\xA5 World!\n" > test_files/ISO11.input
printf "Hello,\xA6 World!\n" > test_files/ISO12.input
printf "Hello,\xA7 World!\n" > test_files/ISO13.input
printf "Hello,\xA8 World!\n" > test_files/ISO14.input
printf "Hello,\xA9 World!\n" > test_files/ISO15.input
printf "Hello,\xAA World!\n" > test_files/ISO16.input
printf "Hello,\xAB World!\n" > test_files/ISO17.input
printf "Hello,\xAC World!\n" > test_files/ISO18.input
printf "Hello,\xAD World!\n" > test_files/ISO19.input
printf "Hello,\xAE World!\n" > test_files/ISO20.input
printf "Hello,\xAF World!\n" > test_files/ISO21.input
printf "Hello,\xB1 World!\n" > test_files/ISO22.input
printf "Hello,\xB2 World!\n" > test_files/ISO23.input
printf "Hello,\xB3 World!\n" > test_files/ISO24.input
printf "Hello,\xB4 World!\n" > test_files/ISO25.input
printf "Hello,\xB5 World!\n" > test_files/ISO26.input
printf '\xFE\xFF\x00' > test_files/UTF-16BE.input
printf '\xFE\xFF\x00\xDD' > test_files/UTF-16BE2.input
printf '\xFE\xFF\x00\xAA' > test_files/UTF-16BE3.input
printf '\xFE\xFF\x00\xBB' > test_files/UTF-16BE4.input
printf '\xFE\xFF\x00\xCC' > test_files/UTF-16BE5.input
printf '\xFE\xFF\x00\xEE' > test_files/UTF-16BE6.input
printf '\xFE\xFF\x00\xA1' > test_files/UTF-16BE7.input
printf '\xFE\xFF\x00\xA2' > test_files/UTF-16BE8.input
printf '\xFE\xFF\x00\xA3' > test_files/UTF-16BE9.input
printf '\xFE\xFF\x00\xA4' > test_files/UTF-16BE10.input
printf '\xFE\xFF\x00\xA5' > test_files/UTF-16BE11.input
printf '\xFE\xFF\x00\xA6' > test_files/UTF-16BE12.input
printf '\xFE\xFF\x00\xA7' > test_files/UTF-16BE13.input
printf '\xFE\xFF\x00\xA8' > test_files/UTF-16BE14.input
printf '\xFF\xFE\x00' > test_files/UTF-16LE.input
printf '\xFF\xFE\x00\xAA' > test_files/UTF-16LE2.input
printf '\xFF\xFE\x00\xBB' > test_files/UTF-16LE3.input
printf '\xFF\xFE\x00\xCC' > test_files/UTF-16LE4.input
printf '\xFF\xFE\x00\xDD' > test_files/UTF-16LE5.input
printf '\xFF\xFE\x00\xEE' > test_files/UTF-16LE6.input
printf '\xFF\xFE\x00\xA1' > test_files/UTF-16LE7.input
printf '\xFF\xFE\x00\xA2' > test_files/UTF-16LE8.input
printf '\xFF\xFE\x00\xA3' > test_files/UTF-16LE9.input
printf '\xFF\xFE\x00\xA4' > test_files/UTF-16LE10.input
printf '\xFF\xFE\x00\xA5' > test_files/UTF-16LE11.input
printf '\xFF\xFE\x00\xA6' > test_files/UTF-16LE12.input
printf '\xFF\xFE\x00\xA7' > test_files/UTF-16LE13.input
printf '\xFF\xFE\x00\xA8' > test_files/UTF-16LE14.input
printf "\xC2\xA2" > test_files/UTF-8.input
printf "\xC2\xA3" > test_files/UTF-8-2.input
printf "\xC2\xA4" > test_files/UTF-8-3.input
printf "\xC2\xA5" > test_files/UTF-8-4.input
printf "\xC2\xA6" > test_files/UTF-8-5.input
printf "\xC2\xA7" > test_files/UTF-8-6.input
printf "\xC2\xA8" > test_files/UTF-8-7.input
printf "\xC2\xA9" > test_files/UTF-8-8.input
printf "\xC2\xAA" > test_files/UTF-8-9.input
printf "\xC2\xAB" > test_files/UTF-8-10.input
printf "\xC2\xAC" > test_files/UTF-8-11.input
printf "\xC2\xAD" > test_files/UTF-8-12.input
printf "\xC2\xAE" > test_files/UTF-8-13.input
printf "\xC2\xAF" > test_files/UTF-8-14.input
printf "\xE0\xA0\x80" > test_files/UTF-8-15.input
printf "\xE0\xA0\x81" > test_files/UTF-8-16.input
printf "\xE0\xA0\x82" > test_files/UTF-8-17.input
printf "\xE0\xA0\x83" > test_files/UTF-8-18.input
printf "\xE0\xA0\x84" > test_files/UTF-8-19.input
printf "\xE0\xA0\x85" > test_files/UTF-8-20.input
printf "\xE0\xA0\x86" > test_files/UTF-8-21.input
printf "\xE0\xA0\x87" > test_files/UTF-8-22.input
printf "\xE0\xA0\x88" > test_files/UTF-8-23.input
printf "\xE0\xA0\x89" > test_files/UTF-8-24.input
#printf "\xF0\x90\x8C\xb0" > test_files/UTF-8-25.input
### TODO: Generate more test files ###


echo "Running the tests.."
exitcode=0
g=not_a_valid_path
f=test_files/*.input
echo ">>> Testing ${f}.."
file    ${f} | sed 's/ASCII text.*/ASCII text/' | sed 's/Little-endian UTF-16 Unicode text.*/Little-endian UTF-16 Unicode text/' | sed 's/Big-endian UTF-16 Unicode text.*/Big-endian UTF-16 Unicode text/' | sed 's/UTF-8 Unicode text.*/UTF-8 Unicode text/' > test_files/expected
./file  ${f} > test_files/actual

if ! diff -u test_files/expected test_files/actual
then
  echo ">>> Failed :-("
  exitcode=1
else
  echo ">>> Success :-)"
fi

exit $exitcode
