/// A class to store and post messages to carbon/graphite
#pragma once

#include <boost/asio.hpp>
#include <string>

namespace monstar {
namespace detail {

using btcp = boost::asio::ip::tcp;
namespace basio = boost::asio;

/// A class for posting data to our monitoring backend.
/// @todo batch sending?
class graphite_poster
{
  public:
	/// Will throw a std::runtime_error if no socket connection to a
	/// graphite server could be established.
	graphite_poster()
	{
		try {
			btcp::resolver resolver(m_ios);
			btcp::resolver::query query(graphite_ip, graphite_port);
			btcp::endpoint endpoint = *resolver.resolve(query);
			m_sock.connect(endpoint);
		} catch (std::exception& e) {
			throw std::runtime_error("An exception was caught while trying to establish a "
			                         "socket conection to a graphite server, with port " +
			                         graphite_port + " and ip " + graphite_ip +
			                         ". \n The exception text was: " + e.what());
		}
	}

	/// @todo error handling.
	void operator()(std::string msg)
	{
		boost::system::error_code ignored_error;
		basio::write(m_sock, basio::buffer(msg), ignored_error);
	}

	static void set_server(std::string ip, int port)
	{
		graphite_ip = ip;
		graphite_port = std::to_string(port);
	}
	static void set_prefix(std::string pre) { prefix = pre; }

  private:
	basio::io_service m_ios;
	btcp::socket m_sock{m_ios};
	static std::string graphite_ip;
	static std::string graphite_port;
	static std::string prefix;
};
}
}
