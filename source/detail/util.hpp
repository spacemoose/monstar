#include <string>

namespace util
{


/// This just escapes quotes. Implement more sophisticated handling if
/// the need occurs.
std::string escape_quotes(const std::string& text)
{
  std::string  retval;
  retval.reserve(text.length() + 8);
  for (std::string::size_type i=0; i<text.length(); ++i) {
	switch (text[i]) {
	case '"':
	case '\\':
	  retval += '\\';
	default:
	  retval += text[i];
	}
  }
  return retval;
}

}
