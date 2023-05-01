// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/chrono/stopwatch.h"
#include "core/util/random.h"

auto rotate_right_window(uint64_t value, size_t n) {
    auto lower = value >> 1;
    auto upper = value << (n - 1);
    auto mask = (uint64_t{1} << n) - 1;
    return (lower bitor upper) bitand mask;
}

uint64_t hash(uint64_t v, size_t n) {
    uint64_t result{};
    uint64_t output_mask = uint64_t{1};
    uint64_t input0_mask = uint64_t{1} << 1;
    uint64_t input1_mask = uint64_t{1} << 2;
    uint64_t input2_mask = uint64_t{1} << 3;
    for (auto i = 0; i < n; ++i) {
	if ((v bitand input0_mask) ^ (v bitand input1_mask) ^ (v bitand input2_mask))
	    result |= output_mask;
	
	output_mask <<= 1;
	input0_mask <<= 1;
	input1_mask <<= 1;
	input2_mask <<= 1;
	if (not input0_mask)
	    input0_mask = 1;
	if (not input1_mask)
	    input1_mask = 1;
	if (not input2_mask)
	    input2_mask = 1;
    }
    
    return result;
};

template<class Work>
void measure(std::ostream& os, std::string_view desc, Work&& work) {
    chron::StopWatch timer;
    timer.mark();
    work();
    auto millis = timer.elapsed_duration<std::chrono::milliseconds>().count();
    os << fmt::format("{:>12s}: {:5d} ms", desc, millis) << endl;
}

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argValue<'b'>("bits", 4, "Number of bits"),
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);
    auto b = opts.get<'b'>();
    auto verbose = opts.get<'v'>();

    auto n = uint64_t{1} << b;
    std::vector<uint64_t> input;
    for (auto i = 0; i < n; ++i)
	input.push_back(i);

    if (verbose) {
	for (auto elem : input)
	    cout << elem << " " << rotate_right_window(elem, b) << endl;
    }
    
    return 0;
}
