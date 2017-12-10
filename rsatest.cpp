#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include "rsa.h"

using namespace std;

int main(){
	int keysize = 1024;
	RSA rsa(keysize);
	cout << "p: " << rsa.p << endl;
	cout << "q: " << rsa.q << endl;
	cout << "n: " << rsa.n << endl;
	cout << "e: " << rsa.e << endl;
	cout << "d: " << rsa.d << endl;
	mpz_class d, e, lambda, n;
	d = rsa.d;
	e = rsa.e;
	lambda = rsa.lambda;
	n = rsa.n;
	cout << "x: " << (e * d) % lambda << endl;

	mpz_class message, message_p;
	gmp_randclass r(gmp_randinit_mt);
	r.seed(time(NULL));
	for(int i = 0; i < 20; i++){
		message = r.get_z_range(rsa.n);
		mpz_powm(message_p.get_mpz_t(), message.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
		mpz_powm(message_p.get_mpz_t(), message_p.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
		cout << (message == message_p) << endl;
	}

	return 0;
}