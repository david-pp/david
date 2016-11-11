#!/bin/bash 


dd if=/dev/urandom of=test.data bs=1M count=1

tarfile=test.data

rm $tarfile.*

echo "-------zip"
time zip $tarfile.zip $tarfile;
rm $tarfile
ls -lh $tarfile.zip
time unzip $tarfile.zip

echo "--------gzip"
time gzip $tarfile
ls -lh $tarfile.gz
time gzip -d $tarfile.gz

echo "--------bzip2"
time bzip2 $tarfile
ls -lh $tarfile.bz2
time bzip2 -d $tarfile.bz2

echo "--------xz"
time xz $tarfile
ls -lh $tarfile.xz
time xz -d $tarfile.xz

echo "--------7za"
time 7za a $tarfile.7z $tarfile
rm $tarfile
ls -lh $tarfile.7z
time 7za e $tarfile.7z
