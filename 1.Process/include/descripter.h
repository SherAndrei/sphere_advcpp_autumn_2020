#ifndef DESCRIPTER_H
#define DESCRIPTER_H

class Descripter
{
public:
	bool isValid() const;

public:
	Descripter() = default;
	explicit Descripter(int id);
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

#endif //DESCRIPTER_H