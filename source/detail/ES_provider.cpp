#include "ES_provider.hpp"
#include "configuration.hpp"
#include "logging.hpp"

#include <iostream>
#include <sstream>


namespace monstar {
namespace detail {

/// We use an optional socket so we only get a single error message,
/// and only if the service was configured but is unavailabe.
ES_provider::ES_provider()
  : m_service(configuration::instance().make_service(connection_type::elastic_search))
{
	if (m_service) {
		m_instance_data = m_service.value()->get_instance_data();
		std::stringstream ss;
		ss << "Host: " + m_service.value()->get_server() + ":" +
		        m_service.value()->get_port() + "\r\n"
		   << "User-Agent: C/1.0\r\n"
		   << "Accept: */*\r\n"
		   << "Content-Type: application/json\r\n"
		   << "Content-Length: ";
		m_const_header_part = ss.str();
	}
}

/// @todo batch processing.
void ES_provider::post_message(const std::string& index,
                               const std::string& type,
                               std::string message)
{
	if (not m_service) return;

	message += m_instance_data;
	thread_local std::stringstream ss;
	ss << "POST /" << index << "/" << type << " HTTP/1.1 \r\n"
	   << m_const_header_part << message.length() + 2 << "\r\n\r\n" // +2 for the {}
	   << "{" << message << "}";
	try {
		m_service.value()->write(ss.str());
		m_service.value()->check_response();
	} catch (std::runtime_error& e) {
		m_service = boost::none;
		auto foo =
		  configuration::instance().get_config(connection_type::elastic_search).value();
		logger()->error("MONSTAR LIB: An exception was handled while trying to post {} data "
		                "to the Elasticsearch index {}  configured with {}.  The metric  has "
		                "been disabled.  The exception text was: \n{}\nThe message attempt was:{}",
		                type,
		                index,
		                foo,
		                e.what(),
						ss.str());
	}
	ss.str(std::string());
}
}
}
