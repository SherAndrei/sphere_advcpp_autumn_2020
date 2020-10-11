#ifndef PROCESS_H
#define PROCESS_H
#include "descriptor.h"
#include <string>

class Process
{
public:
    explicit Process(const std::string& path);    
	~Process();

    size_t write(const void* data, size_t len);
    void   writeExact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void   readExact(void* data, size_t len);

    void closeStdin();
    void close();

private:
    Descriptor _cpid;	   /* child pid       */
    Descriptor _w_pid_in;  /* write to child  */
    Descriptor _r_pid_out; /* read from child */
};

#endif //PROCESS_H