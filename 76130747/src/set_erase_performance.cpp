// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/chrono/stopwatch.h"
#include "core/util/random.h"

template<class Work>
void measure(std::ostream& os, std::string_view desc, Work&& work) {
    chron::StopWatch timer;
    timer.mark();
    work();
    auto millis = timer.elapsed_duration<std::chrono::milliseconds>().count();
    os << fmt::format("{:>16s}: {:5d} ms", desc, millis) << endl;
}

int tool_main(int argc, const char *argv[]) {
    ArgParse opts
	(
	 argValue<'n'>("number", 100000, "Number of elements"),
	 argFlag<'v'>("verbose", "Verbose diagnostics")
	 );
    opts.parse(argc, argv);
    auto n = opts.get<'n'>();
    // auto verbose = opts.get<'v'>();

    using Set = std::set<uint64_t>;
    using Elements = std::vector<Set::value_type>;
    using Iterators = std::vector<Set::iterator>;

    Set src_data;
    Elements elements;
    for (auto i = 0; i < n; ++i) {
	src_data.insert(i);
	elements.push_back(i);
    }
    
    Set data = src_data;
    Iterators iterators(n);
    for (auto i = 0; i < n; ++i)
	iterators[i] = data.find(i);
    measure(cout, "iterator-ordered", [&]() {
	for (auto iter : iterators) {
	    data.erase(iter);
	}
    });

    data = src_data;
    for (auto i = 0; i < n; ++i)
	iterators[i] = data.find(i);
    std::reverse(iterators.begin(), iterators.end());
    measure(cout, "iterator-reverse", [&]() {
	for (auto iter : iterators)
	    data.erase(iter);
    });

    data = src_data;
    for (auto i = 0; i < n; ++i)
	iterators[i] = data.find(i);
    std::shuffle(iterators.begin(), iterators.end(), core::rng());
    measure(cout, "iterator-random", [&]() {
	for (auto iter : iterators)
	    data.erase(iter);
    });
    
    data = src_data;
    measure(cout, "value-ordered", [&]() { ;
	    for (auto value : elements)
		data.erase(value);
    });

    data = src_data;
    std::reverse(elements.begin(), elements.end());
    measure(cout, "value-reverse", [&]() { ;
	    for (auto value : elements)
		data.erase(value);
    });
    
    data = src_data;
    std::shuffle(elements.begin(), elements.end(), core::rng());
    measure(cout, "value-random", [&]() { ;
	    for (auto value : elements)
		data.erase(value);
    });
    
    return 0;
}
