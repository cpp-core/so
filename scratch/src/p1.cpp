// Copyright (C) 2022, 2023 by Mark Melton
//

#include <iostream>

using std::cin, std::cout, std::endl;

struct Human {
};

struct Male : public Human {
};

struct Female : public Human {
};

// some functions can handle all humans
void human_func(Human& h) {
    // ...
}

// some only take a subset of humans
void male_func(Male& m) {
    // ...
}

void female_func(Female& m) {
    // ...
}

// and user only uses values of Human as token

int main() {
    auto human = Human{};
    human_func(human);
    // male_func(human); This would not compile
    // female_func(human); This would not compile

    auto male = Male{};
    human_func(male);
    male_func(male);
    // female_func(male); This would not compile

    auto female = Female{};
    human_func(female);
    // male_func(female); This would not compile
    female_func(female);
}
