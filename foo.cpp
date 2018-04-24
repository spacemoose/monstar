#include "spdlog/include/spdlog/spdlog.h"

struct foo {

int main()

{


  auto console = spdlog::stdout_color_mt("moo");
  console = spdlog::basic_logger_mt("monstar", "log.txt");
  console->info("yo");
}
