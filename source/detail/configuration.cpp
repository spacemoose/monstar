#include "configuration.hpp"
#include "logging.hpp"

namespace monstar {
namespace detail {

boost::optional<svc_config> configuration::get_config(connection_type ct)
{
	return m_services[static_cast<int>(ct)];
}

opt_svc configuration::make_service(connection_type ct)
{
	auto i = static_cast<int>(ct);
	if (m_services[i]) {
		auto& conf = m_services[i].value();
		if (not conf.disabled) {
			try {
				auto t =
				  std::make_unique<tcp_service>(conf.server, conf.port, conf.instance_data);
				return std::move(t);
			} catch (std::runtime_error& e) {
			  logger()->error("An exception was handled trying to configure the {}  service at {}.  {} based metrics are now disabled. \nThe exception text was:\n\t {} \n", ct, conf, ct, e.what());
				conf.disabled = true;
			}
		}
	}
	return boost::none;
}

void configuration::disable_service(connection_type ct)
{
	auto i = static_cast<int>(ct);
	assert(m_services[i]);
	if (m_services[i]) m_services[i].value().disabled = true;
}

void configuration::configure_service(connection_type ct,
                                      std::string ip,
                                      int port,
                                      std::string instance_data)
{
	m_services[static_cast<int>(ct)].emplace(ip, port, instance_data);
}
}
}
