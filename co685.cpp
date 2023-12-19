#include <cassert>
#include <gmp.h>
#include <iostream>

void generateRandomNumber(mpz_t n, const mpz_t limit) {
  gmp_randstate_t state;
  gmp_randinit_default(state);
  gmp_randseed_ui(state, static_cast<unsigned long>(time(nullptr)));

  mpz_urandomm(n, state, limit);

  gmp_randclear(state);
}

void getBobParameters(mpz_t r, mpz_t y, mpz_t x, const mpz_t p, const mpz_t q,
                      const mpz_t n) {
  // set r
  gmp_printf("Finding an appropriate value for r\n");
  mpz_t limit, pmo, qmo, pmoDiv, gcdB, gcdC;
  mpz_inits(limit, pmo, qmo, pmoDiv, gcdB, gcdC, NULL);
  mpz_sub_ui(pmo, p, 1);
  mpz_sub_ui(qmo, q, 1);
  mpz_set(limit, q);
  if (mpz_cmp(p, q) < 0) {
    mpz_set(limit, p);
  }
  do {
    generateRandomNumber(r, limit);
    if (mpz_divisible_p(pmo, r) == 0) {
      continue;
    }
    mpz_divexact(pmoDiv, pmo, r);
    mpz_gcd(gcdB, r, pmoDiv);
    mpz_gcd(gcdC, r, qmo);
    // gmp_printf("pmo qmo gB bC: %Zd %Zd %Zd %Zd\n", pmoDiv, qmo, gcdB, gcdC);
  } while ((mpz_divisible_p(pmo, r) == 0) || (mpz_cmp_ui(gcdB, 1) != 0) ||
           (mpz_cmp_ui(gcdC, 1) != 0));
  gmp_printf("found r: %Zd\n", r);
  // set y
  gmp_printf("Finding an appropriate value for x and y\n");
  mpz_t phi, phiDiv;
  mpz_inits(phi, phiDiv, NULL);
  mpz_mul(phi, pmo, qmo);
  mpz_divexact(phiDiv, phi, r);
  do {
    generateRandomNumber(y, n);
    mpz_powm(x, y, phiDiv, n);
    gmp_printf("x y phiDiv r: %Zd %Zd %Zd %Zd\n", x, y, phiDiv, r);
  } while (mpz_cmp_ui(x, 1) == 0);
  gmp_printf("found x, y: %Zd, %Zd\n", x, y);
  mpz_clears(phi, phiDiv, NULL);
  mpz_clears(limit, pmo, qmo, pmoDiv, gcdB, gcdC, NULL);
}

void generateDistinctPrimes(mpz_t prime1, mpz_t prime2, unsigned int bitSize) {
  gmp_randstate_t state;
  gmp_randinit_default(state);
  gmp_randseed_ui(state, static_cast<unsigned long>(time(nullptr)));

  // Generate first prime
  do {
    mpz_urandomb(prime1, state, bitSize);
    mpz_nextprime(prime1, prime1);
  } while (mpz_probab_prime_p(prime1, 25) == 0);

  // Generate second distinct prime
  do {
    mpz_urandomb(prime2, state, bitSize);
    mpz_nextprime(prime2, prime2);
  } while (mpz_probab_prime_p(prime2, 25) == 0 && mpz_cmp(prime1, prime2) != 0);

  gmp_randclear(state);
}

// Benaloh Encryption Function
void benalohEncrypt(mpz_t result, const mpz_t message, const mpz_t y,
                    const mpz_t r, const mpz_t n, const mpz_t u) {
  mpz_t y_to_m, u_to_r;
  mpz_inits(y_to_m, u_to_r, NULL);

  mpz_powm(y_to_m, y, message, n); // y^message mod n
  mpz_powm(u_to_r, u, r, n);       // u^r mod n

  mpz_mul(result, u_to_r, y_to_m); // u^r * y^message
  mpz_mod(result, result, n);      // (u^r * y^message) mod n
  mpz_clears(y_to_m, u_to_r, NULL);
}

// Benaloh Decryption Function (Brute Force for Small Numbers)
void benalohDecrypt(mpz_t decrypted, const mpz_t ciphertext, const mpz_t p,
                    const mpz_t q, const mpz_t r, const mpz_t x,
                    const mpz_t n) {
  mpz_t phi, div, a, pmo, qmo;
  mpz_t rth_root;
  mpz_inits(pmo, qmo, a, phi, div, rth_root, NULL);

  mpz_sub_ui(pmo, p, 1);
  mpz_sub_ui(qmo, q, 1);
  // gmp_printf("check: %Zd, %Zd, %Zd, %Zd\n", p, pmo, q, qmo);
  mpz_mul(phi, pmo, qmo);
  mpz_divexact(div, phi, r);
  mpz_powm(a, ciphertext, div, n);
  gmp_printf("check: %Zd, %Zd, %Zd, %Zd\n", a, ciphertext, div, phi);

  // Brute force search for rth_root such that (x ^ rth_root) mod n equals a
  for (mpz_set_ui(rth_root, 0); mpz_cmp(rth_root, n) < 0;
       mpz_add_ui(rth_root, rth_root, 1)) {
    mpz_t temp;
    mpz_init(temp);
    mpz_powm(temp, x, rth_root, n); // temp = rth_root^r mod n

    if (mpz_cmp(temp, a) == 0) {
      mpz_set(decrypted, rth_root);
      mpz_clear(temp);
      break;
    }

    mpz_clear(temp);
  }

  gmp_printf("check: %Zd, %Zd, %Zd, %Zd\n", decrypted, rth_root, a, phi);

  mpz_clears(pmo, qmo, a, phi, div, rth_root, NULL);
}

int main() {
  mpz_t p, q, n, r, x, y, u, message, encrypted, decrypted;
  gmp_randstate_t state;

  mpz_inits(p, q, n, r, x, y, u, message, encrypted, decrypted, NULL);
  gmp_randinit_default(state); // rnd generator
  // Seed the random state with the current time
  gmp_randseed_ui(state, static_cast<unsigned long>(time(nullptr)));

  // Test values
  // mpz_set_ui(p, 397); // p = 11
  // mpz_set_ui(q, 191); // q = 17
  // mpz_set_ui(r, 99);  // r = 3

  generateDistinctPrimes(p, q, 32);
  mpz_mul(n, p, q);
  generateRandomNumber(u, n);
  getBobParameters(r, y, x, p, q, n);
  generateRandomNumber(message, r);
  gmp_printf("Original Message: %Zd\n", message);

  // Encrypt the message
  benalohEncrypt(encrypted, message, y, r, n, u);
  gmp_printf("Encrypted Message: %Zd\n", encrypted);

  // Decrypt the message
  benalohDecrypt(decrypted, encrypted, p, q, r, x, n);
  gmp_printf("Decrypted Message: %Zd\n", decrypted);

  // Assert that the original message and the decrypted message are equal
  assert(mpz_cmp(message, decrypted) == 0);

  std::cout << "Assertion passed: The original message and decrypted message "
               "are equal."
            << std::endl;

  // Clear mpz_t variables and random state
  mpz_clears(p, q, n, r, y, u, message, encrypted, decrypted, NULL);
  gmp_randclear(state);

  return 0;
}
