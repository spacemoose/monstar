#include "ES_provider.hpp"

#include <iostream>

#define FMT_HEADER_ONLY
#include <fmt/format.h>

namespace monstar {

/// If it's in the detail namespace, it's an implementation detail,
/// not part of the api, and espcially prone to change.
namespace detail {

std::string ES_provider::m_server{"127.0.0.1"};
std::string ES_provider::m_port{"9200"};
std::string ES_provider::m_instance_data;

ES_provider::ES_provider()
  : m_resolver(m_io_service)
  , m_socket(m_io_service)
{
	try {
		boost::asio::ip::tcp::resolver::query query(m_server, m_port);
		auto endpoint = *m_resolver.resolve(query);
		m_socket.connect(endpoint);
	} catch (std::exception& e) {
		throw std::runtime_error(
		  "Couldn't establish a socket conection to an elasticsearch server, with port " +
		  m_port + " and ip " + m_server + ".   The text was: \n<" + e.what() + ">\n");
	}
}

/// Formats instance data to be inserted into a message.  prepends a
/// comma, so it assumes a non empty message, and that the instnace
/// data is getting added to the end.j
void ES_provider::set_instance_data(const es_data_t& data)
{
	std::stringstream ss;
	for (auto& p : data) {
		ss << fmt::format(",\"{}\" : \"{}\"", p.first, p.second);
	};
	m_instance_data = ss.str();
}

/// @todo batch processing.
void ES_provider::post_message(std::string index, std::string type, std::string message)
{

	message += m_instance_data;
	thread_local std::stringstream ss;
	/// @todo  make a constant header.
	ss << "POST /" << index << "/" << type << " HTTP/1.1 \r\n"
	   << "Host: " + m_server + ":" + m_port + "\r\n"
	   << "User-Agent: C/1.0\r\n"
	   << "Accept: */*\r\n"
	   << "Content-Type: application/json\r\n"
	   << "Content-Length: " << message.length() + 2 << "\r\n\r\n" // +2 for the {}
	   << "{" << message << "}";
	boost::asio::streambuf request;
	std::ostream rstream(&request);
	std::ostream request_stream(&request);

	request_stream << ss.str();

	boost::asio::write(m_socket, request);
	process_response();

	ss.str(std::string());
}

/// @todo real error handling
void ES_provider::process_response()
{
	boost::asio::streambuf response;

	boost::asio::read_until(m_socket, response, "\n");
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
