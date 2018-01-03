#pragma once
#include <cassert>
#include <string.h>

#include <boost/uuid/uuid.hpp>
/// clients will probably want to use uuids...
/// Need to convert uuids to __int128s, but hopefully only here.
/// @todo unit test this.
__int128 toInt(const boost::uuids::uuid id)
{
	__int128 retval;
	memcpy(&retval, &id, 16);
	return retval;
}
