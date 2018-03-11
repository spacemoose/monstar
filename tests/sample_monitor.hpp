#include "monstar.hpp"
#include "notification_handler.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <string>
#include <thread>

/// Same thing I did in 1_less_silly.cpp.
class sample_monitor
{
  public:
	enum State {  wait, process, db, success, failure };

	/// Creation is notification.
	sample_monitor(State s, const std::string& owner)
	  : m_msg(make_id(), to_str(s), "foo", "task", {{"owner", owner}})
	{
		monstar::notify(m_msg);
	}

	/// If the destructor is called on an unfinished message, something
	/// went wrong.
	~sample_monitor()
	{
		if (not m_msg.finished()) {
			m_msg.set_state("error");
			monstar::notify(m_msg.set_finished());
		}
	}

	void update(State s)
	{
		m_msg.set_state(to_str(s));
		if (s == success || s == failure) {
			m_msg.set_finished();
		}
		monstar::notify(m_msg);
	}

  private:
	monstar::msg_id_t make_id()
	{
		auto id = boost::uuids::random_generator();
		monstar::msg_id_t retval;
		memcpy(&retval, &id, 16);
		return retval;
	}

	std::string to_str(State s)
	{
		switch (s) {
			case wait: return "wait";
			case process: return "process";
			case db: return "db";
			case success: return "success";
			case failure: return "failure";
		}
		assert(true);
		return "default";
	}

	monstar::notification m_msg;
};
