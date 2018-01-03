#pragma once

#include <deque>
#include <mutex>
#include <thread>

namespace monstar {
namespace detail {

/// Provides a Multile Producer, Single Consumer safe queue.  Here, count and pop are
/// always called by the same thread, while push might be called from any number of
/// threads.
template<typename T>
class MPSC_queue
{
  public:
	/// Called concurrently:
	void push(T t)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_data.push_back(t);
	}

	/// Only called by consumer thread.
	size_t count()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_data.size();
	}

	/// Only called by consumer thread.
	T pop()
	{
		auto retval = m_data.front();
		m_data.pop_front();
		return retval;
	}

  private:
	std::mutex m_mutex;
	std::deque<T> m_data;
};
}
}
