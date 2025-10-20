# Filtered String View

**Type:** C++ Utility (Header-only) · **Tech:** C++20, STL · **Status:** Completed

## **Overview:**
A lightweight, non-owning view over a char buffer that exposes only characters passing a user-supplied predicate; zero allocations, const-correct, and efficient.

## **Key Features**

* Stores `const char*`, `std::size_t`, and `std::function<bool(char)>` (default accepts all).
* Constructors from `std::string` and `const char*`, with or without custom predicates; copy/move ops; dtor.
* Safe access: `operator[]` (read-only), `at()`, `size()`, `empty()`, `data()`, `predicate()`.
* Conversion to `std::string` returns filtered content.
* Comparisons: `==` and `<=>` compare filtered content only.
* Streaming: `operator<<` prints the filtered view.
* Iteration: bidirectional `const_iterator`; full range support (`begin/end`, `cbegin/cend`, `rbegin/rend`).
* Utilities: `compose(preds...)`, `split(view, delim)`, `substr(view, pos, count)`.
* Marked `noexcept` where appropriate; no copies of underlying data.

## **Example**

```cpp
// predicates
auto is_alpha = [](char c){ return std::isalpha(static_cast<unsigned char>(c)); };
auto not_space = [](char c){ return c != ' '; };

// compose filters
auto alpha_no_space = compose(is_alpha, not_space);

// construct views
filtered_string_view v1{"ab c! 123"}; // default: all chars visible
filtered_string_view v2{std::string{"ab c! 123"}, alpha_no_space}; // "abc"

// iteration
for (char c : v2) { /* use c */ }

// substring and split
auto sub = substr(v2, 0, 2); // "ab"
for (auto part : split(filtered_string_view{"a,b,c", [](char){return true;}}, ',')) {
    std::string s = static_cast<std::string>(part); // "a", "b", "c"
}

// comparisons use filtered content
filtered_string_view x{"A 1", is_alpha}; // "A"
filtered_string_view y{"A",   is_alpha}; // "A"
bool eq = (x == y); // true
```

## **Tests**

* Cover constructors, copy/move, iteration, accessors, conversions, comparisons, streaming, and utilities (`compose/split/substr`), including edge cases (empty view, out-of-range `at()`, all-filtered-out).
