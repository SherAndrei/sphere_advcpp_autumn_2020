#ifndef TCP_DESCRIPTOR_H
#define TCP_DESCRIPTOR_H
#include <unistd.h>

namespace tcp {

class Descriptor {
 public:
    bool valid() const;
    void invalidate();

 public:
    Descriptor() = default;
    explicit Descriptor(int id);

    Descriptor(const Descriptor& other) = delete;
    Descriptor& operator= (const Descriptor &  other) = delete;

    Descriptor(Descriptor && other);
    Descriptor& operator= (Descriptor && other);

    ~Descriptor();

 public:
    void close();

 public:
    int  fd() const;
    void set_fd(int id);

 private:
    int _id = -1;
};

}  // namespace tcp

#endif  // TCP_DESCRIPTOR_H
