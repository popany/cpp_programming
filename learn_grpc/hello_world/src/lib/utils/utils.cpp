#include "utils.h"
#include <sstream>
#include <boost/date_time.hpp>

namespace utils
{

std::string GetCurrentTimeString(const std::string& fmt)
{
    std::ostringstream os;
    const static std::locale currlocale = std::locale(os.getloc(), new boost::posix_time::time_facet(fmt.c_str()));
    os.imbue(currlocale);
    const boost::posix_time::ptime& now = boost::posix_time::microsec_clock::local_time();
    os << now;
    return os.str();
}

LogLevel ParseLogLevel(std::string logLevel)
{
    if (logLevel == "DEBUG") {
        return LogLevel::LOG_DEBUG;
    }
    if (logLevel == "INFO") {
        return LogLevel::LOG_INFO;
    }
    if (logLevel == "WARN") {
        return LogLevel::LOG_WARN;
    }
    if (logLevel == "ERROR") {
        return LogLevel::LOG_ERROR;
    }
    return LogLevel::LOG_INFO;
}

bool StringToBool(const std::string& s)
{
    return s == "true";
}

}