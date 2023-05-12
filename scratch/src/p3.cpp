// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <stack>
#include <vector>

struct Interval {
    Interval(int s, int e, const std::string& l)
        : start(s)
        , end(e)
        , label(l) {
    }
    int start, end;
    std::string label;
};

using Intervals = std::vector<Interval>;
using Stack = std::stack<Interval>;

using std::cout, std::endl;

void dump_stack(Stack stack) {
    while (stack.size()) {
	auto [a, b, label] = stack.top();
	stack.pop();
	cout << a << " " << b << " " << label << endl;
    }
}

int main(int argc, const char *argv[]) {
    // Intervals vals = {
    //     { 0, 3, "foo" },
    //     { 2, 4, "bar" }
    // };
    Intervals vals = {
        { 0, 3, "foo" },
        { 0, 4, "bar" },
	{ 0, 5, "que" }
    };

    // Sort the intervals by their start value and end value.
    std::sort(vals.begin(), vals.end(), [](const auto& a, const auto& b) {
        return (a.start < b.start) or (a.start == b.start and a.end < b.end);
        return false;
    });

    // Place the intervals on a stack such that the first is on top.
    Stack stack(vals.rbegin(), vals.rend());

    Intervals results;
    // While there are at least two intervals on the stack, operate on
    // the first two intervals.
    while (stack.size() > 1) {
	// cout << "stack: " << endl;
	// dump_stack(stack);
	
        auto a = stack.top(); stack.pop();
        auto b = stack.top(); stack.pop();

        // If there is no overalap, output the first interval and
        // return the second interval to the stack.
        if (a.end <= b.start) {
            results.emplace_back(a);
            stack.push(b);
        }
        // If and b start at the same point, we have to adjust all
        // intervals that start at that point.
        else if (a.start == b.start) {
            auto end = std::min(a.end, b.end);
            std::string label = a.label + "-" + b.label;
            Stack tmp;
            while (stack.size() and stack.top().start == a.start) {
                auto next = stack.top();
                label += "-" + next.label;
                stack.pop();
                tmp.emplace(end, next.end, next.label);
            }
            while (tmp.size()) {
                stack.emplace(tmp.top());
                tmp.pop();
            }
            stack.emplace(end, b.end, b.label);
            results.emplace_back(a.start, end, label);
        }
        // If a and b partially overlap, we have three intervals
        // (a.start, b.start), (b.start, a.end) and (a.end,
        // b.end). Output the first and push the next two back to the
        // stack.
        else if (a.end > b.start and b.end > a.end) {
            results.emplace_back(a.start, b.start, a.label);
            stack.emplace(a.end, b.end, b.label);
            stack.emplace(b.start, a.end, a.label + "-" + b.label);
        }
        // If b is a suffix of a, we have two intervals (a.start,
        // b.start) and (b.start, a.end). Output the first and push
        // the second on the stack.
        else if (a.end > b.start and a.end == b.end) {
            results.emplace_back(a.start, b.start, a.label);
            stack.emplace(b.start, a.end, a.label + "-" + b.label);
        }
        // If b is complete covered by a, we have three intervals
        // (a.start, b.start), (b.start, b.end), and (b.end, a.end).
        else if (a.end > b.start and a.end > b.end) {
            results.emplace_back(a.start, b.start, a.label);
            stack.emplace(b.end, a.end, a.label);
            stack.emplace(b.start, b.end, a.label + "-" + b.label);
	}
    }

    if (stack.size() > 0)
	results.emplace_back(stack.top());

    for (const auto& [a, b, label] : results)
	cout << a << " " << b << " " << label << endl;
    
    return 0;
}
