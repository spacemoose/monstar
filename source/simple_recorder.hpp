#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <sstream>
#include <string>

namespace monstar {

/// For storing some numeric information (only to Graphite currently),
/// at the current timestamp.
class simple_recorder
{
  public:
	/// a single recorder is used to record a single value.
	simple_recorder(std::string metric_name);
	simple_recorder(const simple_recorder& gr);
	~simple_recorder();

	/// Use this operator to record a value with an automatic timestamp
	void operator()(double val);

	/// This is useful for creating fake data, or making sure a set of vals are
	/// recorded with an identical timestamp.
	void operator()(int secs_since_epoch, double val);

  private:
	std::optional<boost::asio::ip::tcp::socket> m_socket;
	std::stringstream m_msg;
	std::string m_prefix;

};
}
