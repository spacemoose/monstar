#include "tcp_service.hpp"

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/system/system_error.hpp>
#include <iostream>
#include <string>

namespace monstar {
namespace detail {

using namespace boost;

tcp_service::tcp_service(const std::string& server, int port, const std::string& instance_data)
  : m_deadline_timer(m_io_service)
  , m_server(server)
  , m_port(std::to_string(port))
  , m_instance_data(instance_data)
  , m_timeout(posix_time::seconds(5))
  , m_socket(m_io_service)
{
	// No deadline is required until the first socket operation is started. We
	// set the deadline to positive infinity so that the actor takes no action
	// until a specific deadline is set.
	m_deadline_timer.expires_at(posix_time::pos_infin);

	// Start the persistent actor that checks for deadline expiry.
	check_deadline();
	connect();
}

tcp_service::~tcp_service() {}
void tcp_service::connect()
{
	asio::ip::tcp::resolver::query query(m_server, m_port);
	auto iter = asio::ip::tcp::resolver(m_io_service).resolve(query);
	m_deadline_timer.expires_from_now(m_timeout);
	system::error_code ec = boost::asio::error::would_block;
	asio::async_connect(m_socket, iter, lambda::var(ec) = lambda::_1);
	do
		m_io_service.run_one();
	while (ec == asio::error::would_block);

	// Determine whether a connection was successfully established. The
	// deadline actor may have had a chance to run and close our socket, even
	// though the connect operation notionally succeeded. Therefore we must
	// check whether the socket is still open before deciding if we succeeded
	// or failed.
	if (ec || !m_socket.is_open())
		throw system::system_error(ec ? ec : asio::error::operation_aborted);
}

/// Used to:
// throw std::runtime_error(
//   "Couldn't establish a socket conection to an elasticsearch server, with port " +
//   m_port + " and ip " + m_server + ".   The text was: \n<" + e.what() + ">\n");
void tcp_service::write(const std::string& contents)
{
	m_deadline_timer.expires_from_now(m_timeout);
	system::error_code ec = boost::asio::error::would_block;
	boost::asio::streambuf request;
	std::ostream request_stream(&request);
	request_stream << contents;

	asio::async_write(m_socket, request, lambda::var(ec) = lambda::_1);
	do
		m_io_service.run_one();
	while (ec == asio::error::would_block);

	if (ec) throw system::system_error(ec);
}

/// sanity check response, and throw an exception if it was bad.
void tcp_service::check_response()
{

	boost::asio::streambuf buff;
	// Set a deadline for the asynchronous operation. Since this function uses
	// a composed operation (async_read_until), the deadline applies to the
	// entire operation, rather than individual reads from the socket.
	m_deadline_timer.expires_from_now(m_timeout);

	// Set up the variable that receives the result of the asynchronous
	// operation. The error code is set to would_block to signal that the
	// operation is incomplete. Asio guarantees that its asynchronous
	// operations will never fail with would_block, so any other value in
	// ec indicates completion.
	boost::system::error_code ec = boost::asio::error::would_block;

	// Start the asynchronous operation itself. The boost::lambda function
	// object is used as a callback and will update the ec variable when the
	// operation completes. The blocking_udp_client.cpp example shows how you
	// can use boost::bind rather than boost::lambda.
	boost::asio::async_read_until(m_socket, buff, '\n', lambda::var(ec) = lambda::_1);

	// Block until the asynchronous operation has completed.
	do
		m_io_service.run_one();
	while (ec == boost::asio::error::would_block);

	if (ec) throw boost::system::system_error(ec);

	std::string line;
	std::istream is(&buff);
	std::getline(is, line);

	std::istringstream iss(line);
	std::string http_version;
	iss >> http_version;
	std::string status_code;
	iss >> status_code;

	if ( http_version.substr(0, 5) != "HTTP/") {
		throw std::runtime_error("Invalid response.  Contents: " + line);
	}
	if (status_code != "201") {
		throw std::runtime_error("Response returned with status code " + status_code +
		                         ".  The complete response was: \n" + line);
	}
}

void tcp_service::check_deadline()
{
	// Check whether the deadline has passed. We compare the deadline against
	// the current time since a new asynchronous operation may have moved the
	// deadline before this actor had a chance to run.
	if (m_deadline_timer.expires_at() <= asio::deadline_timer::traits_type::now()) {
		// The deadline has passed. The socket is closed so that any outstanding
		// asynchronous operations are cancelled. This allows the blocked
		// connect(), read_line() or write_line() functions to return.
		system::error_code ignored_ec;
		m_socket.close(ignored_ec);

		// There is no longer an active deadline. The expiry is set to positive
		// infinity so that the actor takes no action until a new deadline is set.
		m_deadline_timer.expires_at(posix_time::pos_infin);
	}

	// Put the actor back to sleep.
	m_deadline_timer.async_wait(std::bind(&tcp_service::check_deadline, this));
}
}
}
