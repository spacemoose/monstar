#pragma once

#include "detail/graphite_poster.hpp"
#include "epoch.hpp"
#include <sstream>
#include <string>

namespace monstar {
namespace detail {


/// For storing some numeric informatoin (only to Graphite currently),
/// at the current timestamp.
class simple_recorder
{
  public:
	/// a single recorder is used to record a single value.
	/// @todo Configuration!
	simple_recorder(std::string metric_name)
	  : m_metric_name("." + metric_name)
	{
	}

	simple_recorder(const simple_recorder& gr)
	  : m_metric_name(gr.m_metric_name)
	{

	}

	/// Use this operator to record a value with an automatic timestamp
	void operator()(double val) { this->operator()(epoch::now(), val); }

	/// This is useful for creating fake data, or making sure a set of vals are
	/// recorded with an identical timestamp.
	void operator()(int secs_since_epoch, double val)
	{
		m_msg.str("");
		m_msg << m_metric_name << " " << val << " " << secs_since_epoch << "\n";
		m_poster(m_msg.str());
	}

	const std::string m_metric_name;

  private:
	/// @todo get from configuration.
	/// @todo single carbon connection, batch posting.
		graphite_poster m_poster;
	std::stringstream m_msg;
};
}
}
