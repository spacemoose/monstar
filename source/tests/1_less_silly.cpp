#include "monstar.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

/// To understand this example, you may find it easiest to
/// first work through 0_silly_example.
///
/// The monitoring in silly_example.cpp has a couple serious
/// shortcoming, which we will adress in this example
///
///     - Using the interface by setting the state directly with
///       strings is unsafe.  If you accidentally type set_state("dv")
///       instead of set_state("db"), you will only discover your
///       error when you diagnose why you are missign data in ES.
///
///     - If an exception occurs before notifying that a tracked
///       quantity has entered a 'finished' state, the timeseries
///       generator will never know that it's tracked entity has
///       finished, and so should it.
///
///  So we can create a scoped class that solves both of these
///  problems.  In this example we use composition.  A later example
///  will illustrate using inheritence accomplish the same thing.
class TaskMonitor
{
  public:
	enum State { wait, process, db, success, failure };

	/// Creation is notification.
	TaskMonitor(State s, const std::string& owner)
	  : m_msg(make_id(), to_str(s), "foo", "task", {{"owner", owner}})
	{
		monstar::notify(m_msg);
	}

	/// If the destructor is called on an unfinishd message, something
	/// went wrong.
	~TaskMonitor()
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

	monstar::TS_message m_msg;
};

void fake_it(std::string owner, int wait, int proc, int db)
{
	using namespace std::chrono_literals;
	TaskMonitor tm(TaskMonitor::wait, owner);
	std::this_thread::sleep_for(wait * 1s);
	tm.update(TaskMonitor::process);
	std::this_thread::sleep_for(proc * 1s);
	tm.update(TaskMonitor::db);
	std::this_thread::sleep_for(db * 1s);
	tm.update(TaskMonitor::success);
}

void configure_monitoring(int period)
{
	auto server_addy = "127.0.0.1";
	monstar::configure_graphite(server_addy, 2003, "test");
	monstar::configure_elasticsearch(server_addy, 9200, {{"environment", "test"}});
	monstar::initialize_ts_processor(period); /// Let's send messages every second.

	/// @todo better solution for this:
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);
}

int main()
{

	int period = 1;
	/// @todo exception handling.
	configure_monitoring(period);

	/// joe runs a quick task alone:
	fake_it("joe", 1, 1, 1);
	/// same with mary:
	fake_it("mary", 1, 1, 1);

	/// joe and mary run together:
	std::vector<std::thread> threads;
	threads.emplace_back(fake_it, "joe", 1, 1, 1);
	threads.emplace_back(fake_it, "mary", 1, 1, 1);
	for (auto& t : threads) {
		t.join();
	}

	/// Chaos ensues:
	threads.clear();
	threads.emplace_back(fake_it, "joe", 1, 2, 1);
	threads.emplace_back(fake_it, "joe", 2, 2, 2);
	threads.emplace_back(fake_it, "joe", 2, 3, 2);
	threads.emplace_back(fake_it, "mary", 1, 3, 1);
	threads.emplace_back(fake_it, "mary", 2, 4, 2);
	threads.emplace_back(fake_it, "mary", 3, 5, 2);
	std::for_each(threads.begin(), threads.end(), [](auto& t) { t.join(); });

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(period * 1s);
}
