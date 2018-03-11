#pragma once

#include "tcp_service.hpp"

#include <iostream>
#include <optional>
#include <utility>

namespace monstar {
namespace detail {

using opt_socket = std::optional<boost::asio::ip::tcp::socket>;

enum class connection_type { graphite = 0, elastic_search };

inline std::ostream& operator<<(std::ostream& ostr, connection_type ct)
{
	switch (ct) {
		case connection_type::graphite: ostr << "graphite"; break;
		case connection_type::elastic_search: ostr << "elastic_search"; break;
	}
	return ostr;
}

/// This provides the Monstar configuration to any library calls.
///
/// If a library calls a service which has not been enabled, the
/// optional value will be empty, and the library will simply do
/// nothing silently.
///
/// A singleton seemed like a reasonable approach here.
class configuration
{

  public:
	opt_socket get_socket(connection_type ct)
	{
		try {
			auto i = static_cast<int>(ct);
			if (m_services[i]) {
				auto& svc = m_services[i].value();
				return std::move(svc.get_socket());
			}
		} catch (std::exception& e) {
			std::cerr << "An exception was handled while trying to connect to the " << ct
			          << ".  " << ct
			          << " based metrics are now disabled.  The exception's text was:  \n"
			          << e.what() << std::endl;
		}
		return std::nullopt;
	}

	std::string get_instance_data(connection_type ct)
	{
		auto i = static_cast<int>(ct);
		return (m_services[i]) ? m_services[i].value().get_instance_data() : "";
	}

	std::string get_server(connection_type ct)
	{
		auto i = static_cast<int>(ct);
		if (m_services[i]) {
			return m_services[i].value().get_server();
		} else
			return "";
	}

	std::string get_port(connection_type ct)
	{
		auto i = static_cast<int>(ct);
		if (m_services[i]) {
			return m_services[i].value().get_port();
		} else
			return "";
	}

	static configuration& instance()
	{
		static configuration c;
		return c;
	}

	/// @todo error handling on failure to connect :
	/// if configure is called, and we are unable to establish a connection, then we log an
	/// error message.  Thereafter, calls to the library will see the optional connection
	/// as false, and we act like monstar is disabled.
	/// @param instance_data is a string containing the 'global' instance data, formatted
	///        appropriately for the service in question.
	void configure_service(connection_type ct,
	                       std::string ip,
	                       int port,
	                       std::string instance_data)
	{
		try {
			auto i = static_cast<int>(ct);
			m_services[i].emplace(
			  std::forward<std::string>(ip), port, std::forward<std::string>(instance_data));
		} catch (std::runtime_error& e) {
			std::cerr << "Monstar:  An exception was handled trying to configure the " << ct
			          << " service at " << ip << ":" << port
			          << " with instance data  = " << instance_data << ".  The "
			          << " service is now disabled.\n "
			          << "The exception text was:\n\t < " << e.what() << ">\n";
		}
	}

  private:
	std::array<std::optional<tcp_service>, 2> m_services;
};
}
}

/* @todo still have to initialize defaults.
ES:
std::string ES_provider::m_server{"127.0.0.1"};
std::string ES_provider::m_port{"9200"};
std::string ES_provider::m_instance_data;
*/
