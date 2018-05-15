#pragma once

#include <mutex>
#include <thread>
#include <iostream>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

/// It seems like this needs to be included before spdlog incusion if
/// you want custom types.  Is that right?  Maybe make a pull request
/// for that one.
#include <fmt/ostream.h>
#include <spdlog.h>
namespace monstar {
namespace detail {

static std::once_flag logger_flag;

/// This lets me use the default console logger, without worrying
/// about when or if the monstar::add_log_sink method has been
/// called.
inline void make_and_register()
{
    std::cout  << "Creating the logger" << std::endl;
	auto console = spdlog::stdout_color_mt("MONSTAR");
//	spdlog::register_logger(console);
}

inline std::shared_ptr<spdlog::logger> logger()
{
  std::call_once(logger_flag, [](){make_and_register();});
  return spdlog::get("MONSTAR");
}

}
}
