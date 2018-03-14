#include <sstream>
#include <iostream>

namespace testing{
/// I want to test what gets written to stderr and stdout, so I have a
/// redirector object to help with that.
class redirector
{
  public:
	redirector()
	  : m_old_out(std::cout.rdbuf(m_out.rdbuf()))
	  , m_old_err(std::cerr.rdbuf(m_err.rdbuf()))
	{
	}

	~redirector()
	{
		std::cout.rdbuf(m_old_out);
		std::cerr.rdbuf(m_old_err);
	}

	std::string get_out() { return m_out.str(); };
	std::string get_err() { return m_err.str(); };
	redirector(const redirector&) = delete;
	redirector& operator=(const redirector&) = delete;

  private:
	std::stringstream m_out;
	std::stringstream m_err;
	std::streambuf* m_old_out;
	std::streambuf* m_old_err;
};

}
