#ifndef DESCRIPTER_H
#define DESCRIPTER_H

namespace tcp
{
    
class Descripter
{
public:
	bool valid() const;
    void invalidate();

public:
	Descripter() = default;
	explicit Descripter(int id);

	Descripter(const Descripter& other) = delete;
    Descripter(Descripter && other);
    ~Descripter();


    Descripter& operator= (const Descripter &  other) = delete;
    Descripter& operator= (Descripter && other);
public:
	void close();

public:
	int  fd() const;
	void set_fd(int id);

private:
	int _id = -1;
};

} // namespace tcp

#endif //DESCRIPTER_H