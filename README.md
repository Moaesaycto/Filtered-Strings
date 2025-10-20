# `filtered_string_view`

Read this for more information on what the goal of this is: [totw/1](https://abseil.io/tips/1)._

* Defines a lightweight, non-owning class `filtered_string_view` that views a read-only string through an optional filter predicate.
* Stores:

  * A `const char*` pointer to external string data.
  * A `std::size_t` length.
  * A `std::function<bool(const char&)>` predicate determining which chars are visible.
* Defaults predicate to “always true”.
* Provides constructors for `std::string` and `const char*`, with and without custom predicates, plus copy/move constructors.
* Defines copy/move assignment, destructor, and subscript operator (read-only).
* Implements `at()`, `size()`, `empty()`, `data()`, and `predicate()` methods.
* Allows conversion to `std::string`, returning the filtered characters.
* Defines equality (`==`) and three-way comparison (`<=>`) based on filtered content only.
* Overloads `<<` to print the filtered view.
* Adds non-member utilities:

  * `compose()` — combines multiple filters with short-circuiting AND logic.
  * `split()` — splits a filtered view by a delimiter into multiple filtered subviews.
  * `substr()` — returns a filtered substring view by index and length.
* Implements a bidirectional `const_iterator` to traverse filtered characters.
* Adds full iterator/range support: `begin()`, `end()`, `rbegin()`, etc., and their const versions.
* Enforces const-correctness, noexcept where appropriate, and efficiency (no copies or allocations).
* Requires thorough tests verifying all constructors, operators, and behaviours.
