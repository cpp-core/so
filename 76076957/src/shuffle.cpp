// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/util/random.h"
#include "core/chrono/stopwatch.h"
#include "core/string/lexical_cast_stl.h"

template<class Work>
void measure(std::ostream& os, std::string_view desc, Work&& work) {
    chron::StopWatch timer;
    timer.mark();
    if (work())
	os << fmt::format("{:>12s}: work failed", desc) << endl;
    auto millis = timer.elapsed_duration<std::chrono::milliseconds>().count();
    os << fmt::format("{:>12s}: {:5d} ms", desc, millis) << endl;
}

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argValue<'m'>("range", std::make_pair(0, 16), "Permutation range min:max")
	 );
    opts.parse(argc, argv);
    auto [min, max] = opts.get<'m'>();

    std::vector<uint64_t> data;
    for (auto i = min; i <= max; ++i)
	data.push_back(i);
    std::shuffle(data.begin(), data.end(), core::rng());

    for (auto elem : data)
	cout << elem << endl;

    return 0;
}
