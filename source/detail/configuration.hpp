#pragma once

#include "tcp_service.hpp"

#include <iostream>
#include <optional>
#include <utility>

namespace monstar {
namespace detail {

enum class connection_type { graphite = 0, elastic_search };

using opt_svc = std::optional<std::unique_ptr<tcp_service>>;

inline std::ostream& operator<<(std::ostream& ostr, connection_type ct)
{
	switch (ct) {
		case connection_type::graphite: ostr << "graphite"; break;
		case connection_type::elastic_search: ostr << "elastic_search"; break;
	}
	return ostr;
}

struct svc_config {
	svc_config(const std::string& s, int p, std::string& ind)
	  : instance_data(ind)
	  , server(s)
	  , port(p){};
	std::string instance_data;
	std::string server;
	int port;
	bool disabled{false};
};

inline std::ostream& operator<<(std::ostream& ostr, const svc_config& c)
{
	ostr << c.server << ":" << c.port << ":{" << c.instance_data << "}";
	return ostr;
}

/// This provides the Monstar configuration to any library calls.
///
/// If a library calls a service which has not been configured, the
/// optional value will be empty, and the library will simply do
/// nothing silently.
///
/// If a library component attempts to use a service and fails, it
/// calls disable_service, so the error only gets handled and logged
/// once.
///
/// A singleton seemed like a reasonable approach here.
///
/// @todo thread safe disable_service so timeouts can globally disable?
class configuration
{

  public:
	std::optional<svc_config> get_config(connection_type ct)
	{
		return m_services[static_cast<int>(ct)];
	}

	/// If the service has been configured (and isn't disabled) we try
	/// to create the service object.
	///
	/// If the service fails to connect, we disable the service.
	///
	/// Note that the disabled flag just prevents from trying to newly
	/// instantiate a connection to a disabled service.  Concrete
	/// service providers will want to handle timeouts and disable
	/// their optional services.
	opt_svc make_service(connection_type ct)
	{
		auto i = static_cast<int>(ct);
		if (m_services[i]) {
			auto& conf = m_services[i].value();
			if (not conf.disabled) {
				try {
					auto t = std::make_unique<tcp_service>(
					  conf.server, conf.port, conf.instance_data);
					return std::move(t);
				} catch (std::runtime_error& e) {
					std::cerr << "MONSTAR-LIB:  An exception was handled trying to configure the "
					          << ct << " service at " << conf << ".  " << ct
					          << " based metrics are now disabled. \n "
					          << "The exception text was:\n\t < " << e.what() << ">\n";
					conf.disabled = true;
				}
			}
		}
		return std::nullopt;
	}
	static configuration& instance()
	{
		static configuration c;
		return c;
	}

	void disable_service(connection_type ct)
	{
		auto i = static_cast<int>(ct);
		assert(m_services[i]);
		if (m_services[i]) m_services[i].value().disabled = true;
	}

	void configure_service(connection_type ct,
	                       std::string ip,
	                       int port,
	                       std::string instance_data)
	{
		m_services[static_cast<int>(ct)].emplace(ip, port, instance_data);
	}

  private:
	std::array<std::optional<svc_config>, 2> m_services;
};
}
}
