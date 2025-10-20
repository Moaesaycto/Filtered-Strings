#include "./filtered_string_view.h"
#include "filtered_string_view.h"

#include <compare>
#include <iostream>
#include <vector>

namespace fsv {
	filter filtered_string_view::default_predicate = [](const char&) { return true; };

	// Default Constructor
	filtered_string_view::filtered_string_view() noexcept
	: _ptr(nullptr)
	, _length(0)
	, _predicate(default_predicate) {}

	// Implicit String Constructor
	filtered_string_view::filtered_string_view(const std::string& str)
	: _ptr(str.data())
	, _length(str.size())
	, _predicate(default_predicate) {}

	// String Constructor with Predicate
	filtered_string_view::filtered_string_view(const std::string& str, filter predicate)
	: _ptr(str.data())
	, _length(str.size())
	, _predicate(predicate) {}

	// Implicit Null-Terminated String Constructor
	filtered_string_view::filtered_string_view(const char* str)
	: _ptr(str)
	, _length(std::strlen(str))
	, _predicate(default_predicate) {}

	// Null-Terminated String with Predicate Constructor
	filtered_string_view::filtered_string_view(const char* str, filter predicate)
	: _ptr(str)
	, _length(std::strlen(str))
	, _predicate(predicate) {}

	// Copy Constructor
	filtered_string_view::filtered_string_view(const filtered_string_view& other) noexcept
	: _ptr(other._ptr)
	, _length(other._length)
	, _predicate(other._predicate) {}

	// Move Constructor
	filtered_string_view::filtered_string_view(filtered_string_view&& other) noexcept
	: _ptr(other._ptr)
	, _length(other._length)
	, _predicate(other._predicate) {
		other._ptr = nullptr;
		other._length = 0;
		other._predicate = default_predicate;
	}

	// Copy Assignment Operator
	auto filtered_string_view::operator=(const filtered_string_view& other) noexcept -> filtered_string_view& {
		if (this != &other) {
			_ptr = other._ptr;
			_length = other._length;
			_predicate = other._predicate;
		}
		return *this;
	}

	// Move Assignment Operator
	auto filtered_string_view::operator=(filtered_string_view&& other) noexcept -> filtered_string_view& {
		if (this != &other) {
			_ptr = other._ptr;
			_length = other._length;
			_predicate = std::move(other._predicate);

			other._ptr = nullptr;
			other._length = 0;
			other._predicate = default_predicate;
		}
		return *this;
	}

	// Subscript Operator
	auto filtered_string_view::operator[](int n) -> const char& {
		int index = 0;
		for (const char* p = _ptr; p != _ptr + _length; ++p) {
			if (_predicate(*p)) {
				if (index == n) {
					return *p;
				}
				++index;
			}
		}

		static const char default_char = '\0';
		return default_char;
	}

	// String Type Conversion Operator
	filtered_string_view::operator std::string() const {
		std::string result;
		result.reserve(size());
		for (const char* p = _ptr; p != _ptr + _length; ++p) {
			if (_predicate(*p)) {
				result += *p;
			}
		}
		return result;
	}

	// at Member Function
	auto filtered_string_view::at(int index) -> const char& {
		if (index < 0 || static_cast<std::size_t>(index) >= size()) {
			throw std::domain_error("filtered_string_view::at(" + std::to_string(index) + "): invalid index");
		}
		int filtered_index = 0;
		for (const char* p = _ptr; p != _ptr + _length; ++p) {
			if (_predicate(*p)) {
				if (filtered_index == index) {
					return *p;
				}
				++filtered_index;
			}
		}
		throw std::domain_error("filtered_string_view::at(" + std::to_string(index) + "): invalid index");
	}

	// size Member Function
	auto filtered_string_view::size() const -> std::size_t {
		return static_cast<std::size_t>(std::count_if(_ptr, _ptr + _length, _predicate));
	}

	// empty Member Function
	auto filtered_string_view::empty() -> bool {
		return size() == 0;
	}

	// data Member Function
	const char* filtered_string_view::data() const {
		return _ptr;
	}

	// predicate Member Function
	auto filtered_string_view::predicate() const -> const filter& {
		return _predicate;
	}

	// Equality Comparison Operator
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool {
		auto lhs_filtered = static_cast<std::string>(lhs);
		auto rhs_filtered = static_cast<std::string>(rhs);
		return lhs_filtered == rhs_filtered;
	}

	// Not Equality Comparison Operator
	auto operator!=(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool {
		return !(lhs == rhs);
	}

	// Spaceship Operator
	auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering {
		auto lhs_filtered = static_cast<std::string>(lhs);
		auto rhs_filtered = static_cast<std::string>(rhs);
		return lhs_filtered <=> rhs_filtered;
	}

	// Output Stream Operator
	auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream& {
		for (const char* p = fsv._ptr; p != fsv._ptr + fsv._length; ++p) {
			if (fsv._predicate(*p)) {
				os.put(*p);
			}
		}
		return os;
	}

	// Compose function
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view {
		auto composed_predicate = [filts](const char& c) {
			for (const auto& f : filts) {
				if (!f(c)) {
					return false;
				}
			}
			return true;
		};
		return filtered_string_view(fsv.data(), composed_predicate);
	}

	// Split function
	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view> {
		std::vector<filtered_string_view> result;
		const char* fsv_start = fsv.data();
		const char* fsv_end = fsv_start + std::strlen(fsv.data());
		const char* tok_start = tok.data();
		std::size_t tok_size = std::strlen(tok.data());

		if (tok_size == 0 || fsv.size() == 0) {
			result.push_back(fsv);
			return result;
		}

		const char* segment_start = fsv_start;
		while (segment_start < fsv_end) {
			const char* tok_pos = std::search(segment_start, fsv_end, tok_start, tok_start + tok_size);

			if (segment_start != tok_pos) {
				result.emplace_back(segment_start, [segment_start, tok_pos, &fsv](const char& c) {
					return &c >= segment_start && &c < tok_pos && fsv.predicate()(c);
				});
			}
			else {
				result.emplace_back("", [](const char&) { return false; });
			}

			if (tok_pos == fsv_end) {
				break;
			}

			segment_start = tok_pos + tok_size;
		}

		if (segment_start == fsv_end) {
			result.emplace_back("");
		}

		return result;
	}

	// Substring Function
	auto substr(const filtered_string_view& fsv, int pos, int count) -> filtered_string_view {
		std::size_t total_size =
		    static_cast<std::size_t>(std::count_if(fsv.data(), fsv.data() + std::strlen(fsv.data()), fsv.predicate()));
		if (pos < 0 || static_cast<std::size_t>(pos) >= total_size) {
			return filtered_string_view("", [](const char&) { return false; });
		}
		std::size_t rcount = (count <= 0) ? (total_size - static_cast<std::size_t>(pos))
		                                  : static_cast<std::size_t>(count);
		const char* start = fsv.data();
		int index = 0;
		const char* substr_start = nullptr;
		const char* substr_end = nullptr;

		for (const char* p = start; *p != '\0'; ++p) {
			if (fsv.predicate()(*p)) {
				if (index == pos) {
					substr_start = p;
				}
				if (index == pos + static_cast<int>(rcount)) {
					substr_end = p;
					break;
				}
				++index;
			}
		}

		if (substr_start == nullptr) {
			return filtered_string_view("", [](const char&) { return false; });
		}
		if (substr_end == nullptr) {
			substr_end = start + std::strlen(fsv.data());
		}

		return filtered_string_view(substr_start, [substr_start, substr_end, &fsv](const char& c) {
			return &c >= substr_start && &c < substr_end && fsv.predicate()(c);
		});
	}

	// Iterator Default Constructor
	filtered_string_view::iter::iter() noexcept
	: _ptr(nullptr)
	, _pred(&filtered_string_view::default_predicate) {}

	// Iterator Constructor with Predicate
	filtered_string_view::iter::iter(const char* str, const filter* pred) noexcept
	: _ptr(str)
	, _pred(pred) {
		if (_ptr && !_pred->operator()(*_ptr)) {
			advance();
		}
	}

	// Iterator Dereference Operator
	auto filtered_string_view::iter::operator*() const -> reference {
		return *_ptr;
	}

	// Iterator Arrow Operator
	auto filtered_string_view::iter::operator->() const -> pointer {
		return _ptr;
	}

	// Iterator Pre-Increment Operator
	auto filtered_string_view::iter::operator++() -> iter& {
		advance();
		return *this;
	}

	// Iterator Post-Increment Operator
	auto filtered_string_view::iter::operator++(int) -> iter {
		iter tmp = *this;
		advance();
		return tmp;
	}

	// Iterator Pre-Decrement Operator
	auto filtered_string_view::iter::operator--() -> iter& {
		retreat();
		return *this;
	}

	// Iterator Post-Decrement Operator
	auto filtered_string_view::iter::operator--(int) -> iter {
		iter tmp = *this;
		retreat();
		return tmp;
	}

	// Iterator Equality Comparison Operator
	auto filtered_string_view::iter::operator==(const iter& other) const noexcept -> bool {
		return _ptr == other._ptr;
	}

	// Iterator Inequality Comparison Operator
	auto filtered_string_view::iter::operator!=(const iter& other) const noexcept -> bool {
		return !(*this == other);
	}

	// Advance Iterator to Next Valid Position
	void filtered_string_view::iter::advance() {
		do {
			++_ptr;
		} while (_ptr && !_pred->operator()(*_ptr));
	}

	// Retreat Iterator to Previous Valid Position
	void filtered_string_view::iter::retreat() {
		do {
			--_ptr;
		} while (_ptr && !_pred->operator()(*_ptr));
	}

	// Begin Iterator
	auto filtered_string_view::begin() const -> const_iterator {
		return const_iterator(_ptr, &_predicate);
	}

	// End Iterator
	auto filtered_string_view::end() const -> const_iterator {
		return const_iterator(_ptr + _length, &_predicate);
	}

	// Constant Begin Iterator
	auto filtered_string_view::cbegin() const -> const_iterator {
		return begin();
	}

	// Constant End Iterator
	auto filtered_string_view::cend() const -> const_iterator {
		return end();
	}

	// Reverse Begin Iterator
	auto filtered_string_view::rbegin() const -> const_reverse_iterator {
		return const_reverse_iterator(end());
	}

	// Reverse End Iterator
	auto filtered_string_view::rend() const -> const_reverse_iterator {
		return const_reverse_iterator(begin());
	}

	// Constant Reverse Begin Iterator
	auto filtered_string_view::crbegin() const -> const_reverse_iterator {
		return rbegin();
	}

	// Constant Reverse End Iterator
	auto filtered_string_view::crend() const -> const_reverse_iterator {
		return rend();
	}
} // namespace fsv
