#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sezar.h"

static int tests_run = 0;
static int tests_passed = 0;

static void test_round_trip(const char* text, long long key) {
    tests_run++;
    char* ct = sezar_encrypt(text, key);
    if (!ct) {
        printf("FAIL  [%s] (key=%lld) -> encrypt returned NULL\n", text, key);
        return;
    }
    char* pt = sezar_decrypt(ct, key);
    if (!pt) {
        printf("FAIL  [%s] (key=%lld) -> ciphertext=%s -> decrypt returned NULL\n",
               text, key, ct);
        free(ct);
        return;
    }
    if (strcmp(pt, text) == 0) {
        printf("PASS  [%-22s] key=%-8lld ct=%s\n", text, key, ct);
        tests_passed++;
    } else {
        printf("FAIL  [%s] (key=%lld) -> ciphertext=%s -> got '%s' (expected '%s')\n",
               text, key, ct, pt, text);
    }
    free(ct);
    free(pt);
}

static void test_invalid_key(long long key, const char* label) {
    tests_run++;
    char* r = sezar_encrypt("hello", key);
    if (r == NULL) {
        printf("PASS  invalid key %s (%lld) correctly rejected\n", label, key);
        tests_passed++;
    } else {
        printf("FAIL  invalid key %s (%lld) was accepted (should be rejected)\n", label, key);
        free(r);
    }
}

static void test_decode_invalid(const char* digits, const char* label) {
    tests_run++;
    char* r = sezar_decode(digits);
    if (r == NULL) {
        printf("PASS  invalid digit string '%s' (%s) correctly rejected\n", digits, label);
        tests_passed++;
    } else {
        printf("FAIL  invalid digit string '%s' (%s) was accepted\n", digits, label);
        free(r);
    }
}

int main(void) {
    printf("=== The New Sezar Algorithm — Tests ===\n\n");

    /* Round-trip tests */
    test_round_trip("hello", 4781);
    test_round_trip("hello world", 4781);
    test_round_trip("abc", 100);
    test_round_trip("xyz", 9999999);
    test_round_trip("hello world", 5050);
    test_round_trip("a", 2);
    test_round_trip("z", 9999999);
    test_round_trip("the quick brown fox", 1234567);
    test_round_trip("aaaa", 4781);
    test_round_trip("  leading and trailing  ", 4781);
    test_round_trip("hello punctuation world", 4781); /* punctuation stripped by design */
    test_round_trip("mixed case hello", 4781); /* case-insensitive: output is lowercase */
    test_round_trip("z", 2);
    test_round_trip("a", 9999999);

    /* Edge case: long message */
    test_round_trip("the quick brown fox jumps over the lazy dog", 9999999);

    printf("\n--- Invalid key tests ---\n");
    test_invalid_key(0, "forbidden");
    test_invalid_key(1, "forbidden");
    test_invalid_key(-1, "negative");
    test_invalid_key(10000000, "too large");

    printf("\n--- Invalid decode tests ---\n");
    test_decode_invalid("0", "odd length");
    test_decode_invalid("28", "code out of range (high)");
    test_decode_invalid("00", "code out of range (zero)");
    test_decode_invalid("99", "code out of range");

    printf("\n=== Results: %d/%d passed ===\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
