#include "simple_recorder.hpp"
#include "detail/configuration.hpp"
#include "detail/tcp_service.hpp"
#include "epoch.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <optional>

namespace monstar {

using namespace detail;
using opt_svc = std::optional<detail::tcp_service>;

/// The graphite provider is optional, so if graphite is never
/// configured a simple_recorder call does nothing.
simple_recorder::simple_recorder(std::string metric_name)
  : m_prefix(metric_name),
	m_service(configuration::instance().make_service(connection_type::graphite))
{
	if (m_service) {
		m_prefix = m_service.value()->get_instance_data() + "." + m_prefix;
	}
}

/// Use this operator to record a value with an automatic timestamp
void simple_recorder::operator()(double val)
{
	if (m_service) {
		(*this)(epoch::now(), val);
	}
}

/// This is useful for creating fake data, or making sure a set of vals are
/// recorded with an identical timestamp.
/// @todo error handling
void simple_recorder::operator()(int secs_since_epoch, double val)
{
	if (m_service) {
		m_msg.str("");
		m_msg << m_prefix << " " << val << " " << secs_since_epoch << "\n";

		try {
			m_service.value()->write(m_msg.str());
		} catch (std::runtime_error& e) {
			std::cerr << "MONSTAR LIB: An exception was handled while trying to record " << m_prefix
			          << " to the graphite server at "
			          << m_service.value()->get_server()
			          << ".  The metric has been disabled.  The error message was: "
			          << e.what() << std::endl;
			m_service = std::nullopt;
			configuration::instance().disable_service(connection_type::graphite);
		}
	}
}

simple_recorder::~simple_recorder(){};

}
