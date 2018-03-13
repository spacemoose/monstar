#include "simple_recorder.hpp"
#include <chrono>

namespace monstar {

/// This provides a RAII style benchmarker for recording timings in graphite.
/// Example:
///
///    int foo() {
///        scoped_timer("your.graphite.path")
///        // do your work.
///    }
///
/// Every time foo leaves scope, the scoped timer will record the
/// current timed value.  Note, that as is, this will clobber any
/// entry in the same time interval in Graphite.  Other options coming.
class scoped_timer
{
  public:
	scoped_timer(const char* path)
	  : m_recorder(path)
	  , m_start_time(std::chrono::system_clock::now())
	{
	}
	~scoped_timer()
	{
		m_recorder(std::chrono::duration_cast<std::chrono::milliseconds>(
		             std::chrono::system_clock::now() - m_start_time)
		             .count());
	}
	scoped_timer(const scoped_timer&) = delete;
	scoped_timer& operator=(const scoped_timer&) = delete;

  private:
	simple_recorder m_recorder;
	std::chrono::time_point<std::chrono::system_clock> m_start_time;
};
}
