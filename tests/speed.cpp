/// @file To help testing and tuning throughput.  Just a quick hack for now.

#include "monstar.hpp"
#include "sample_monitor.hpp"

monstar::notification_handler setup_monitoring(int period)
{
	auto server_addy = "127.0.0.1";
	monstar::configure_graphite(server_addy, 2003, "foo.test");
	monstar::configure_elasticsearch(server_addy, 9200, {{"environment", "test"}});
	monstar::notification_handler nh;
	nh.start(period);
	monstar::set_notification_handler(nh);
	// notificaton_handler suports move, not copy, so this should be safe:g
	return nh;
}

std::vector<sample_monitor> make_n_monitors(unsigned int num)
{
	std::vector<sample_monitor> retval;
	retval.reserve(num);
	while (--num > 0) {
	  retval.push_back(sample_monitor(sample_monitor::wait, "bob"));
	}
	return retval;
}

/// Fake running three seconds worth of data for N different
void fake_n(int num)
{
	std::vector<sample_monitor> monitors = make_n_monitors(num);
	std::for_each(
	  monitors.begin(), monitors.end(), [](auto& m) { m.set_state(sample_monitor::process); });
	std::for_each(
	  monitors.begin(), monitors.end(), [](auto& m) { m.set_state(sample_monitor::db); });
	std::for_each(
	  monitors.begin(), monitors.end(), [](auto& m) { m.set_state(sample_monitor::success); });
}

/// Here I want to see what kind of throughput I can handle per
/// second.  So I start one producer thread and one consumer thread.
/// With each second I increase the number of notifications produced.

int main()
{
	auto nh = setup_monitoring();

	constexpr int start = 100;
	constexpr int stop = 100000;
	int num = start;
	while (num < stop) {
		fake_n(num);
		num = num * 1.5;
	}
}
