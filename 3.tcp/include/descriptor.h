#ifndef TCP_DESCRIPTOR_H
#define TCP_DESCRIPTOR_H
#include <unistd.h>

namespace tcp {

class Descriptor {
 public:
    bool valid() const;

 public:
    Descriptor() = default;
    explicit Descriptor(int fd);

    Descriptor(const Descriptor& other) = delete;
    Descriptor& operator= (const Descriptor&  other) = delete;

    Descriptor(Descriptor&& other);
    Descriptor& operator= (Descriptor&& other);

    virtual ~Descriptor();

 public:
    void close();

 public:
    int  fd() const;
    void set_fd(int fd);

 protected:
    void invalidate();

 protected:
    int _fd = -1;
};

}  // namespace tcp

#endif  // TCP_DESCRIPTOR_H
