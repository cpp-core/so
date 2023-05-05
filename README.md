# Stack Overflow Code

This repo contains code related to Stack Overflow Questions and Answers. The code
related to each Question is in an independent directory named after the
Question title.

* [fiesty](#fiesty) -- Efficient pseudo-random permutation.
* [pfr](#pfr) -- Basic structure reflection.
* [primes](#primes) -- Checking uniqueness of shifting sieve of eratosthenes
* [rshift](#rshift) -- C++ Bitwise shift of uint64_t in vector.
* [static-assert](#static-assert) -- Type constraining template parameters inside concept.
* [compound-assert](#compound) -- Find compound words

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

## <a name="fiesty"></a> Efficient pseudo-random permutation

```
git clone git@github.com:cpp-core/so.git
mkdir so/76076957/build
cd so/76076957/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make feisty && ./bin/feisty -r 3 -m 0:100000000 -p
 Permutation:  3854 ms
```

## <a name="pfr"></a> Basic structure reflection

```
git clone git@github.com:cpp-core/so.git
mkdir so/76168305/build
cd so/76168305/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make scalr_struct && ./bin/scalar_struct
```

## <a name="primes"></a> Checking uniqueness of shifting sieve of eratosthenes

```
git clone git@github.com:cpp-core/so.git
mkdir so/76107531/build
cd so/76107531/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make primes && ./bin/primes
 sieve_index:   187 ms
    sieve_2n:     0 ms
    sieve_6n:     0 ms
```

## <a name="rshift"></a> C++ Bitwise shift of uint64_t in vector

```
git clone git@github.com:cpp-core/so.git
mkdir so/76128087
cd so/76128087/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make right_shift && ./bin/right_shift -n 128 -m 37
                                     110100001001000110111011010111000010001010101110100111101111011011100111111000011111101011101110110101011100001100011111011110010010000010000010001101010010110011111000000001111011011111011111
000000000000000000000000000000000000011010000100100011011101101011100001000101010111010011110111101101110011111100001111110101110111011010101110000110001111101111001001000001000001000110101001

```

## <a name="static-assert"></a> Type constraining template parameters inside concept

```
git clone git@github.com:cpp-core/so.git
mkdir so/76114394/build
cd so/76114394/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make static_assert && ./bin/static_assert
```

## <a name="compound"></a> Find compound words

```
git clone git@github.com:cpp-core/so.git
mkdir so/76178746/build
cd so/76178746/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make compound && ./bin/compound
```
