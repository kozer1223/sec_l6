#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <fstream>
#include <random>
#include <openssl/sha.h>
#include <sstream>
#include <string>
#include <iomanip>

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define PORT 8079

#include "rsa.h"

using namespace std;

string random_str(const int len) {
	string alphanum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnoprstuvwxyz";
	string str;

	random_device r;
	default_random_engine generator(r());
	uniform_int_distribution<int> distribution(0,alphanum.length() - 1);

	for (int i = 0; i < len; i++) {
		str.append(1, alphanum[distribution(generator)]);
	}
	return str;
}

string sha256(const string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// calculates x^n mod m in constant time
mpz_class powmod_consttime(mpz_class x, mpz_class n, mpz_class m){
	mpz_class res = 1;
	while (n > 0){
		int s = (n % 2 == 1);
		res = (res * x * s + res * (1-s)) % m;
		x = (x * x) % m;
		n = n / 2;
	}
	return res;
}


void server(mpz_class n, mpz_class d, mpz_class e) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[8192];
      
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket");
        exit(1);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(1);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(1);
    }
    int pid;
    while(true){
    	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        	perror("accept");
        	exit(1);
    	}

    	if ((pid = fork()) == -1){
    		close(new_socket);
    		continue;
    	} else if (pid > 0) {
    		close(new_socket);
    		continue;
    	} else if (pid == 0) {
    		mpz_class m = 0;
 			mpz_class temp, rd;
    		stringstream strVal;
    		mpz_ui_pow_ui(temp.get_mpz_t(), 10, 8192);
    		while((valread = recv(new_socket, buffer, 8192, 0)) > 0){
    			m *= temp;
    			strVal << buffer;
    			strVal >> rd;
    			m += rd;
    			break;
    		}

  			gmp_randclass rng(gmp_randinit_mt);
			rng.seed(time(NULL));
			mpz_class r = 0;
			do{
				r = rng.get_z_range(n-2) + 1;
			} while (gcd(r, n) != 1);

			mpz_class re;
    		//mpz_powm_sec(re.get_mpz_t(), r.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
    		re = powmod_consttime(r, e, n);
    		//m *= re;
    		m = (m * re) % n;
    		//mpz_powm_sec(m.get_mpz_t(), m.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
    		m = powmod_consttime(m, d, n);
    		mpz_invert(r.get_mpz_t(), r.get_mpz_t(), n.get_mpz_t());
    		m = (m * r) % n;
    		//m = m % n;

    		//cout << m << endl;

    		stringstream ss;
    		ss << m;
    		send(new_socket, ss.str().c_str(), ss.str().length(), 0);
    	}

    }
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage: bserver setup <keysize> or bserver sign" << endl;
		exit(0);
	}
	string arg = argv[1];
	if (arg == "setup"){
		if (argc < 3) {
			std::cout << "Usage: bserver setup <keysize>" <<endl;
			exit(0);
		}
		int keysize = 0;
		stringstream strVal;
		strVal << argv[2];
		strVal >> keysize;
		if (keysize < 32){
			std::cout << "Invalid key size" << endl;
			exit(0);
		}

		RSA rsa(keysize);

		ofstream privkey_file;
		privkey_file.open("privkey", ios::trunc);
		privkey_file << rsa.n << endl;
		privkey_file << rsa.d;
		privkey_file.close();

		ofstream pubkey_file;
		pubkey_file.open("pubkey", ios::trunc);
		pubkey_file << rsa.n << endl;
		pubkey_file << rsa.e;
		pubkey_file.close();

		string pass = random_str(16);
		cout << "Password:" << endl;
		cout << pass << endl;

		ofstream hash_file;
		hash_file.open("hash", ios::trunc);
		hash_file << sha256(pass) << endl;
		hash_file.close();
	} else if (arg == "sign"){
		string hash;
		string pass;
		ifstream hash_file("hash");
		if (!hash_file){
			cout << "Unable to open hash file" << endl;
			exit(1);
		}

		hash_file >> hash;
		hash_file.close();
		cout << "Pasword:" << endl;
		cin >> pass;
		if (sha256(pass) != hash){
			cout << "Invalid password" << endl;
			exit(1);
		}

		mpz_class n, d, e;
		ifstream privkey_file("privkey");
		if (!privkey_file){
			cout << "Unable to open privkey file" << endl;
			exit(1);
		}

		privkey_file >> n;
		privkey_file >> d;
		privkey_file.close();

		ifstream pubkey_file("pubkey");
		if (!pubkey_file){
			cout << "Unable to open pubkey file" << endl;
			exit(1);
		}

		pubkey_file >> n;
		pubkey_file >> e;
		pubkey_file.close();

		//cout << n << endl;
		//cout << d << endl;

		server(n, d, e);
	} else {
		std::cout << "Usage: bserver setup <keysize> or bserver sign" << endl;
		exit(0);
	}
	return 0;
}