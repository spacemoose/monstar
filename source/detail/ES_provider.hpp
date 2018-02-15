#pragma once


#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <chrono>

#include <string>

namespace monstar {
using es_data_t = std::map<std::string, std::string>;
namespace detail {

/// Provides access to an elasticsearch instance, and the
/// configuration information for it.
///
class ES_provider
{
  public:
	/// Index and type are what the tutorial calls the first two parameters in a
	/// post, e.g. 'jobs' & 'job'
	ES_provider();
	void post_message(std::string index, std::string type, std::string json);

    static std::string es_server;
    static std::string es_port;
    static es_data_t   instance_data;

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
