#!/bin/csh -f

rm -r -f tmp && mkdir tmp && cd tmp
git clone --depth 1 -b 2023/main git@github.com:mdaus/coda-oss.git
rm -r -f coda-oss/.git

cd ..
rm -r -f externals/coda-oss
mv tmp/coda-oss externals
rm -r -f externals/coda-oss/.git
rmdir tmp
