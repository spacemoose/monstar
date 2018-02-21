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
/// Not built for concurrent use.
class ES_provider
{
  public:
	/// Index and type are what the tutorial calls the first two parameters in a
	/// post, e.g. 'jobs' & 'job'
	ES_provider();
	void post_message(std::string index, std::string type, std::string json);

    static void set_server(std::string server){m_server=server;}
    static void set_port(int port){m_port = std::to_string(port);}
    static void set_instance_data(const es_data_t& instance_data);

  private:
	void process_response();

	boost::asio::io_service m_io_service;
	boost::asio::ip::tcp::resolver m_resolver;
	boost::asio::ip::tcp::socket m_socket;

    static std::string m_server;
    static std::string m_port;
    static std::string m_instance_data;

};
}
}
