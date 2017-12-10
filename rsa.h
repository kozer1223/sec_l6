#include <gmp.h>
#include <gmpxx.h>
#include <iostream>

mpz_class random_prime(int bits);

class RSA {
public:
	mpz_class n,p,q,e,d,lambda;
	RSA(int);
};