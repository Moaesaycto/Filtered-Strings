#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <compare>
#include <cstring>
#include <functional>
#include <iterator>
#include <optional>
#include <string>

#include <algorithm>

namespace fsv {
	using filter = std::function<bool(const char&)>;

	class filtered_string_view {
		class iter {
		 public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using difference_type = std::ptrdiff_t;
			using pointer = const char*;
			using reference = const char&;

			iter() noexcept;
			iter(const char* str, const filter* pred) noexcept;

			auto operator*() const -> reference;
			auto operator->() const -> pointer;

			auto operator++() -> iter&;
			auto operator++(int) -> iter;
			auto operator--() -> iter&;
			auto operator--(int) -> iter;

			auto operator==(const iter& other) const noexcept -> bool;
			auto operator!=(const iter& other) const noexcept -> bool;

		 private:
			const char* _ptr;
			const filter* _pred;

			void advance();
			void retreat();
		};
		using const_iterator = iter;
		using const_reverse_iterator = std::reverse_iterator<iter>;

	 public:
		static filter default_predicate;

		const char* data() const;

		// Constructors
		filtered_string_view() noexcept;
		filtered_string_view(const std::string& str);
		filtered_string_view(const std::string& str, filter predicate);
		filtered_string_view(const char* str);
		filtered_string_view(const char* str, filter predicate);

		// Copy and Move Constructors
		filtered_string_view(const filtered_string_view& other) noexcept;
		filtered_string_view(filtered_string_view&& other) noexcept;

		// Destructor
		~filtered_string_view() = default;

		// Member Operators
		auto operator=(const filtered_string_view& other) noexcept -> filtered_string_view&;
		auto operator=(filtered_string_view&& other) noexcept -> filtered_string_view&;
		auto operator[](int n) -> const char&;
		explicit operator std::string() const;

		// Member Functions
		auto at(int index) -> const char&;
		auto empty() -> bool;
		auto size() const -> std::size_t;
		auto predicate() const -> const filter&;

		// Non-Member Operators
		friend auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;
		friend auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering;
		friend auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream&;

		// Range
		auto begin() const -> const_iterator;
		auto end() const -> const_iterator;
		auto cbegin() const -> const_iterator;
		auto cend() const -> const_iterator;
		auto rbegin() const -> const_reverse_iterator;
		auto rend() const -> const_reverse_iterator;
		auto crbegin() const -> const_reverse_iterator;
		auto crend() const -> const_reverse_iterator;

	 private:
		const char* _ptr;
		std::size_t _length;
		filter _predicate;
	};

	// Non-Member Utility Functions
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view;
	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view>;
	auto substr(const filtered_string_view& fsv, int pos = 0, int count = 0) -> filtered_string_view;

} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
