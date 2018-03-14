#pragma once

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <iostream>
#include <string>

namespace monstar {
namespace detail {

/// Contains everything needed to create a tcp monitoring connection.
/// The instance data must be formatted in a way appropriate to the connection.
///
/// boost asio has pretty restrictive ownership semantics.  Since this owns all the boost asio
/// stuff, I make it unmoveable and uncopyable -- transfer ownership by moving a unique ptr.
class tcp_service
{

  public:
	/// Constructor will throw an exception if it could not connect.
	/// @param server address of the server being configued.
	/// @param port port number.
	/// @param instance_data must be in a form useful for the service being provided.
	tcp_service(const std::string& server, int port, const std::string& instance_data);
	tcp_service() = delete;
	tcp_service(const tcp_service&) = delete;
	tcp_service(tcp_service&& ts) = delete;
	~tcp_service();

	void write(const std::string& contents);
	void check_response();

	std::string get_instance_data() const { return m_instance_data; }
	std::string get_port() const { return m_port; }
	std::string get_server() const { return m_server; }

  private:
	/// throws a boost system_error on failure to connect.
	void connect();
	void check_deadline();
	boost::asio::io_service m_io_service;
	boost::asio::deadline_timer m_deadline_timer;
	std::string m_server;
	std::string m_port;
	std::string m_instance_data;
	boost::posix_time::time_duration m_timeout;
	boost::asio::ip::tcp::socket m_socket;

};
}
}
