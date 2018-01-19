
#include "graphite_provider.hpp"

namespace monstar {
namespace detail {

std::string graphite_provider::m_graphite_ip{"127.0.0.1"};
std::string graphite_provider::m_graphite_port{"2003"};
std::string graphite_provider::m_prefix{""};

namespace basio = boost::asio;

graphite_provider::graphite_provider()
{
	using btcp = boost::asio::ip::tcp;
	try {
		btcp::resolver resolver(m_ios);
		btcp::resolver::query query(m_graphite_ip, m_graphite_port);
		btcp::endpoint endpoint = *resolver.resolve(query);
		m_sock.connect(endpoint);
	} catch (std::exception& e) {
		throw std::runtime_error(
		  "Couldn't establish a socket conection to a graphite server, with port" +
		  m_graphite_port + " and ip " + m_graphite_ip + ".  Text was:\n< " + e.what() +
		  ">\n");
	}
}

/// @todo error handling.
void graphite_provider::operator()(std::string msg)
{
	boost::system::error_code ignored_error;
	basio::write(m_sock, basio::buffer(msg), ignored_error);
}

void graphite_provider::set_server(std::string ip, int port)
{
	m_graphite_ip = ip;
	m_graphite_port = std::to_string(port);
}
}
}
