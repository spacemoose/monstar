#pragma once

#include "tcp_service.hpp"

#include <boost/optional.hpp>
#include <iostream>
#include <memory>
#include <utility>

namespace monstar {
namespace detail {

enum class connection_type { graphite = 0, elastic_search };

using opt_svc = boost::optional<std::unique_ptr<tcp_service>>;

template<typename OStream>
OStream& operator<<(OStream& ostr, connection_type ct)
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

template<typename ostr_t>
ostr_t& operator<<(ostr_t& ostr, const svc_config& c)
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
/// @todo thread safe disable_service so timeouts can globally
/// disable?
///
/// @todo configuration should be namespace level functions,
/// and we can have a hidden services object.
class configuration
{

  public:
	static configuration& instance()
	{
		static configuration c;
		return c;
	}

	boost::optional<svc_config> get_config(connection_type ct);

	/// If the service has been configured (and isn't disabled) we try
	/// to create the service object.
	///
	/// If the service fails to connect, we disable the service.
	///
	/// Note that the disabled flag just prevents from trying to newly
	/// instantiate a connection to a disabled service.  Concrete
	/// service providers will want to handle timeouts and disable
	/// their optional services.
	opt_svc make_service(connection_type ct);
	void disable_service(connection_type ct);
	void configure_service(connection_type ct,
	                       std::string ip,
	                       int port,
	                       std::string instance_data);

  private:
	std::array<boost::optional<svc_config>, 2> m_services;
};
}
}
