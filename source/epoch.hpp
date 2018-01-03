#pragma once
#include <string>
#include <sstream>


#include <chrono>

namespace monstar{
/// Functions for returning seconds since epoch.
namespace epoch {

using timestamp_t = unsigned long long; // for seconds since epoch.

constexpr std::chrono::hours operator ""_d(unsigned long long days)
{
    return std::chrono::hours(24*days);
}

namespace sc = std::chrono;

inline unsigned long now()
{
    auto foo = sc::duration_cast<sc::seconds>(
        sc::system_clock::now().time_since_epoch());
    return foo.count();
}

// template <typename T>
// unsigned long now(T val)
// {
//     auto foo = sc::duration_cast<sc::seconds>(
//         sc::system_clock::now().time_since_epoch() + val);
//     return foo.count();
// }

}  // ns epoch
}
