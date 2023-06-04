// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>
#include <vector>
#include <string>
#include "core/timer/timer.h"

using std::cout, std::endl;

typedef std::vector<std::vector<int>> Vec2i;

void Vec2DPrinter(Vec2i vec) {
    size_t vec_h = vec.size();
    for (size_t y = 0; y < vec_h; y++) {
        size_t vec_w = vec.at(y).size();
        for (size_t x = 0; x < vec_w; x++)
            std::cout << vec.at(y).at(x);
        std::cout << std::endl;
    }
}

void Vec2DFiller(Vec2i &dest, Vec2i src, int dest_x, int dest_y) {
    int src_h = (int)src.size();

    // Loop to fill dest vector with elements of src vector at specific coords
    for (int y = 0; y < src_h; y++) {
        int src_w = (int)src.at(y).size();
        int dest_h = (int)dest.size();

        if ((y + dest_y) >= dest_h)
            break;

        if ((y + dest_y) < 0)
            continue;

        int dest_w = (int)dest.at(y + dest_y).size();

        for (int x = 0; x < src_w; x++) {
            if (x + dest_x >= dest_w)
                break;

            if (x + dest_x < 0)
                continue;

            dest.at(y + dest_y).at(x + dest_x) = src.at(y).at(x);
        }
    }
}

void Vec2DFiller2(Vec2i &dest, const Vec2i& src, int dest_x, int dest_y) {
    int ylen = std::min(src.size(), dest.size() > dest_y ? dest.size() - dest_y : 0);
    if (ylen == 0)
        return;
    
    int xlen = std::min(src.at(0).size(), dest.at(0).size() - dest_x);
    if (xlen == 0)
        return;

    for (auto ydx = 0; ydx < ylen; ++ydx) {
        auto& ydest = dest.at(ydx + dest_y);
        const auto& ysrc = src.at(ydx);
        std::copy(ysrc.data(), ysrc.data() + xlen, ydest.data() + dest_x);
    }
}

int main(void)
{
    // This is just visual example (i know how to fill vectors)
    Vec2i dest =
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    };

    Vec2i src =
    {
        { 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1 }
    };

    std::cout << "Destination contents (before change):" << std::endl;
    Vec2DPrinter(dest);
    std::cout << "\nSurce contents:" << std::endl;
    Vec2DPrinter(src);

    // Destination coords
    int dest_x = 3;
    int dest_y = 4;

    // Key function to fill one vector with another
    auto ns = core::timer::Timer().run(1'000'000, [&]() {
	Vec2DFiller(dest, src, dest_x, dest_y);
    }).elapsed_per_iteration();
    cout << ns << " ns / op" << endl;

    ns = core::timer::Timer().run(1'000'000, [&]() {
	Vec2DFiller2(dest, src, dest_x, dest_y);
    }).elapsed_per_iteration();
    cout << ns << " ns / op" << endl;
    
    std::cout << "\nDestination contents (after change):" << std::endl;
    Vec2DPrinter(dest);

    return 0;
}
