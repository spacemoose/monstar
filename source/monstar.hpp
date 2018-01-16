#pragma once

#include "Notification.hpp"
#include <map>
#include <string>

namespace monstar {

class Notification;

/// Configure the Graphite server access.
///   @param ip The ip-address (or path) of graphite server.
///   @param port port of the graphite server.
///   @param prefix  A global prefix to be applied to all graphite posts.
void configure_graphite(std::string ip, int port, std::string prefix);

/// Configure options for the elasticsearch server.
///  @param ip   The ip address or path to elasticsearch server.
///  @param port The port of the elasticsearch server.
///  @param instance_data Data to be applied to all elasticsearch
///                       data.  This is the elasticsearch analog to
///                       the graphite prefix argument.
void configure_elasticsearch(std::string ip,
                             int port,
                             const std::map<std::string, std::string> instance_data);

/// This must be called before any call to notify, and after
/// configure_* methods.  It initializes the timeseries processor
/// which receives notification and sends timeseries data to
/// elasticsearch.
void initialize_ts_processor(int period);

/// Creates a message from the Trx, and adds it to the queue.
///
/// @todo maintain a backlog in case of disconnects?
void notify(const Notification& msg);

}
