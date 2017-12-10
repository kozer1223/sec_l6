#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <iomanip>
#include "rsa.h"

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
	if (argc < 4){
		std::cout << "Usage: bvrfy <message file> <signed file> <pubkey file>" << endl;
		exit(0);
	}
	string message_fname, signed_fname, pubkey_fname;
	message_fname = argv[1];
	signed_fname = argv[2];
	pubkey_fname = argv[3];

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
	ifstream message_file(message_fname);
	if (!message_file){
		cout << "Unable to open message file" << endl;
		exit(1);
	}
	message_file >> message;
	message_file.close();

	mpz_class s;
	ifstream signed_file(signed_fname);
	if (!signed_file){
		cout << "Unable to open signed file" << endl;
		exit(1);
	}
	signed_file >> s;
	signed_file.close();

	mpz_class m, decoded_s;
	m = str_to_int(message);

	mpz_powm(decoded_s.get_mpz_t(), s.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
	if (decoded_s == m) {
		cout << "Message verified!" << endl;
	} else {
		cout << "Wrong signature!" << endl;
	}
	return 0;
}