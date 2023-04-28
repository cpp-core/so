# Stack Overflow Code

This repo contains code related to Stack Overflow answers. The code
related to each answer is in an independent directory named after the
Question title.

* [primes](#primes) -- Checking uniqueness of shifting sieve of eratosthenes
* [rshift](#rshift) -- C++ Bitwise shift of uint64_t in vector.

If you need to install a particular versionn of clang for a build on Ubuntu, you
can do something like the following:

```
apt install -y clang-15 libc++-15-dev libc++abi-15-dev
```

On older versions of Ubuntu (e.g. 20.04), you may have to point to the
clang packages before installing with something like:

```
wget https://apt.llvm.org/llvm.sh
./llvm.sh 15
```

## <a name="#primes"></a>`checking-uniqueness-of-shifting-sieve-of-eratosthenes`

```
git clone git@github.com:cpp-core/so.git
mkdir so/checking-uniqueness-of-shifting-sieve-of-eratosthenes/build
cd so/checking-uniqueness-of-shifting-sieve-of-eratosthenes/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make primes && ./bin/primes
 sieve_index:   187 ms
    sieve_2n:     0 ms
    sieve_6n:     0 ms
```

## <a name="#rshift"></a>`c++-bitwise-shift-of-long-long-int-in-vector-for-example-by-380b-bits`

```
git clone git@github.com:cpp-core/so.git
mkdir so/c++-bitwise-shift-of-long-long-int-in-vector-for-example-by-380b-bits
cd so/c++-bitwise-shift-of-long-long-int-in-vector-for-example-by-380b-bits/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make right_shift && ./bin/right_shift -n 128 -m 37
                                     110100001001000110111011010111000010001010101110100111101111011011100111111000011111101011101110110101011100001100011111011110010010000010000010001101010010110011111000000001111011011111011111
000000000000000000000000000000000000011010000100100011011101101011100001000101010111010011110111101101110011111100001111110101110111011010101110000110001111101111001001000001000001000110101001

```
