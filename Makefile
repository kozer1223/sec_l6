all: rsatest bserver bclient bvrfy

rsatest: rsa.cpp rsa.h rsatest.cpp
	g++ -o rsatest rsatest.cpp rsa.cpp -lgmpxx -lgmp

bserver: bserver.cpp rsa.cpp rsa.h
	g++ -o bserver bserver.cpp rsa.cpp -lgmpxx -lgmp -lssl -lcrypto -std=c++11 

bclient: bclient.cpp rsa.cpp rsa.h
	g++ -o bclient bclient.cpp rsa.cpp -lgmpxx -lgmp -std=c++11

bvrfy: bvrfy.cpp rsa.cpp rsa.h
	g++ -o bvrfy bvrfy.cpp rsa.cpp -lgmpxx -lgmp -std=c++11