#ifndef DESCRIPTER_H
#define DESCRIPTER_H

namespace tcp
{
    
class Descripter
{
public:
	bool isValid() const;

public:
	Descripter() = default;
	explicit Descripter(int id);
    // TODO: Descripter(Descripter && d);
	Descripter(const Descripter& other) = delete;
	~Descripter();
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