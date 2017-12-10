#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <iomanip>
#include "rsa.h"

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define PORT 8079

using namespace std;

mpz_class str_to_int(string str){
	mpz_class n = 0;
	for (int i = 0; i < str.length(); i++) {
		n = n * 256;
		n = n + (int)str[i];
	}
	return n;
}

string int_to_str(mpz_class n){
	mpz_class x = n;
	string str;
	while(x > 0){
		mpz_class z = x % 256;
		str.insert(0, 1, (char)(z.get_ui()));
		x = x / 256;
	}
	return str;
}

int main(int argc, char* argv[]) {
	if (argc < 3){
		std::cout << "Usage: bclient <server address> <pubkey file>" << endl;
		exit(0);
	}
	string address, pubkey_fname;
	address = argv[1];
	pubkey_fname = argv[2];

	mpz_class n, e;
	ifstream pubkey_file(pubkey_fname);
	if (!pubkey_file){
		cout << "Unable to open pubkey file" << endl;
		exit(1);
	}
	pubkey_file >> n;
	pubkey_file >> e;
	pubkey_file.close();

	string message;
	cin >> message;

	mpz_class m;
	m = str_to_int(message);
	//cout << m << endl;
	//cout << int_to_str(m) << endl;

    struct sockaddr_in adddress;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[8192];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        cerr << "socket error" << endl;
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr)<=0){
        cerr << "invalid address" << endl;
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        cerr << "connection failed" << endl;
        return -1;
    }

    gmp_randclass rng(gmp_randinit_mt);
	rng.seed(time(NULL));
	mpz_class r = 0;
	do{
		r = rng.get_z_range(n);
	} while (gcd(r, n) != 1);

	mpz_class re;
	mpz_powm(re.get_mpz_t(), r.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
	m = (m * re) % n;

    stringstream ss;
    ss << m;
    send(sock, ss.str().c_str(), ss.str().length(), 0);

    mpz_class mprim = 0;
    mpz_class temp, rd;
    stringstream strVal;
    mpz_ui_pow_ui(temp.get_mpz_t(), 10, 8192);
    while((valread = recv(sock, buffer, 8192, 0)) > 0){
    	mprim *= temp;
    	strVal << buffer;
    	strVal >> rd;
    	mprim += rd;
    	break;
    }

    mpz_invert(r.get_mpz_t(), r.get_mpz_t(), n.get_mpz_t());
    mprim *= r;

    cout << mprim <<endl;

	return 0;
}