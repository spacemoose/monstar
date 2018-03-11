#include "ES_provider.hpp"
#include "configuration.hpp"

#include <iostream>
#include <sstream>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

namespace monstar {
namespace detail {

/// We use an optional socket so we only get a single error message,
/// and only if the service was configured but is unavailabe.
ES_provider::ES_provider()
{
	try {
		auto& ci = configuration::instance();
		auto es = connection_type::elastic_search;
		m_socket = ci.get_socket(es);
		if (m_socket) {
			m_instance_data = ci.get_instance_data(es);
			std::stringstream ss;
			ss << "Host: " + ci.get_server(es) + ":" + ci.get_port(es) + "\r\n"
			   << "User-Agent: C/1.0\r\n"
			   << "Accept: */*\r\n"
			   << "Content-Type: application/json\r\n"
			   << "Content-Length: ";
			m_const_header_part = ss.str();
		}
	}
	catch (std::exception& e) {
		std::cerr << "Exception handled while trying to connect to the elasticsearch server.  "
		             "Elasticsearch metrics are now disabled.The exception text was : \n "
		          << e.what() << std::endl;
		m_socket = std::nullopt;
	}
}

/// @todo batch processing.
void ES_provider::post_message(const std::string& index,
                               const std::string& type,
                               std::string message)
{
	if (not m_socket) return;

	message += m_instance_data;
	thread_local std::stringstream ss;
	ss << "POST /" << index << "/" << type << " HTTP/1.1 \r\n"
	   << m_const_header_part << message.length() + 2 << "\r\n\r\n" // +2 for the {}
	   << "{" << message << "}";
	boost::asio::streambuf request;
	std::ostream rstream(&request);
	std::ostream request_stream(&request);

	request_stream << ss.str();

	boost::asio::write(m_socket.value(), request);
	process_response();

	ss.str(std::string());
}

/// @todo real error handling
void ES_provider::process_response()
{
	assert(m_socket);
	if (not m_socket) return;
	boost::asio::streambuf response;
	boost::asio::read_until(m_socket.value(), response, "\n");
	std::istream response_stream(&response);
	std::string http_version;
	response_stream >> http_version;
	unsigned int status_code;
	response_stream >> status_code;
	std::string status_message;
	std::getline(response_stream, status_message);

	if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
		std::cerr << "Invalid response " << std::endl;
		exit(1);
	}
	if (status_code == 201) return; // created.

	//		error handling
	std::cerr << "Response returned with status code " << status_code << std::endl;
	std::string header;
	while (std::getline(response_stream, header) && header != "\r") {
		std::cerr << header << std::endl;
	}
	// Write whatever content we already have to output.
	if (response.size() > 0) {
		std::cerr << &response << std::endl;
	}
}
}
}
