#pragma once
#include "tcp_service.hpp"

#include <boost/optional.hpp>
#include <string>

namespace monstar {

using es_data_t = std::map<std::string, std::string>;

namespace detail {

/// Provides access to an elasticsearch instance, and the
/// configuration information for it.
///
/// Each ES_provider is assumed to run in a single thread.
///
/// The concept here is that client code can just use an ES_provider,
/// and if elasticsearch is not configured, or is disabled at some
/// point, post_message calls wind up doing nothing.
///
class ES_provider
{
  public:
	/// Index and type are what the tutorial calls the first two parameters in a
	/// post, e.g. 'jobs' & 'job'
	/// @todo the index and type should be part of the provider?
	ES_provider();
	void post_message(const std::string& index, const std::string& type, std::string message);

  private:
	void process_response();
	boost::optional<std::unique_ptr<tcp_service>> m_service;
	std::string m_instance_data;
	std::string m_header;
	std::string m_const_header_part;
};
}
}
