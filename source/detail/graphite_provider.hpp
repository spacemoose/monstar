/// A class to store and post messages to carbon/graphite
#pragma once

#include <boost/asio.hpp>
#include <string>

namespace monstar {
namespace detail {

/// A class for posting data to our monitoring backend.
/// @todo batch sending & buffering?
class graphite_provider
{
  public:
	/// Will throw a std::runtime_error if no socket connection to a
	/// graphite server could be established.
	graphite_provider();
    void operator()(std::string msg);

    static void set_server(std::string ip, int port);
    static void set_prefix(const std::string& pfx){m_prefix=pfx;}
    static std::string get_prefix() {return m_prefix;}
  private:
	boost::asio::io_service m_ios;
	boost::asio::ip::tcp::socket m_sock{m_ios};
	static std::string m_graphite_ip;
	static std::string m_graphite_port;
	static std::string m_prefix;
};
}
}
