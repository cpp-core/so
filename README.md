# Stack Overflow Code

This repo contains code related to Stack Overflow answers. The code
related to each answer is in an independent directory named after the
Question title.

* [primes](#primes) -- Checking uniqueness of shifting sieve of eratosthenes

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
