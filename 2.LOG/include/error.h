#ifndef LOGGER_ERROR_H
#define LOGGER_ERROR_H
#include <stdexcept>

namespace log
{
    
class InstanceError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace log

#endif // LOGGER_ERROR_H