#! /bin/sh

if [ `uname -s` = "Linux" ]; then
 echo Linux 32
 gcc -m32 -fPIC -shared -o tinyfiledialogsLinux86.so ../tinyfiledialogs.c
 gcc -m32 -o hello ../hello.c ./tinyfiledialogsLinux86.so

 echo Linux 64
 gcc -m64 -fPIC -shared -o tinyfiledialogsLinux64.so ../tinyfiledialogs.c
 gcc -m64 -o hello ../hello.c ./tinyfiledialogsLinux64.so
 echo Linux 64 static
 gcc -m64 -fPIC -c -o tinyfiledialogsLinux64.o ../tinyfiledialogs.c
 ar rcs tinyfiledialogsLinux64.a tinyfiledialogsLinux64.o
elif [ `uname -s` = "OpenBSD" ]; then
 echo OpenBSD
 clang -m32 -fPIC -shared -o tinyfiledialogsOpenBSDx86.so ../tinyfiledialogs.c
 clang -m32 -o hello ../hello.c ./tinyfiledialogsOpenBSDx86.so

 clang -m64 -fPIC -shared -o tinyfiledialogsOpenBSDx64.so ../tinyfiledialogs.c
 clang -m64 -o hello ../hello.c ./tinyfiledialogsOpenBSDx64.so
else
 echo Other Unix
fi
