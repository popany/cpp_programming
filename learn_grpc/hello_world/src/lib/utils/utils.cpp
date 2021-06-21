#include "utils.h"
#include <sstream>
#include <boost/date_time.hpp>

std::string GetCurrentTimeString(const std::string& fmt)
{
    std::ostringstream os;
    const static std::locale currlocale = std::locale(os.getloc(), new boost::posix_time::time_facet(fmt.c_str()));
    os.imbue(currlocale);
    const boost::posix_time::ptime& now = boost::posix_time::microsec_clock::local_time();
    os << now;
    return os.str();
}