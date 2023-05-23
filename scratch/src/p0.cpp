// Copyright (C) 2022, 2023 by Mark Melton
//

#include <variant>
#include <iostream>

// Pattern match a `std::variant` derived type against its contained type.
//
// match helpers
namespace detail {
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}; // detail

template<class T, class... Us>
auto match(T&& variant, Us&&... handlers) {
    return std::visit(detail::overloaded{std::forward<Us>(handlers)...}, std::forward<T>(variant));
}

// Logic symobls using unicode.
namespace uch {
inline constexpr auto True = "\u22a4";
inline constexpr auto False = "\u22a5";
inline constexpr auto Negation = "\u00ac";
inline constexpr auto Conjunction = "\u2227";
inline constexpr auto Disjunction = "\u2228";
inline constexpr auto Implies = "\u2192";
inline constexpr auto Equivalence = "\u2194";
};

// Declare the Proposition types.
struct Variable;
struct True;
struct False;
struct Negation;
struct Conjunction;
struct Disjunction;
struct Implies;
struct Equivalence;

// The variant type that holds a Proposition.
using Base = std::variant<
    Variable,
    True,
    False,
    Negation,
    Conjunction,
    Disjunction,
    Implies,
    Equivalence
    >;

// All Proposition's are shared pointers.
using Proposition = std::shared_ptr<Base>;

// Define the Proposition types.
struct Variable {
    Variable(const std::string& v) : name(v) { }
    std::string name;
};

struct True { };

struct False { };

struct Negation {
    Negation(Proposition p) : child(p) { }
    Proposition child;
};

struct Conjunction {
    Conjunction(Proposition p, Proposition q) : left(p), right(q) { }
    Proposition left, right;
};

struct Disjunction {
    Disjunction(Proposition p, Proposition q) : left(p), right(q) { }
    Proposition left, right;
};
struct Implies {
    Implies(Proposition p, Proposition q) : left(p), right(q) { }
    Proposition left, right;
};

struct Equivalence {
    Equivalence(Proposition p, Proposition q) : left(p), right(q) { }
    Proposition left, right;
};

// Helpers for creating Propositions.
template<class T, class... Args>
Proposition proposition(Args... args) {
    return std::make_shared<Base>(T{args...});
}

Proposition variable(const std::string& v) {
    return proposition<Variable>(v);
}
    
Proposition ptrue() {
    return proposition<True>();
}

Proposition pfalse() {
    return proposition<False>();
}

Proposition negation(Proposition child) {
    return proposition<Negation>(child);
}

Proposition conjunction(Proposition l, Proposition r) {
    return proposition<Conjunction>(l, r);
}

Proposition disjunction(Proposition l, Proposition r) {
    return proposition<Disjunction>(l, r);
}

Proposition implies(Proposition l, Proposition r) {
    return proposition<Implies>(l, r);
}

Proposition equivalence(Proposition l, Proposition r) {
    return proposition<Equivalence>(l, r);
}

// Variable literals.
Proposition operator ""_v(const char *s, size_t) {
    std::string str(s);
    return variable(str);
}

// Use the builtin operators to construct Propositions.
auto operator!(Proposition p) {
    return negation(p);
}

auto operator&&(Proposition p, Proposition q) {
    return conjunction(p, q);
}

auto operator||(Proposition p, Proposition q) {
    return disjunction(p, q);
}

auto operator==(Proposition p, Proposition q) {
    return equivalence(p, q);
}

// Output a Proposition
std::ostream& operator<<(std::ostream& os, Proposition p) {
    match
	(*p,
	 [&](const Variable& v) {
	     os << v.name;
	 },
	 [&](const True&) {
	     os << uch::True;
	 },
	 [&](const False&) {
	     os << uch::False;
	 },
	 [&](const Negation& p) {
	     os << uch::Negation << " " << p.child;
	 },
	 [&](const Conjunction& p) {
	     os << "(" << p.left << " " << uch::Conjunction << " " << p.right << ")";
	 },
	 [&](const Disjunction& p) {
	     os << "(" << p.left << " " << uch::Disjunction << " " << p.right << ")";
	 },
	 [&](const Implies& p) {
	     os << "(" << p.left << " " << uch::Implies << " " << p.right << ")";
	 },
	 [&](const Equivalence& p) {
	     os << "(" << p.left << " " << uch::Equivalence << " " << p.right << ")";
	 },
	 [&](const auto&) {
	     os << "???";
	 }
	 );
    return os;
}

// Simplification
Proposition simplify_not(Proposition child) {
    return match
	(*child,
	 [](const True& f) { return pfalse(); },
	 [](const False& f) { return ptrue(); },
	 [&](const Negation& f) { return f.child; },
	 [&](const auto&) { return negation(child); }
	 );
}

Proposition simplify_and(Proposition left, Proposition right) {
    if (std::holds_alternative<False>(*left) or std::holds_alternative<False>(*right))
	return pfalse();
    else if (std::holds_alternative<True>(*left) and std::holds_alternative<True>(*right))
	return ptrue();
    else
	return conjunction(left, right);
}

Proposition simplify_or(Proposition left, Proposition right) {
    if (std::holds_alternative<False>(*left) and std::holds_alternative<False>(*right))
	return pfalse();
    else if (std::holds_alternative<True>(*left) or std::holds_alternative<True>(*right))
	return ptrue();
    else
	return disjunction(left, right);
}

Proposition simplify(Proposition node) {
    return match
	(*node,
	 [&](const Negation& f) {  return simplify_not(simplify(f.child)); },
	 [&](const Conjunction& f) { return simplify_and(simplify(f.left), simplify(f.right)); },
	 [&](const Disjunction& f) { return simplify_or(simplify(f.left), simplify(f.right)); },
	 [&](const auto&) { return node; }
	 );
}

using std::cout, std::endl;

int main(int argc, const char *argv[]) {
    auto f = !(ptrue() || "A"_v);
    auto g = simplify(f);
    cout << f << endl;
    cout << g << endl;
    return 0;
}
