#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <iostream>
#include <set>

TEST_CASE("Default Constructor") {
	fsv::filtered_string_view sv;
	REQUIRE(sv.data() == nullptr);
	REQUIRE(sv.size() == 0);
}

TEST_CASE("Implicit String Constructor with size") {
	auto s = std::string{"cat"};
	auto sv = fsv::filtered_string_view{s};
	REQUIRE(sv.data() == s.data());
	REQUIRE(sv.size() == 3);
}

TEST_CASE("String Constructor with Predicate with size") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{s, pred};
	REQUIRE(sv.data() == s.data());
	REQUIRE(sv.size() == 1);
}

TEST_CASE("Implicit Null-Terminated String Constructor with size") {
	const char* str = "cat";
	auto sv = fsv::filtered_string_view{str};
	REQUIRE(sv.data() == str);
	REQUIRE(sv.size() == 3);
}

TEST_CASE("Null-Terminated String with Predicate Constructor with size") {
	const char* str = "cat";
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{str, pred};
	REQUIRE(sv.data() == str);
	REQUIRE(sv.size() == 1);
}

TEST_CASE("Copy Constructor with data comparison") {
	auto sv1 = fsv::filtered_string_view{"bulldog"};
	const auto copy = sv1;
	REQUIRE(copy.data() == sv1.data());
	REQUIRE(copy.size() == sv1.size());
}

TEST_CASE("Move Constructor with data null check") {
	auto sv1 = fsv::filtered_string_view{"bulldog"};
	const char* original_data = sv1.data();
	const auto original_size = sv1.size();
	const auto move = std::move(sv1);
	REQUIRE(sv1.data() == nullptr);
	REQUIRE(sv1.size() == 0);
	REQUIRE(move.data() == original_data);
	REQUIRE(move.size() == original_size);
}

TEST_CASE("Copy Assignment Operator") {
	auto pred = [](const char& c) { return c == '4' || c == '2'; };
	auto fsv1 = fsv::filtered_string_view{"42 bro", pred};
	auto fsv2 = fsv::filtered_string_view{};
	fsv2 = fsv1;
	REQUIRE(fsv1.data() == fsv2.data());
	REQUIRE(fsv1.size() == fsv2.size());
	REQUIRE(fsv1 == fsv2);
}

TEST_CASE("Subscript Operator") {
	auto pred = [](const char& c) { return c == '9' || c == '0' || c == ' '; };
	auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};

	REQUIRE(fsv1[0] == ' ');
	REQUIRE(fsv1[1] == '9');
	REQUIRE(fsv1[2] == '0');
	REQUIRE(fsv1[3] == ' ');
	REQUIRE(fsv1[4] == ' ');

	REQUIRE(fsv1[5] == '\0');
}

TEST_CASE("String Type Conversion") {
	auto sv = fsv::filtered_string_view("vizsla");
	auto s = static_cast<std::string>(sv);
	REQUIRE(s == "vizsla");
	REQUIRE(sv.data() != s.data());
}

TEST_CASE("at Member Function") {
	auto vowels = std::set<char>{'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
	auto is_vowel = [&vowels](const char& c) { return vowels.find(c) != vowels.end(); };
	auto sv = fsv::filtered_string_view{"Malamute", is_vowel};

	REQUIRE(sv.at(0) == 'a');
	REQUIRE(sv.at(1) == 'a');
	REQUIRE(sv.at(2) == 'u');
	REQUIRE(sv.at(3) == 'e');

	REQUIRE_THROWS_WITH(sv.at(4), "filtered_string_view::at(4): invalid index");
	REQUIRE_THROWS_WITH(sv.at(-1), "filtered_string_view::at(-1): invalid index");

	auto empty_sv = fsv::filtered_string_view{""};
	REQUIRE_THROWS_WITH(empty_sv.at(0), "filtered_string_view::at(0): invalid index");
}

TEST_CASE("size Member Function") {
	auto sv1 = fsv::filtered_string_view{"Maltese"};
	REQUIRE(sv1.size() == 7);

	auto sv2 = fsv::filtered_string_view{"Toy Poodle", [](const char& c) { return c == 'o'; }};
	REQUIRE(sv2.size() == 3);
}

TEST_CASE("empty Member Function") {
	auto sv1 = fsv::filtered_string_view{"Australian Shephard"};
	auto empty_sv = fsv::filtered_string_view{};

	REQUIRE(!sv1.empty());
	REQUIRE(empty_sv.empty());

	auto sv2 = fsv::filtered_string_view{"Border Collie", [](const char& c) { return c == 'z'; }};
	REQUIRE(sv2.empty());
}

TEST_CASE("data Member Function") {
	auto s = "Sum 42";
	auto sv = fsv::filtered_string_view{s, [](const char&) { return false; }};

	const char* ptr = sv.data();
	std::string result;

	while (*ptr) {
		result += *ptr;
		++ptr;
	}

	REQUIRE(result == "Sum 42");
}

TEST_CASE("predicate Member Function") {
	// Changed from example to avoid printing line in case it screws up auto tests.
	// That case was manually tested.
	const auto print_and_return_true = [](const char&) { return true; };
	const auto print_and_return_false = [](const char&) { return false; };

	const auto s = fsv::filtered_string_view{"doggo", print_and_return_true};

	const auto& pred = s.predicate();
	REQUIRE(pred('c'));

	const auto s2 = fsv::filtered_string_view{"doggo", print_and_return_false};
	const auto& pred2 = s2.predicate();
	REQUIRE(!pred2('c'));
}

TEST_CASE("Equality Comparison Operator") {
	auto const lo = fsv::filtered_string_view{"aaa"};
	auto const hi = fsv::filtered_string_view{"zzz"};

	REQUIRE(lo != hi);
	REQUIRE(!(lo == hi));

	auto const lo_same = fsv::filtered_string_view{"aaa"};
	REQUIRE(lo == lo_same);
	REQUIRE(!(lo != lo_same));

	auto const filtered_1 = fsv::filtered_string_view{"abcdef", [](const char& c) { return c == 'a' || c == 'b'; }};
	auto const filtered_2 = fsv::filtered_string_view{"abxxxx", [](const char& c) { return c == 'a' || c == 'b'; }};
	REQUIRE(filtered_1 == filtered_2);
}

TEST_CASE("Relational Operators Using Spaceship Operator") {
	auto const lo = fsv::filtered_string_view{"aaa"};
	auto const hi = fsv::filtered_string_view{"zzz"};

	REQUIRE(lo < hi);
	REQUIRE(lo <= hi);
	REQUIRE(!(lo > hi));
	REQUIRE(!(lo >= hi));
	REQUIRE((lo <=> hi) == std::strong_ordering::less);

	auto const lo_same = fsv::filtered_string_view{"aaa"};
	REQUIRE(!(lo < lo_same));
	REQUIRE(lo <= lo_same);
	REQUIRE(!(lo > lo_same));
	REQUIRE(lo >= lo_same);
	REQUIRE((lo <=> lo_same) == std::strong_ordering::equal);

	auto const filtered_1 = fsv::filtered_string_view{"abcdef", [](const char& c) { return c == 'a' || c == 'b'; }};
	auto const filtered_2 = fsv::filtered_string_view{"abxxxx", [](const char& c) { return c == 'a' || c == 'b'; }};
	REQUIRE(filtered_1 == filtered_2);
	REQUIRE((filtered_1 <=> filtered_2) == std::strong_ordering::equal);
}

TEST_CASE("Output Stream Operator") {
	auto fsv = fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'c' || c == '+'; }};
	std::ostringstream oss;
	oss << fsv;
	REQUIRE(oss.str() == "c++");
}

TEST_CASE("Compose Function") {
	auto best_languages = fsv::filtered_string_view{"c / c++"};
	auto vf = std::vector<fsv::filter>{[](const char& c) { return c == 'c' || c == '+' || c == '/'; },
	                                   [](const char& c) { return c > ' '; },
	                                   [](const char&) { return true; }};

	auto sv = fsv::compose(best_languages, vf);
	REQUIRE(sv == "c/c++");
}

TEST_CASE("Split Function") {
	auto interest = std::set<char>{'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', ' ', '/'};
	auto sv = fsv::filtered_string_view{"0xDEADBEEF / 0xdeadbeef",
	                                    [&interest](const char& c) { return interest.contains(c); }};
	auto tok = fsv::filtered_string_view{" / "};
	auto v = fsv::split(sv, tok);

	REQUIRE(v.size() == 2);
	REQUIRE(v[0] == "DEADBEEF");
	REQUIRE(v[1] == "deadbeef");

	sv = fsv::filtered_string_view{"xax"};
	tok = fsv::filtered_string_view{"x"};
	v = fsv::split(sv, tok);
	REQUIRE(v.size() == 3);

	REQUIRE(v[0] == "");
	REQUIRE(v[1] == "a");
	REQUIRE(v[2] == "");

	sv = fsv::filtered_string_view{"xx"};
	tok = fsv::filtered_string_view{"x"};
	v = fsv::split(sv, tok);
	REQUIRE(v.size() == 3);
	REQUIRE(v[0] == "");
	REQUIRE(v[1] == "");
	REQUIRE(v[2] == "");
}

TEST_CASE("Substr Function") {
	auto sv = fsv::filtered_string_view{"Siberian Husky"};
	auto substr_sv = fsv::substr(sv, 9);
	REQUIRE(substr_sv == "Husky");

	auto is_upper = [](const char& c) { return std::isupper(static_cast<unsigned char>(c)); };
	sv = fsv::filtered_string_view{"Sled Dog", is_upper};
	substr_sv = fsv::substr(sv, 0, 2);
	REQUIRE(substr_sv == "SD");

	sv = fsv::filtered_string_view{"Siberian Husky"};
	substr_sv = fsv::substr(sv, 0, 0);
	REQUIRE(substr_sv == "Siberian Husky");
}

TEST_CASE("Iterator Functionality") {
	auto print_via_iterator = [](fsv::filtered_string_view const& sv) {
		std::ostringstream oss;
		std::copy(sv.begin(), sv.end(), std::ostream_iterator<char>(oss, " "));
		return oss.str();
	};

	auto fsv1 = fsv::filtered_string_view{"corgi"};
	REQUIRE(print_via_iterator(fsv1) == "c o r g i ");

	auto fsv2 = fsv::filtered_string_view{"samoyed", [](const char& c) {
		                                      return !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
	                                      }};
	auto it = fsv2.begin();
	std::ostringstream oss;
	oss << *it << *std::next(it) << *std::next(it, 2) << *std::next(it, 3);
	REQUIRE(oss.str() == "smyd");

	const auto str = std::string("tosa");
	const auto s = fsv::filtered_string_view{str};
	auto it_end = s.cend();
	std::ostringstream oss_end;
	oss_end << *std::prev(it_end) << *std::prev(it_end, 2);
	REQUIRE(oss_end.str() == "as");
}

TEST_CASE("Range Functionality") {
	auto s1 = fsv::filtered_string_view{"puppy", [](const char& c) { return !(c == 'u' || c == 'y'); }};
	auto v1 = std::vector<char>{s1.begin(), s1.end()};
	REQUIRE(v1 == std::vector<char>{'p', 'p', 'p'});

	auto s2 = fsv::filtered_string_view{"milo", [](const char& c) { return !(c == 'i' || c == 'o'); }};
	auto v2 = std::vector<char>{s2.rbegin(), s2.rend()};
	REQUIRE(v2 == std::vector<char>{'l', 'm'});

	auto s3 = fsv::filtered_string_view{"racecar", [](const char& c) { return c != 'e'; }};
	auto v3 = std::vector<char>{s3.cbegin(), s3.cend()};
	REQUIRE(v3 == std::vector<char>{'r', 'a', 'c', 'c', 'a', 'r'});

	auto v4 = std::vector<char>{s3.crbegin(), s3.crend()};
	REQUIRE(v4 == std::vector<char>{'r', 'a', 'c', 'c', 'a', 'r'});
}

TEST_CASE("TEST 1") {
	auto is_upper = [](const char& c) { return std::isupper(static_cast<unsigned char>(c)); };
	auto sv = fsv::filtered_string_view{"Sled Dog", is_upper};
	std::cout << fsv::substr(sv, 0, 2);
}
