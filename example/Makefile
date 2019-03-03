#c++11
C11=/opt/rh/devtoolset-3/root/usr/bin/
PATH:=$(C11):$(PATH)

all:
	g++ -std=c++11 -g -o2 -Wall -D_REENTRANT -D_GNU_SOURCE -g -fPIC -I/home/homework/include/ -L/home/homework/lib64/ -lpthread -lcrypto -lLruCache  example.cpp -o example

clean:
	rm -rf example
