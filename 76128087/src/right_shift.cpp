// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/util/random.h"

// Shift the bits in `a` by `n` bits to the right (towards lesser
// signifigance assuming that a[0] is the most significant word)
// prepending with zero bits as necessary.
void right_shift(std::vector<uint64_t>& a, size_t n) {
    constexpr auto bits_per_word = sizeof(uint64_t) * CHAR_BIT;

    // Parition n into the number of a number of whole words to shift and the
    // remaining number of bits to shift.
    auto words_to_shift = n / bits_per_word;
    auto bits_to_shift = n % bits_per_word;

    // Create a bit mask that covers the upper `bits_to_shift`
    // bits in a word. The value `(uint64_t{1} << bits_to_shift) -
    // 1` is the lower `bits_to_shift` bits so the bit reversal
    // turns them into the upper `bits_to_shift` bits.
    auto mask = __builtin_bitreverse64((uint64_t{1} << bits_to_shift) - 1);

    // The words in range [0,limit) are the only bits that will appear
    // in the result because words in range [limit,a.size) are shifted
    // "off the end". Starting with the least significant word, loop
    // over all the words that will contribute bits to the result
    // (possibly none).
    for (int src_idx = a.size() - words_to_shift - 1; src_idx >= 0; --src_idx) {
	// In general this source word will be split between the two
	// destination words left and right (least significant). The
	// lower `bits_to_shift` bits will go to the upper bits of the
	// right word while the upper bits will become the lower bits
	// of the left word.
	auto rdx = src_idx + words_to_shift + 1;
	auto ldx = src_idx + words_to_shift;

	// Get the source value and rotate it `bits_to_shift` bits to
	// the right so that the bits are aligned with their
	// eventual destinations.
	auto value = __builtin_rotateright64(a[src_idx], bits_to_shift);

	if (rdx < a.size())
	    a[rdx] |= value bitand mask;
	a[ldx] = value bitand ~mask;
    }

    // Prepend with zeros as necessary
    for (auto i = 0; i < words_to_shift; ++i)
	a[i] = 0;
}

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argValue<'n'>("number-bits", 512, "Number bits"),
	 argValue<'m'>("shift", 380, "Right shift M bits"),
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);
    auto n = opts.get<'n'>();
    auto m = opts.get<'m'>();
    // auto verbose = opts.get<'v'>();

    std::vector<uint64_t> bits(1 + n / 64);
    std::uniform_int_distribution<uint64_t> d;
    for (auto& value : bits)
	value = d(core::rng());

    cout << std::string(m, ' ');
    for (const auto& value : bits)
	cout << fmt::format("{:064b}", value);
    cout << endl;

    right_shift(bits, m);
    for (const auto& value : bits)
	cout << fmt::format("{:064b}", value);
    cout << endl;

    return 0;
}
