#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include "rsa.h"

using namespace std;

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
		//mpz_powm(message_p.get_mpz_t(), message.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
		message_p = powmod_consttime(message, e , n);
		message_p = powmod_consttime(message_p, d , n);
		//mpz_powm(message_p.get_mpz_t(), message_p.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
		cout << (message == message_p) << endl;
	}

	mpz_class a, b, c, z;
	a = 11230;
	b = 5959;
	c = 134447;
	mpz_powm(z.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t(), c.get_mpz_t());
	cout << powmod_consttime(a, b, c) << endl;
	cout << z << endl;
	cout << a << " " << b << " " << c << endl;

	return 0;
}