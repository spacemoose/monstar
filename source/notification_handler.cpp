#include "notification_handler.hpp"
#include "detail/TS_processor.hpp"

namespace monstar{

/// This should only be called after all notifications are done.
notification_handler::~notification_handler()
{
	m_in_destructor = true;
	m_thread.join();
}

void notification_handler::start(int period)
{
	assert(not m_ts_processor);
	m_ts_processor = std::make_unique<detail::TS_processor>(period);
	m_thread = std::thread(&notification_handler::processing_loop, this);
}

/// This assumes that no calls to notify are made after calling the
/// notification handler's destructor.  Otherwise difficulties arise.
void notification_handler::processing_loop()
{
	while (not m_in_destructor) {
		m_ts_processor->process();
		/// currently the ts_processor manages the period, but maybe I
		/// should change that.
	}
}

notification_handler::notification_handler(notification_handler&& nh) noexcept :
  m_ts_processor(std::move(nh.m_ts_processor)),
  m_thread(std::move(nh.m_thread)),
  m_in_destructor(std::move(nh.m_in_destructor))
{}

/// Having a nullptr for TS_processor is not considered an error.
/// In the event of a failure we want the system to continue
/// running unaffected (minus monitoring of course).
/// the possiblity of a nullptr on ts_processor to allow disabling
/// of messaging, either intentionally or because a connection
/// problem occured , and we want do nothing.
///
/// @todo handle/test not-so-fringe cases (must be thread safe):
///   - server connection timeout.
void notification_handler::add(const notification& note)
{
	assert(not m_in_destructor);
	if (not m_ts_processor) {
		return;
	}
	m_ts_processor->add_message(note);
}

}
