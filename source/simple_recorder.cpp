#include "simple_recorder.hpp"
#include "detail/graphite_provider.hpp"
#include "epoch.hpp"

namespace monstar {

/// a single recorder is used to record a single value.
simple_recorder::simple_recorder(std::string metric_name)
  : m_metric_path(detail::graphite_provider::get_prefix() + "." + metric_name)
  , m_provider(std::make_unique<detail::graphite_provider>())
{
}

simple_recorder::simple_recorder(const simple_recorder& gr)
  : m_metric_path(gr.m_metric_path)
{
}

simple_recorder::~simple_recorder() {}

/// Use this operator to record a value with an automatic timestamp
void simple_recorder::operator()(double val) { this->operator()(epoch::now(), val); }

/// This is useful for creating fake data, or making sure a set of vals are
/// recorded with an identical timestamp.
void simple_recorder::operator()(int secs_since_epoch, double val)
{
	m_msg.str("");
	m_msg << m_metric_path << " " << val << " " << secs_since_epoch << "\n";
	(*m_provider)(m_msg.str());
}
}