#include "monstar.hpp"
#include "notification.hpp"
#include "scoped_timer.hpp"
#include "simple_recorder.hpp"
#include "notification_handler.hpp"
#include "TaskMonitor.hpp"

#include <iostream>
#include <thread>

void bad_elasticsearch()
{
	monstar::configure_elasticsearch("silly.path", 9200, {{"environmnent", "test"}});
}
void bad_graphite() { monstar::configure_graphite("silly.path", 2003, "foo.test"); }

void good_graphite() { monstar::configure_graphite("127.0.0.1", 2003, "foo.test"); }
void good_elasticsearch()
{
	monstar::configure_elasticsearch("127.0.0.1", 9200, {{"environmnent", "test"}});
}


monstar::notification_handler start_nh(int period)
{
	monstar::notification_handler nh;
	nh.start(period);
	return nh;
}

/// run through all the various monstar features.
void use_monstar()
{
	std::cout << "one," << std::endl;
	monstar::send_annotation("index", "title", "text", "tags");
	std::cout << "two," << std::endl;
	monstar::notification note(1, "new", "idx", "type", {{"foo", "bar"}});
	std::cout << "three," << std::endl;
	monstar::notify(note);
	std::cout << "four," << std::endl;
	for (int i = 0; i < 10; ++i) {
		monstar::simple_recorder sr("some_metric");
		sr(i);
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}

	fake_it("blob", 1, 1, 1);
	{
		std::cout << "six\n" << std::endl;
		monstar::scoped_timer st("timer");
	}
}

void both_bad()
{
	std::cout << "ES and Graphite unavailable.  Log some lines, then silent.\n\n";
	std::cout << "calling bad graphite\n";
	bad_graphite();
	std::cout << "calling bad es\n";
	bad_elasticsearch();
	use_monstar();
	std::cout << "this should be next and last line you see " << std::endl;
}




int main() {
  good_graphite();
  bad_elasticsearch();
  start_nh(1);
  use_monstar();
}
