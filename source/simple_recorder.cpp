#include "simple_recorder.hpp"
#include "detail/configuration.hpp"
#include "detail/tcp_service.hpp"
#include "epoch.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <boost/optional.hpp>

namespace monstar {

/// For reasons I don't understand, gcc 7.2 insists on constructing a
/// default deleter for simple_recorder, even though I've defined
/// one.  This is a workaround for that.
namespace detail {
void tcps_deleter::operator()(tcp_service* p) { delete p; }
}

using namespace detail;

/// Because of the wierd behavior in gcc 7.2, we have to take the
/// optional<uptr> and pack it into an optional<special_unique>
boost::optional<special_unique> repack(boost::optional<std::unique_ptr<tcp_service>>& old)
{
	if (not old) {
		return boost::none;
	}
	std::unique_ptr<detail::tcp_service, detail::tcps_deleter> newp;
	newp.reset(old.value().release());
	return std::move(newp);
}

/// The graphite provider is optional, so if graphite is never
/// configured a simple_recorder call does nothing.
simple_recorder::simple_recorder(std::string metric_name)
  : m_prefix(metric_name)
{
	auto temp = configuration::instance().make_service(connection_type::graphite);
	m_service = repack(temp);

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
			std::cerr << "MONSTAR LIB: An exception was handled while trying to record "
			          << m_prefix << " to the graphite server at "
			          << m_service.value()->get_server()
			          << ".  The metric has been disabled.  The error message was: "
			          << e.what() << std::endl;
			m_service = boost::none;
			configuration::instance().disable_service(connection_type::graphite);
		}
	}
}

simple_recorder::~simple_recorder(){};
}
