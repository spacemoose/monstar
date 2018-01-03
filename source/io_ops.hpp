/// Provide some useful io tools for monitoring objects.

#pragma once

#include <ostream>
#include <string>

/// @todo this is a quick and dirty hack.  Should test whether any benefit is gained
/// from the __int128 trick and test this method b4 production.
inline std::ostream& operator<<(std::ostream& dest, __int128_t value)
{
	std::ostream::sentry s(dest);
	if (s) {
		__uint128_t tmp = value < 0 ? -value : value;
		char buffer[128];
		char* d = std::end(buffer);
		do {
			--d;
			*d = "0123456789"[tmp % 10];
			tmp /= 10;
		} while (tmp != 0);
		if (value < 0) {
			--d;
			*d = '-';
		}
		int len = std::end(buffer) - d;
		if (dest.rdbuf()->sputn(d, len) != len) {
			dest.setstate(std::ios_base::badbit);
		}
	}
	return dest;
}


inline std::string to_string(__int128_t val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}
