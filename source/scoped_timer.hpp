#include <detail/GraphiteRecorder.hpp>

/// This provides a RAII style benchmarker for recording timings in graphite.
/// Example:
///
///    int foo() {
///        scoped_timer("your.graphite.path")
///        // do your work.
///    }
///
/// Every time foo leaves scope,  the scoped timer
class scoped_timer
{
  public:
	scoped_timer(const char* path)
	  : m_recorder(path)
	  ,m_start_time(std::chrono::system_clock::now())
	{
	}
    ~scoped_timer() {m_recorder(std::chrono::system_clock::now - m_start_time); }

  private:
	GraphiteRecorder m_recorder;
	std::chrono::duration<double, std::milli> m_start_time;
}
