# Stack Overflow Code

This repo contains code related to Stack Overflow answers. The code
related to each answer is in an independent directory named after the
Question title.

* [primes](#primes) -- Checking uniqueness of shifting sieve of eratosthenes
* [rshift](#rshift) -- C++ Bitwise shift of uint64_t in vector.
* [static-assert](#static-assert) -- Type constraining template parameters inside concept.

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

## <a name="primes"></a>`checking-uniqueness-of-shifting-sieve-of-eratosthenes`

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

## <a name="rshift"></a>`c++-bitwise-shift-of-long-long-int-in-vector-for-example-by-380b-bits`

```
git clone git@github.com:cpp-core/so.git
mkdir so/c++-bitwise-shift-of-long-long-int-in-vector-for-example-by-380b-bits
cd so/c++-bitwise-shift-of-long-long-int-in-vector-for-example-by-380b-bits/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make right_shift && ./bin/right_shift -n 128 -m 37
                                     110100001001000110111011010111000010001010101110100111101111011011100111111000011111101011101110110101011100001100011111011110010010000010000010001101010010110011111000000001111011011111011111
000000000000000000000000000000000000011010000100100011011101101011100001000101010111010011110111101101110011111100001111110101110111011010101110000110001111101111001001000001000001000110101001

```

## <a name="static-assert"></a>`type-contraining-template-parameters-inside-concept`

```
git clone git@github.com:cpp-core/so.git
mkdir so/type-constraining-template-parameters-inside-concept/build
cd so/type-constraining-template-parameters-inside-concept/build
CC=clang-mp-15 CXX=clang++-mp-15 cmake ..
make static_assert && ./bin/static_assert
$ make static_assert
/Users/mmelton/work/so/type-constraining-template-parameters-inside-concept/src/static_assert.cpp\
:26:5: error:
      static assertion failed
    static_assert(HasFuncs<nope>);
    ^             ~~~~~~~~~~~~~~
/Users/mmelton/work/so/type-constraining-template-parameters-inside-concept/src/static_assert.cpp\
:26:19: note:
      because 'nope' (aka 'tuple<good, bad, good>') does not satisfy 'HasFuncs'
    static_assert(HasFuncs<nope>);
                  ^
/Users/mmelton/work/so/type-constraining-template-parameters-inside-concept/src/static_assert.cpp\
:14:39: note:
      because 'HasFuncI<1, std::tuple<good, bad, good> >' evaluated to false
concept HasFuncs = HasFuncI<0, T> and HasFuncI<1, T> and HasFuncI<2, T> and HasFuncI<3, T> and
                                      ^
/Users/mmelton/work/so/type-constraining-template-parameters-inside-concept/src/static_assert.cpp\
:11:20: note:
      because 'std::tuple_size_v<std::tuple<good, bad, good> > <= 1UL' (3 <= 1) evaluated to
      false
concept HasFuncI = std::tuple_size_v<T> <= I or HasFunc<std::tuple_element_t<I, T>>;
                   ^
/Users/mmelton/work/so/type-constraining-template-parameters-inside-concept/src/static_assert.cpp\
:11:49: note:
      and 'std::tuple_element_t<1UL, tuple<good, bad, good>>' (aka '__type_pack_element<1UL,
      good, bad, good>') does not satisfy 'HasFunc'
concept HasFuncI = std::tuple_size_v<T> <= I or HasFunc<std::tuple_element_t<I, T>>;
                                                ^
/Users/mmelton/work/so/type-constraining-template-parameters-inside-concept/src/static_assert.cpp\
:8:38: note:
      because 'a.func()' would be invalid: no member named 'func' in 'bad'
concept HasFunc = requires (T a) { a.func(); };
                                     ^
1 error generated.
```
