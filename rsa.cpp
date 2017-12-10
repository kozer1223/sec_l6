#include <gmp.h>
#include <gmpxx.h>
#include "rsa.h"

using namespace std;

/*
mpz_class random_prime(int bits){
	mpz_class min = 1;
	mpz_ui_pow_ui(min.get_mpz_t(), 2, bits-1);
	min *= sqrt(mpz_class(2));

	mpz_class max = 1;
	mpz_ui_pow_ui(max.get_mpz_t(), 2, bits);

	gmp_randclass r(gmp_randinit_mt);
	int evenFix = (min % 2 == 0);
	min += evenFix;
	mpz_class difference = (max-min)/2;

	r.seed(time(NULL));
	while(true){
		mpz_class p = min + 2 * r.get_z_range(difference);
		if (mpz_probab_prime_p(p.get_mpz_t(), 35)){
			return p;
		}
	}
}
*/

mpz_class random_prime(int bits){
	mpz_class min = 1;
	mpz_ui_pow_ui(min.get_mpz_t(), 2, bits-1);

	gmp_randclass r(gmp_randinit_mt);

	r.seed(time(NULL));
	while(true){
		mpz_class p = min + (r.get_z_bits(bits-2) << 1) + 1;
		if (mpz_probab_prime_p(p.get_mpz_t(), 35)){
			return p;
		}
	}
}

RSA::RSA(int keysize) {
	p = random_prime(keysize/2);
	//cout << p << endl;
	do {
		q = random_prime(keysize/2);
		//cout << q << endl;
	} while (p == q);
	lambda = lcm(p - 1, q - 1);
	n = p * q;

	gmp_randclass r(gmp_randinit_mt);
	r.seed(time(NULL));
	do {
		e = r.get_z_range(lambda - 2);
		e += 2;
	} while (gcd(e, lambda) != 1);

	mpz_invert(d.get_mpz_t(), e.get_mpz_t(), lambda.get_mpz_t());
}