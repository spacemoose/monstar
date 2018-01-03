#include "TS_message.hpp"
#include "epoch.hpp"

namespace monstar {

TS_message::TS_message(msg_id_t id,
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

TS_message::TS_message(msg_id_t id,
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

TS_message& TS_message::set_state(std::string new_state)
{
	m_timestamp = epoch::now();
	m_new_state = new_state;
	return *this;
}

void TS_message::set_timestamp()
{
	m_timestamp = epoch::now();
}
}
