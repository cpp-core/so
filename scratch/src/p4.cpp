// Copyright (C) 2022, 2023 by Mark Melton
//

#include <concepts>
#include <string>
#include <tuple>

template <typename t>
concept type_model = requires(t p_t) {
  typename t::inner;
};

template <typename t, typename u>
concept same_inner = std::is_same_v<typename std::decay_t<t>::inner,
				    typename std::decay_t<u>::inner>;

template <typename t, typename... t_type_model>
concept container_model = (same_inner<t, t_type_model> && ...);

struct type_1 {
  using inner = int;
};

struct type_2 {
  using inner = std::string;
};

template <type_model... t_type_models> struct container {};

template <type_model... t_type_model>
void f(container_model<container<t_type_model...>> auto) {}

using containers_X = container<type_1, type_2>;

static_assert(not same_inner<type_1, type_2>);
static_assert(not container_model<type_1, type_2>);

int main() {

  // containers_X _x;
  // This produces a compilation error as expected.
  // f(_x);

  return 0;
}
