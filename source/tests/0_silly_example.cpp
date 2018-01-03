#include "monstar.hpp"
#include <chrono>
#include <string>
#include <thread>
#include <iostream>


/// Here I inform the api about the monitoring setup.  In this case
/// (modify as needed) we:
///
///   - have a Graphite server on the loopback device, default port.
///   - have an Elasticsearch server on the loopback device, default port.
///   - Distinguish between a test and a production environment.
///     In Elasticsearch this means applying a field with the key
///     "environment" and value "test" to all generated Elasticsearch
///     data.  In Graphite it means assing "test" to the path where
///     the data is stored.
///
///
/// which means we add it to the graphite path, and initialize the
/// elastic-search system-info.
void configure_monitoring(int period)
{
	auto server_addy="127.0.0.1";
	monstar::configure_graphite(server_addy, 2003, "test");
	monstar::configure_elasticsearch(server_addy, 9200, {{"environment", "test"}});
	monstar::initialize_ts_processor(period); /// Let's send messages every second.
	/// @todo get a signal from the initializer here, rather than just sleep.
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(1s);
}

/// This example builds up a test case that introduces the minimal
/// features of the API.  This is not how you will want to implement a
/// real solution, but lets you get started with a minimal set of
/// concepts.
///
/// Consider we have a system which runs 'tasks'.  Each
/// task has an owner, and might be processing, waiting for user
/// input, or interacting with a database.  Further, we run two
/// instances of our system -- one in a test environment, the other
/// gets run in a production environment.
///
/// Our first step is to create a data mapping for elasticsearch.  The
/// following can be copy pasted (minus the '\' escape character
/// before the '@') into kibana to define the mapping in the
/// elasticsearch server:
///
/// PUT /foo
/// {
///   "mappings":
///   { "task":
///   { "properties":
///   {
///     "owner":          {"type":"keyword"},
///     "environment":    {"type":"keyword"},
///     "\@timestamp":     {"type":"date", "format": "epoch_second"},
///     "\@process":       {"type":"integer"},
///     "\@db":            {"type":"integer"},
///     "\@wait":          {"type":"integer"},
///     "status":         {"type":"keyword"}
///   }
///   }
///   }
/// }
///
/// Here we've created a mapping that lets us store a count of time
/// spend in any of the states (\@db, \@process, or \@wait).  We track
/// the owner of the task, and the environment in which the system is
/// running.  The fields "status" and \@timestamp are required by the api to indicate
///
///  We'll create two 'tasks', and move them through various states.
int main()
{
  	int period = 3;
    configure_monitoring(period);

	/// Each message needs a numeric (int128) identifier so
	/// the TS processor knows which timeseries generator it has to
	/// update.

	/// Create joe's message , starting in state "wait".
	using namespace std::chrono_literals;
	monstar::TS_message joes(1, "wait", "foo", "task", {{"owner", "joe"}});

	/// The time-series processor won't see the message until it's notified.
	monstar::notify(joes);

	/// Same for mary's message, but it starts in state "process".
	monstar::TS_message marys(2, "process", "foo", "task", {{"owner", "mary"}});
	monstar::notify(marys);

	/// Let's pretend these tasks stay in their states for 3s until
	/// joes task also starts processing.
	std::this_thread::sleep_for(3s);
	monstar::notify(joes.set_state("process"));

	/// both process for 2s.
	std::this_thread::sleep_for(2s);


	/// Both go into 'db' mode -- joes for 1s (and then finished),
	/// while marys goes for 2.
	monstar::notify(joes.set_state("db"));
	monstar::notify(marys.set_state("db"));
	std::this_thread::sleep_for(1s);
	monstar::notify(joes.set_finished());
	std::this_thread::sleep_for(1s);
	monstar::notify(marys.set_finished());

	/// make sure the ts_processor has time to send off final messages.
	std::this_thread::sleep_for(static_cast<std::chrono::seconds>(period));

	return 0;
}
