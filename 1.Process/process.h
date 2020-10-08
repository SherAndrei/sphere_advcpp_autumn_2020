#ifndef PROCESS_H
#define PROCESS_H
#include <string>
#include <vector>

class Process
{
public:
    Process(const std::string& path);    
	~Process();

    size_t write(const void* data, size_t len);
    void   writeExact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void   readExact(void* data, size_t len);

    void closeStdin();
    void close();

private:
    pid_t _cpid;
    int _pipe_in[2];
    int _pipe_out[2];
};

#endif //PROCESS_H