#ifndef PROCESS_H
#define PROCESS_H
#include "descripter.h"
#include <string>
#include <vector>

class Process
{
public:
    explicit Process(const std::string& path, const std::vector<std::string>& params = {});    
	~Process();

    size_t write(const void* data, size_t len);
    void   writeExact(const void* data, size_t len);
    size_t read(void* data, size_t len);
    void   readExact(void* data, size_t len);

    void open(const std::string& path, const std::vector<std::string>& params = {});
    void closeStdin();
    void close();

	bool isRunning() const;

private:
    int _cpid = -1;	   /* child pid       */
    Descripter _write_to_child;  /* write to child  */
    Descripter _read_from_child; /* read from child */
};

#endif //PROCESS_H
