#ifndef TCP_DESCRIPTER_H
#define TCP_DESCRIPTER_H

namespace tcp {

class Descripter {
 public:
    bool valid() const;
    void invalidate();

 public:
    Descripter() = default;
    explicit Descripter(int id);

    Descripter(const Descripter& other) = delete;
    Descripter& operator= (const Descripter &  other) = delete;

    Descripter(Descripter && other);
    Descripter& operator= (Descripter && other);

    ~Descripter();

 public:
    void close();

 public:
    int  fd() const;
    void set_fd(int id);

 private:
    int _id = -1;
};

}  // namespace tcp

#endif  // TCP_DESCRIPTER_H
