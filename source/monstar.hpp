/// @file
///
/// @todo make sure forward decls are provided for all classes in public api.

#pragma once

#include <map>
#include <string>


namespace monstar {

using es_data_t = std::map<std::string, std::string>;

struct notification;
class notification_handler;

/// Configure the Graphite server access.
///   @param ip The ip-address (or path) of graphite server.
///   @param port port of the graphite server.
///   @param prefix  A global prefix to be applied to all graphite posts.
void configure_graphite(std::string ip, int port, std::string prefix);

/// Configure the Elasticsearch server access.
///   @param ip The ip-address (or path) of es server.
///   @param port port of the es server.
///   @param instance_data  A global data set to be applied to all es entries.
void configure_elasticsearch(std::string ip, int port, const es_data_t& instance_data);

/// The user of the library is responsible to ensure that:
///    1.  The notification handler is set before the first call to
///    notify (warning is printed otherwise).
///    2.  The notification handler lives long enough to handle all
///    notifcations.
void set_notification_handler(notification_handler& nh);

void notify(const notification& msg);

/// Send an annotation event to elasticsearch.  Uses now as the timestamp.
/// @param index the index where the event should be stored.
/// @param title Event title.
/// @param text  Event text.
/// @param tags  Comman separated list of tags.
void send_annotation(const std::string& index,
                     const std::string& title,
                     const std::string& text,
                     const std::string& tags);

}
