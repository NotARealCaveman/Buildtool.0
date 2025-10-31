#pragma once
#include <string>
#include <array>

#include "typenames.h"

namespace Manifest_Utility
{
	template<MFu8 MAX_SIZE>
	struct FixedString
	{
		constexpr FixedString() = default;
		constexpr FixedString(const char* value)
		{
			assign(value);
		}
		constexpr FixedString(const std::string& value)
		{
			assign(value);
		}

		constexpr FixedString operator=(const std::string& sv)
		{
			FixedString result;
			result.assign(sv);

			return result;
		}

		std::array<char, MAX_SIZE> buffer{};
		MFu8 size{ 0 };//buff can never be larger than 255

		// Assign from C-string
		constexpr void assign(const char* string)
		{
			std::size_t i = 0;
			for (; string[i] != '\0' && i < MAX_SIZE - 1; ++i) {
				buffer[i] = string[i];
			}
			buffer[i] = '\0';
			size = i;
		}

		// Assign from string_view
		constexpr void assign(const std::string& string) {
			std::size_t i = 0;
			for (; i < string.size() && i < MAX_SIZE - 1; ++i) {
				buffer[i] = string[i];
			}
			buffer[i] = '\0';
			size = i;
		}

		// Accessors
		constexpr const char* c_str() const noexcept { return buffer.data(); }
		constexpr std::string_view view() const noexcept { return std::string_view{ buffer.data(), size }; }
		constexpr std::size_t length() const noexcept { return size; }
		constexpr std::size_t capacity() const noexcept { return MAX_SIZE; }

		// Debug printing
		constexpr operator std::string_view() const noexcept { return view(); }
	};
}