# The New Sezar

A toy cipher inspired by the classical Caesar cipher — but instead of shifting letters, it **multiplies numbers**.

> ⚠️ **NOT REAL ENCRYPTION.** This is a hobby project for learning and fun. The key space (~10 million) is brute-forceable in seconds on a normal laptop. Do **not** use this to protect anything you actually care about.

---

## How it works

1. **Encode** each character to a 2-digit number:
   - `a=01`, `b=02`, ..., `z=26`
   - `space=27`
   - All other characters (punctuation, digits, emoji) are ignored.
   - Letters are case-insensitive (`A` and `a` both encode to `01`).

2. **Concatenate** into a single digit string.

3. **Multiply** the entire digit string by a shared key (any integer from `2` to `9,999,999`).

4. **Share** the resulting ciphertext. The receiver divides it back by the same key, pads to even length if needed (recovers any leading zero lost during multiplication), and decodes pairs back to text.

### Worked example

```
Input:   "hello world"
Encode:  h=08 e=05 l=12 l=12 o=15 (space=27) w=23 o=15 r=18 l=12 d=04
Digits:  0805121215272315181204
Key:     4781
Cipher:  3849284530216938881336324
```

Receiver divides `3849284530216938881336324 ÷ 4781` → `805121215272315181204`, pads to even length → `0805121215272315181204`, decodes pairs → `"hello world"`. ✅

---

## Install

### Option A — Build locally (requires `gcc`)

```bash
git clone https://github.com/TheNewSezar/TheNewSezar.git
cd TheNewSezar
make
```

### Option B — Run via Docker (no compiler needed)

```bash
git clone https://github.com/TheNewSezar/TheNewSezar.git
cd TheNewSezar
docker build -t thenewsezar .
```

### Option C — Download a prebuilt binary

Prebuilt binaries for Linux, macOS, and Windows are published on the [Releases page](../../releases) — built automatically by GitHub Actions on every tag push.

---

## Usage

### Local binary

```bash
./sezar encrypt "hello world" 4781
# → 3849284530216938881336324

./sezar decrypt 3849284530216938881336324 4781
# → hello world
```

### Docker

```bash
docker run --rm thenewsezar encrypt "hello world" 4781
# → 3849284530216938881336324

docker run --rm thenewsezar decrypt 3849284530216938881336324 4781
# → hello world
```

### Help

```bash
./sezar --help
```

---

## Key rules

| Rule | Reason |
|---|---|
| Key must be in range `2` to `9,999,999` | Defined by the spec |
| `0` is forbidden | Multiplying by 0 destroys the message permanently (no recovery possible) |
| `1` is forbidden | Multiplying by 1 leaves the digit string unchanged — no encryption happens |

---

## Tests

```bash
make test
```

Runs 23 tests covering: round-trip encryption/decryption, edge cases (single chars, long messages, leading spaces), invalid keys (0, 1, negative, too large), and malformed ciphertexts (odd-length, codes out of range).

---

## Project structure

```
TheNewSezar/
├── README.md
├── Makefile                   # local build
├── Dockerfile                 # container build (Alpine + gcc)
├── .dockerignore
├── .github/
│   └── workflows/
│       └── release.yml        # auto-builds 3-platform binaries on tag push
├── src/
│   ├── sezar.h                # public API
│   ├── sezar.c                # core + string-based bignum (no GMP)
│   └── cli.c                  # command-line interface
└── tests/
    └── test_sezar.c           # round-trip + edge case tests
```

---

## Implementation notes

- **No external dependencies.** The bignum multiply/divide is implemented from scratch using string-based algorithms (schoolbook multiplication and long division on decimal digit strings). No GMP, no OpenSSL, no libs.
- **Portable C11.** Compiles on Linux, macOS, and Windows with any C compiler.
- **Leading-zero recovery.** When a message starts with `a`–`i` (codes `01`–`09`), the encoded digit string has a leading zero that gets lost when treated as a number. The decrypt function automatically pads the divided result to even length, recovering the leading zero before decoding.
- **Case-insensitive.** Both `A` and `a` encode to `01`. Decrypted output is always lowercase.

---

## Security analysis

**This cipher is not secure.** Here's why, so you understand what you're using:

| Property | Value |
|---|---|
| Key space | ~10 million (≈23 bits of entropy) |
| Brute-force time | Seconds on a normal laptop |
| Attack method | Try every key 2–9,999,999. For each, divide the ciphertext. If the result is all digits in range `01–27`, it decodes to readable text. The correct key is obvious. |
| Known-plaintext attack | Trivial — if attacker has one plaintext+ciphertext pair, they can derive the key by division, then decrypt all future messages. |
| Pattern leakage | Same key + same letter always produces the same ciphertext prefix. Frequency analysis applies. |

**Use it for:**
- ✅ Puzzles with friends
- ✅ Learning how ciphers work
- ✅ A fun project on GitHub

**Do NOT use it for:**
- ❌ Passwords or secrets
- ❌ Personal messages you actually want private
- ❌ Anything that matters

If you want real encryption, use [AES](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard), [ChaCha20-Poly1305](https://en.wikipedia.org/wiki/ChaCha20-Poly1305), or [age](https://github.com/FiloSottile/age).

---

## License

MIT — see [LICENSE](LICENSE).

---

## Author

**TheNewSezar** — built as a learning project, open sourced for fun.
