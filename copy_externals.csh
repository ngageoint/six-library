#!/bin/csh -f

rm -r -f tmp && mkdir tmp && cd tmp
git clone --depth 1 -b 2023/main git@github.com:mdaus/coda-oss.git
rm -r -f coda-oss/.git

git clone --depth 1 -b 3.0/main git@github.com:mdaus/nitro.git
rm -r -f nitro/.git
rm -r -f nitro/externals

cd ..

rm -r -f externals/coda-oss
mv tmp/coda-oss externals
rm -r -f externals/nitro
mv tmp/nitro externals

rmdir tmp
