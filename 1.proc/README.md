## Домашнее задание №1

'''cpp
//
// Интерфейс процесса
//

#ifndef PROCESS_H
#define PROCESS_H

class Process {
public:
	explicit Process(const std::string& path);
	~Process();

	size_t write(const void* data, size_t len);
	void writeExact(const void* data, size_t len);
	size_t read(void* data, size_t len);
	void readExact(void* data, size_t len);

	void closeStdin();

	void close();
};

#endif //PROCESS_H
'''
