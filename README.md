# Stack Overflow Code

This repo contains code related to Stack Overflow answers. The code
related to each answer is in an independent directory named after the
Question title.

* [primes](#primes) -- Checking uniqueness of shifting sieve of eratosthenes

## <a name="#primes"></a>Build `checking-uniqueness-of-shifting-sieve-of-eratosthenes`

```
git clone git@github.com:cpp-core/so.git
mkdir so/checking-uniqueness-of-shifting-sieve-of-eratosthenes/build
cd so/checking-uniqueness-of-shifting-sieve-of-eratosthenes/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make primes && ./bin/primes
```
