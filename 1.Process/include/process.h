#ifndef PRC_PROCESS_H
#define PRC_PROCESS_H
#include <string>
#include <vector>
#include "descriptor.h"

namespace prc {

class Process {
 public:
    explicit Process(const std::string& path,
                     const std::vector<std::string>& params = {});
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
    int _cpid = -1;
    Descriptor _write_to_child;
    Descriptor _read_from_child;
};

}  // namespace prc

#endif  // PRC_PROCESS_H
