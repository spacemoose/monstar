#include "Notification.hpp"
#include "epoch.hpp"

namespace monstar {

Notification::Notification(msg_id_t id,
                       std::string new_state,
                       std::string es_index,
                       std::string es_type,
                       std::map<std::string, std::string> data)
  : m_data(data)
  , m_finished(false)
  , m_identifier(id)
  , m_es_index(es_index)
  , m_es_type(es_type)
  , m_new_state(new_state)
  , m_timestamp(epoch::now())
{
}

Notification::Notification(msg_id_t id,
                       std::string new_state,
                       std::string es_index,
                       std::string es_type)
  : m_finished(false)
  , m_identifier(id)
  , m_es_index(es_index)
  , m_es_type(es_type)
  , m_new_state(new_state)
  , m_timestamp(epoch::now())
{
}

Notification& Notification::set_state(std::string new_state)
{
	m_timestamp = epoch::now();
	m_new_state = new_state;
	return *this;
}

void Notification::set_timestamp()
{
	m_timestamp = epoch::now();
}
}
