#include "sezar.h"
#include <stdlib.h>
#include <string.h>

/*
 * String-based big-integer multiply.
 * Multiplies a decimal digit string by a long long multiplier,
 * returns the result as a new decimal string (no leading zeros, except "0").
 *
 * Caller must free() the result.
 */
static char* bignum_multiply(const char* digits, long long mult) {
    int len = (int)strlen(digits);

    /* Handle zero cases */
    if (len == 0 || mult == 0) {
        char* r = (char*)malloc(2);
        if (!r) return NULL;
        r[0] = '0'; r[1] = '\0';
        return r;
    }

    /* Max result length: len + 8 (multiplier is up to 7 digits + 1 carry) */
    int max_len = len + 8;
    char* temp = (char*)malloc(max_len + 1);
    if (!temp) return NULL;
    memset(temp, '0', max_len);
    temp[max_len] = '\0';

    long long carry = 0;
    int pos = max_len - 1;
    for (int i = len - 1; i >= 0; i--) {
        long long product = (long long)(digits[i] - '0') * mult + carry;
        temp[pos--] = (char)('0' + (int)(product % 10));
        carry = product / 10;
    }
    while (carry > 0) {
        temp[pos--] = (char)('0' + (int)(carry % 10));
        carry /= 10;
    }

    /* Skip leading zeros */
    int start = 0;
    while (start < max_len - 1 && temp[start] == '0') start++;

    int rlen = max_len - start;
    char* result = (char*)malloc(rlen + 1);
    if (!result) { free(temp); return NULL; }
    memcpy(result, temp + start, rlen);
    result[rlen] = '\0';
    free(temp);
    return result;
}

/*
 * String-based big-integer divide by long long.
 * Returns the quotient as a new string, sets *remainder.
 * Caller must free() the result.
 */
static char* bignum_divide(const char* digits, long long div, long long* remainder) {
    int len = (int)strlen(digits);
    /* Allocate at least 2 bytes so the "0" fallback case has room for '0' + '\0' */
    int alloc_len = len > 0 ? len : 1;
    char* result = (char*)malloc(alloc_len + 1);
    if (!result) return NULL;

    int result_pos = 0;
    long long cur = 0;

    for (int i = 0; i < len; i++) {
        cur = cur * 10 + (digits[i] - '0');
        if (cur >= div || result_pos > 0) {
            result[result_pos++] = (char)('0' + (int)(cur / div));
            cur = cur % div;
        }
    }

    if (result_pos == 0) {
        result[result_pos++] = '0';
    }
    result[result_pos] = '\0';
    *remainder = cur;
    return result;
}

int sezar_key_valid(long long key) {
    return (key >= 2 && key <= 9999999);
}

char* sezar_encode(const char* text) {
    if (!text) return NULL;
    int len = (int)strlen(text);
    char* result = (char*)malloc(len * 2 + 1);
    if (!result) return NULL;

    int pos = 0;
    for (int i = 0; i < len; i++) {
        char c = text[i];
        int code;
        if (c >= 'a' && c <= 'z') {
            code = c - 'a' + 1;
        } else if (c >= 'A' && c <= 'Z') {
            code = c - 'A' + 1;
        } else if (c == ' ') {
            code = 27;
        } else {
            continue; /* ignore punctuation, digits, etc. */
        }
        result[pos++] = (char)('0' + (code / 10));
        result[pos++] = (char)('0' + (code % 10));
    }
    result[pos] = '\0';
    return result;
}

char* sezar_decode(const char* digits) {
    if (!digits) return NULL;
    int len = (int)strlen(digits);
    if (len == 0 || len % 2 != 0) return NULL;

    char* result = (char*)malloc(len / 2 + 1);
    if (!result) return NULL;

    int pos = 0;
    for (int i = 0; i < len; i += 2) {
        int code = (digits[i] - '0') * 10 + (digits[i + 1] - '0');
        if (code < 1 || code > 27) {
            free(result);
            return NULL;
        }
        if (code == 27) {
            result[pos++] = ' ';
        } else {
            result[pos++] = (char)('a' + code - 1);
        }
    }
    result[pos] = '\0';
    return result;
}

char* sezar_encrypt(const char* text, long long key) {
    if (!text || !sezar_key_valid(key)) return NULL;

    char* digits = sezar_encode(text);
    if (!digits || strlen(digits) == 0) {
        free(digits);
        return NULL;
    }

    char* result = bignum_multiply(digits, key);
    free(digits);
    return result;
}

char* sezar_decrypt(const char* ciphertext, long long key) {
    if (!ciphertext || !sezar_key_valid(key)) return NULL;

    /* Validate that ciphertext contains only digits */
    int len = (int)strlen(ciphertext);
    if (len == 0) return NULL;
    for (int i = 0; i < len; i++) {
        if (ciphertext[i] < '0' || ciphertext[i] > '9') return NULL;
    }

    long long remainder = 0;
    char* divided = bignum_divide(ciphertext, key, &remainder);
    if (!divided) return NULL;

    if (remainder != 0) {
        /* Wrong key, or corrupted ciphertext */
        free(divided);
        return NULL;
    }

    /* Pad to even length (recovers any leading zero lost during multiply) */
    int dlen = (int)strlen(divided);
    char* padded;
    if (dlen % 2 != 0) {
        padded = (char*)malloc(dlen + 2);
        if (!padded) { free(divided); return NULL; }
        padded[0] = '0';
        memcpy(padded + 1, divided, dlen);
        padded[dlen + 1] = '\0';
        free(divided);
    } else {
        padded = divided;
    }

    char* result = sezar_decode(padded);
    free(padded);
    return result;
}
