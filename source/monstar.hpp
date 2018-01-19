#pragma once

#include "notification.hpp"
#include <map>
#include <string>

namespace monstar {

using es_data_t = std::map<std::string, std::string>;

class notification;
class notification_handler;

/// Configure the Graphite server access.
///   @param ip The ip-address (or path) of graphite server.
///   @param port port of the graphite server.
///   @param prefix  A global prefix to be applied to all graphite posts.
void configure_graphite(std::string ip, int port, std::string prefix);

/// Configure the Elasticsearch server access.
///   @param ip The ip-address (or path) of es server.
///   @param port port of the es server.
///   @param prefix  A global data set to be applied to all es entries.
void configure_elasticsearch(std::string ip, int port, const es_data_t& instance_data);

/// The user of the library is responsible to ensure that:
///    1.  The notification handler is set before the first call to
///    notify (warning is printed otherwise).
///    2.  The notification handler lives long enough to handle all
///    notifcations.
void set_notification_handler(notification_handler& nh);

/// Add a notification to the notification handler.
void notify(const notification& msg);

}
