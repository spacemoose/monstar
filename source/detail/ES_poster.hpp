#pragma once

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <chrono>

#include <string>

namespace monstar {
namespace detail {

/// ESPoster posts messages to ElasticSearch.
/// @todo batch processing.
class ES_poster
{
  public:
	/// Index and type are what the tutorial calls the first two parameters in a
	/// post, e.g. 'jobs' & 'job'
	ES_poster(std::string es_server, std::string es_port);
	void post_message(std::string index, std::string type, std::string json);

  private:
	void process_response();

	boost::asio::io_service m_io_service;
	boost::asio::ip::tcp::resolver m_resolver;
	boost::asio::ip::tcp::socket m_socket;
	std::string m_server;
	std::string m_port;
};
}
}
