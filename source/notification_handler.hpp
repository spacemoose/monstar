#pragma once

#include "notification.hpp"
#include <thread>

namespace monstar {

namespace detail{
  class TS_processor;
}

/// When first constructed, the notification_handler
/// provides a 'null' implementation -- all notifications are
/// silently swallowed.  This is to allow for disabled or
/// disabling monitoring trivially.
///
/// To enable monitoring, you have to configure graphite and
/// elasticsearch, and call start().  The lifetime of the service is
/// determined by the scope of the instantiated  object.
///
/// Note that this class must live longer than any calls to
/// monstar::notify().
///
/// Still @todo:  threadsafe pointer to null if monitoring fails (e.g. timeout on
class notification_handler
{
  public:

	/// This must be called before any call to notify, and after
	/// configure_* methods.  It initializes the timeseries processor
	/// which receives notification and sends timeseries data to
	/// elasticsearch.
	void start(int period);

	void add(const notification& note);
	~notification_handler();
	notification_handler();
	notification_handler(notification_handler&& nh) noexcept;
	notification_handler(const notification_handler&) = delete;

  private:
	void processing_loop();
	std::unique_ptr<detail::TS_processor> m_ts_processor;
	std::thread m_thread;
	bool m_in_destructor{false};
};

}
