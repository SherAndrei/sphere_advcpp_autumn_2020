#ifndef PRC_DESCRIPTOR_H
#define PRC_DESCRIPTOR_H
#include <unistd.h>

namespace prc {

class Descriptor {
 public:
    bool valid() const;

 private:
    void invalidate();

 public:
    Descriptor() = default;
    explicit Descriptor(int id);

    Descriptor(const Descriptor& other)               = delete;
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

}  // namespace prc

#endif  // PRC_DESCRIPTOR_H
