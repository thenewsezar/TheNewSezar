#ifndef SEZAR_H
#define SEZAR_H

/*
 * The New Sezar Algorithm
 * ----------------------
 * A toy cipher inspired by the classical Caesar cipher.
 * Instead of shifting letters, it multiplies the encoded digit-string
 * by a shared secret key.
 *
 * Encoding:
 *   a-z -> 01..26
 *   ' '  -> 27
 *   everything else is ignored
 *
 * Encryption:
 *   digit_string * key  (string-based bignum, no GMP)
 *
 * Decryption:
 *   ciphertext / key, then pad to even length, then decode pairs
 *
 * Key range: 2 to 9,999,999   (0 and 1 are forbidden)
 *
 * WARNING: This is a fun puzzle cipher, NOT real encryption.
 *          The entire key space (~10M) is brute-forceable in seconds.
 */

/*
 * Encode text into a 2-digit-per-char digit string.
 * Caller must free() the returned string.
 * Returns NULL on allocation failure.
 */
char* sezar_encode(const char* text);

/*
 * Decode a 2-digit-per-char digit string back into text.
 * Caller must free() the returned string.
 * Returns NULL if the string is malformed (odd length, pair out of 01-27).
 */
char* sezar_decode(const char* digits);

/*
 * Encrypt: text -> ciphertext string.
 * Caller must free() the returned string.
 * Returns NULL if key is out of range or input is empty.
 */
char* sezar_encrypt(const char* text, long long key);

/*
 * Decrypt: ciphertext -> text.
 * Caller must free() the returned string.
 * Returns NULL if key is out of range, ciphertext does not divide evenly,
 * or the decoded digit string is malformed.
 */
char* sezar_decrypt(const char* ciphertext, long long key);

/*
 * Returns 1 if key is valid (2 <= key <= 9999999), 0 otherwise.
 */
int sezar_key_valid(long long key);

#endif /* SEZAR_H */
