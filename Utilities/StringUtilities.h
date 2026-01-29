#pragma once
#include "pch.h"
#include <algorithm>
#include <cctype>
#include <ranges>

class StringUtilities {
public:
	[[nodiscard]] static vector<string> Split(string input, char delimiter) {
		vector<string> result;
		size_t index = 0;
		size_t lastIndex = 0;
		while ((index = input.find(delimiter, index)) != string::npos) {
			result.push_back(input.substr(lastIndex, index - lastIndex));
			index++;
			lastIndex = index;
		}
		result.push_back(input.substr(lastIndex));
		return result;
	}

	[[nodiscard]] static string TrimLeft(string str) {
		size_t startIndex = str.find_first_not_of("\t ");
		if (startIndex == string::npos) {
			return "";
		} else if (startIndex > 0) {
			return str.substr(startIndex);
		}
		return str;
	}

	[[nodiscard]] static string TrimRight(string str) {
		size_t endIndex = str.find_last_not_of("\t\r\n ");
		if (endIndex == string::npos) {
			return "";
		} else if (endIndex > 0) {
			return str.substr(0, endIndex + 1);
		}
		return str;
	}

	[[nodiscard]] static string Trim(string str) {
		return TrimLeft(TrimRight(str));
	}

	[[nodiscard]] static string ToUpper(string str) {
		std::ranges::transform(str, str.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
		return str;
	}

	[[nodiscard]] static string ToLower(string str) {
		std::ranges::transform(str, str.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		return str;
	}

	static void CopyToBuffer(string str, char* outBuffer, uint32_t maxSize) {
		memcpy(outBuffer, str.c_str(), std::min<uint32_t>((uint32_t)str.size(), maxSize));
	}

	[[nodiscard]] static bool StartsWith(string& str, const char* content) {
		return str.starts_with(content);
	}

	[[nodiscard]] static bool EndsWith(string& str, const char* content) {
		return str.ends_with(content);
	}

	[[nodiscard]] static bool Contains(string& str, const char* content) {
		return str.contains(content);
	}

	[[nodiscard]] static string GetString(char* src, uint32_t maxLen) {
		return GetString((uint8_t*)src, maxLen);
	}

	[[nodiscard]] static string GetString(uint8_t* src, int maxLen) {
		for (int i = 0; i < maxLen; i++) {
			if (src[i] == 0) {
				return string(src, src + i);
			}
		}
		return string(src, src + maxLen);
	}
};
