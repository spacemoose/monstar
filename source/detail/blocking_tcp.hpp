#pragma once

#include <boost/asio.hpp>
#include <fuctional>
#include <string>

namespace monstar {
namespace detail {

/// This is just the blocking tcp connection with timout provided in
/// the boost examples, modified slightly to fit into my
/// configuration scheme.
class blocking_tcp
{
    blocking_tcp()
	void check_deadline();
	{
		if (m_deadline.expires_at() <= deadline_timer::traits_type::now()) {
			boost::system::error_code ignored_ec;
			m_socket.close(ignored_ec);
			m_deadline.async_wait(std::bind(&client::check_deadline, this))
		}
	}


}
}
