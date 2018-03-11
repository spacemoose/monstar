#pragma once

#include <boost/asio.hpp>
#include <string>

namespace monstar {
namespace detail {

/// Contains everything needed to create a tcp monitoring connection.
/// The instance data must be formatted in a way appropriate to the connection.
///
/// I wanted to implement a move constructor, but boost asio doesn't
/// support move for io_service?
class tcp_service
{

  public:
  /// @param server address of the server being configued.
  /// @param port port number.
  /// @param instance_data must be in a form useful for the service being provided.
	tcp_service(std::string&& server, int port, std::string&& instance_data)
	  : m_server(server)
	  , m_port(std::to_string(port))
	  , m_resolver(m_io_service)
	  , m_query(m_server, m_port)
	  , m_endpoint(*m_resolver.resolve(m_query))
	  , m_instance_data(instance_data)
	{
	}

	boost::asio::ip::tcp::socket get_socket()
	{
		try {
			boost::asio::ip::tcp::socket sock(m_io_service);
			sock.connect(m_endpoint);
			return std::move(sock);
		} catch (std::exception& e) {

			throw std::runtime_error(
			  "Couldn't establish a socket conection to an elasticsearch server, with port " +
			  m_port + " and ip " + m_server + ".   The text was: \n<" + e.what() + ">\n");
		}
	}

	std::string get_instance_data() const { return m_instance_data; }
	std::string get_port() const { return m_port; }
	std::string get_server() const { return m_server; }

  private:
	std::string m_server;
	std::string m_port;
	boost::asio::io_service m_io_service;
	boost::asio::ip::tcp::resolver m_resolver;
	boost::asio::ip::tcp::resolver::query m_query;
	boost::asio::ip::tcp::endpoint m_endpoint;
	std::string m_instance_data;

    /// If fail to connect once, we keep track of it and (for now) don't try again.
    bool m_failed{false};
};
}
}
