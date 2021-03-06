#pragma once

#include <map>
#include <string>

namespace monstar {

using msg_id_t = __int128_t;
using timestamp_t = unsigned long;
using data_t = std::map<std::string, std::string>;

/// A notification is a kind of message to the time-series generator,
/// notifying the TS-generator that something has changed which it
/// needs to know about -- something was created, state was changed,
/// something was finished.
///
/// The TS_processor needs a unique identifier (to find the correct
/// TS_generable), the new state of the tracked entity, and the time
/// at which the state change occured.  Since both the Trx and Job
/// states are expressed as enums, we store their integer
/// representations.
///
/// @todo const correctness here.
/// @todo optimizations:
///      - const_data and volatile_data
///      - move semantics
struct notification
{
  public:
	notification(msg_id_t id,
	           std::string new_state,
	           std::string es_index,
	           std::string es_type,
	           std::map<std::string, std::string> data);

	notification(msg_id_t id,
	           std::string new_state,
	           std::string es_index,
	           std::string es_type);

	msg_id_t get_identifier()   const { return m_identifier; }
    std::string get_state()     const { return m_new_state; }
	timestamp_t get_timestamp() const { return m_timestamp; }
	const data_t& get_data()    const { return m_data; }
	std::string get_es_type()   const { return m_es_type; }
	std::string get_es_index()  const { return m_es_index; }

	bool finished() const { return m_finished; }

	///  When the state is updated, we automatically update the
	///  timestamp to now.
	notification& set_state(std::string new_state);

	/// This is intended for setting non-state data that can change
	/// over time.  Here we don't set the timestamp.
	notification& set(std::string key, std::string value)
	{
		m_data[key] = value;
		return *this;
	}

	/// Note that setting finished sets the timestamp.
	notification& set_finished()
	{
		m_finished = true;
		return *this;
	}

	void set_timestamp();

  private:
	std::map<std::string, std::string> m_data; ///< mostly constant.
	bool m_finished;                           ///< indicates next TS data is the last.
	msg_id_t m_identifier;                     ///< Unique identifier for the message.
	std::string m_es_index;
	std::string m_es_type;
	std::string m_new_state;   ///< The current state of the entitity.
	unsigned long m_timestamp; ///< The timestamp at which the state changed.
};


}
