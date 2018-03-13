#include "simple_recorder.hpp"
#include "detail/configuration.hpp"
#include "epoch.hpp"
#include <boost/asio.hpp>
#include <iostream>

namespace monstar {

using namespace detail;
/// The graphite provider is optional, so if graphite is never
/// configured a simple_recorder call does nothing.
simple_recorder::simple_recorder(std::string metric_name)
  : m_prefix(metric_name)
{
	m_socket = configuration::instance().get_socket(connection_type::graphite);
	m_prefix =
	  configuration::instance().get_instance_data(connection_type::graphite) + "." + m_prefix;
}

simple_recorder::~simple_recorder() {}

/// Use this operator to record a value with an automatic timestamp
void simple_recorder::operator()(double val)
{
	if (m_socket) {
		(*this)(epoch::now(), val);
	}
}

/// This is useful for creating fake data, or making sure a set of vals are
/// recorded with an identical timestamp.
/// @todo error handling
void simple_recorder::operator()(int secs_since_epoch, double val)
{
	if (m_socket) {
		m_msg.str("");
		m_msg << m_prefix << " " << val << " " << secs_since_epoch << "\n";
		boost::system::error_code ignored_error;
		boost::asio::write(m_socket.value(), boost::asio::buffer(m_msg.str()), ignored_error);
	}
}
}
