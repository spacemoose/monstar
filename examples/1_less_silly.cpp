#include "monstar.hpp"
#include "notification_handler.hpp"
#include "TaskMonitor.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

monstar::notification_handler setup_monitoring(int period)
{
	auto server_addy = "127.0.0.1";
	monstar::configure_graphite(server_addy, 2003, "foo.test");
	monstar::configure_elasticsearch(server_addy, 9200, {{"environment", "test"}});
	monstar::notification_handler nh;
	nh.start(period);
	return nh;
}

int main()
{

	int period = 1;
	auto nh = setup_monitoring(period);
	monstar::send_annotation("annotations",
	                         "less silly",
	                         "Started a less silly example, this is the text field",
	                         "dummy_tag,dt2");
	/// @todo exception handling.
	monstar::set_notification_handler(nh);

	/// joe runs a quick task alone:
	fake_it("joe", 1, 1, 1);

	/// same with mary:
	fake_it("mary", 1, 1, 1);

	std::cout << "2" << std::endl;
	/// joe and mary run together:
	std::vector<std::thread> threads;
	threads.emplace_back(fake_it, "joe", 1, 1, 1);
	threads.emplace_back(fake_it, "mary", 1, 1, 1);
	for (auto& t : threads) {
		t.join();
	}

	std::cout << "many" << std::endl;
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
