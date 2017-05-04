#pragma once

#include <string>
#include <cctype>
#include <sstream>
#include <algorithm>
#include <iterator>


namespace Jasper {

bool FindInString(const std::string& needle, const std::string& haystack);



inline bool FindInString(const std::string& needle, const std::string& haystack) {
	auto it = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(), [&](char a, char b) {
		return std::toupper(a) == std::toupper(b);
	});
	if (it != haystack.end()) {
		return true;
	}
	return false;
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

inline std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

}