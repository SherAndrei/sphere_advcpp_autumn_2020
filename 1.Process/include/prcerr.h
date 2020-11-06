#ifndef PRC_ERROR_H
#define PRC_ERROR_H
#include <string>
#include <stdexcept>

namespace prc {

class Error : public std::runtime_error {
 public:
    using std::runtime_error::runtime_error;
};

class CreationError : public Error {
 public:
    explicit CreationError(const std::string& what);
};

class DescriptorError : public Error {
 private:
    int fd_;

 public:
    DescriptorError(const std::string& what, int fd);
    int fd() const;
};

}  // namespace prc


#endif  // PRC_ERROR_H
