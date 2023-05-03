
The following solution constructs a bijective function F that maps a
range of integers onto itself. This function can be used to compute a
pseudo-random index directly from a source index such that the
resulting pseudo-random indices are a permutation of the source
indices.

There are three ideas (all borrowed from cryptography) that taken
together allow the construction of such a function: 1) a Pseudo-Random
Function Family [PRF][1], 2) a [Feistel][2] network, and 3) a
format-preserving-encryption [FPE][3].  While these ideas draw on
well-studied cryptography concepts, I believe the end product is
probably unqiue and should definitely be considered insecure.

The basic strategy is to encrypt the source index to produce the
target index. The secret sauce is to design the encryption to be
bijective and use a range of integers as the domain.

The first step in the construction is creating a PRF that returns a
pseudo-random value given a 64-bit input. We can create this family
using a single function that also takes a subkey parameter that is
used to select the particular function to use. The canonical PRF
example is using AES to produce a 128-bit pseudo-random value. We use
the following function which is much more efficient to evaluate
(although much less secure) and produces a 64-bit pseudo-random
value. The `s0` parameter is the source index and `s1` parameter is
the subkey.

```c++
uint64_t pseudo_random_function(uint64_t s0, uint64_t s1) {
    auto a = s0 + s1;
    a ^= a >> 12;
    a ^= a << 25;
    a ^= a >> 27;
    return a * 0x2545f4914f6cdd1dull;
}
```

This function can be used directly to order the source indices
producing a pseudo-random permutation as in [Severin Pappadeux's
answer](https://stackoverflow.com/a/76082801/1202808) which is
equivalent to constructing an FPE using a prefix cipher. The main
difference is that this PRF produces more "random" looking results
than using the linear congruent generator.

Instead of using the PRF directly, we will apply a Feistel network
that leverages the PRF as its round function. The two key advantage of
the Feistel network is 1) the operation is guaranteed to be invertible
even if the round function is not, and 2) the number of output bits
can be selected to be at most one or two more than the number of input
bits making the encoding range of the network at most four times
larger than the input domain. The following class implements a
balanced Feistel network.

```c++
template<class PRF>
class FeistelNetwork {
public:
    FeistelNetwork(int number_of_bits, int number_rounds, PRF&& prf)
	: shift_((1 + number_of_bits) / 2)
	, mask_((uint64_t{1} << shift_) - 1)
	, nrounds_(number_rounds)
	, prf_(std::forward<PRF>(prf)) {
    }

    auto encode(uint64_t msg) const {
	auto [left, right] = split(msg);
	for (auto i = 0; i < nrounds_; ++i)
	    round(left, right, Rounds[i]);
	return combine(left, right);
    }

    auto decode(uint64_t msg) const {
	auto [left, right] = split(msg);
	for (int i = nrounds_ - 1; i >= 0; --i)
	    round(right, left, Rounds[i]);
	return combine(left, right);
    }

private:
    std::tuple<uint64_t, uint64_t> split(uint64_t msg) const {
	auto right = msg bitand mask_;
	auto left = (msg >> shift_) bitand mask_;
	return std::make_tuple(left, right);
    }

    uint64_t combine(uint64_t left, uint64_t right) const {
	return (left << shift_) bitor right;
    }

    void round(uint64_t& left, uint64_t& right, uint64_t constant) const {
	auto prf_value = prf_(right, constant) bitand mask_;
	auto r = left ^ prf_value;
	left = right;
	right = r;
    }

    static constexpr uint64_t Rounds[] = {
	0x88ef7267b3f978daull,
	0x5457c7476ab3e57full,
	0x89529ec3c1eec593ull,
	0x3fac1e6e30cad1b6ull,
	0x56c644080098fc55ull,
	0x70f2b329323dbf62ull,
	0x08ee98c0d05e3dadull,
	0x3eb3d6236f23e7b7ull,
	0x47d2e1bf72264fa0ull,
	0x1fb274465e56ba20ull,
	0x077de40941c93774ull,
	0x857961a8a772650dull
    };
    
    int shift_;
    uint64_t mask_;
    int nrounds_;
    PRF prf_;
};
```

If the source index range happens to be an even power of two, then we
can simply call encode on the Feistel network to map a source index to
a pseudo-random target index. In general, however, the Feistel network
may return an encoding that is outside the source index domain. The
solution is to simply call encode on the out-of-range index until we
get an index that is in the source index domain. This recursion will
terminate because the Feistel network encryption is bijective and the
domain is finite. The following class implements this logic along with
mapping the source index domain from `min,max` to `0,max-min`.

```c++
template<class PRF>
class PseudoRandomPermutation {
public:
    PseudoRandomPermutation(uint64_t min, uint64_t max, int rounds, PRF&& prf)
	: min_(min)
	, size_(max - min)
	, cipher_(64 - std::countl_zero(size_), rounds, std::forward<PRF>(prf)) { 
    }

    auto min() const {
	return min_;
    }

    auto max() const {
	return min_ + size_;
    }

    auto size() const {
	return size_;
    }
    
    uint64_t encode(uint64_t msg) const {
	msg -= min_;
	do {
	    msg = cipher_.encode(msg);
	} while (msg >= size_);
	msg += min_;
	return msg;
    }
    
private:
    uint64_t min_, size_;
    FeistelNetwork<PRF> cipher_;
};
```

  [1]: https://en.wikipedia.org/wiki/Format-preserving_encryption
  [2]: https://en.wikipedia.org/wiki/Feistel_cipher
  [3]: https://en.wikipedia.org/wiki/Format-preserving_encryption


