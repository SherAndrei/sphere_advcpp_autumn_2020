#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <algorithm>
#include "process.h"
#include "prcerr.h"

static void handle_error(int errnum) {
    if (errnum == -1)
        throw prc::Error(std::strerror(errno));
}

prc::Process::Process(const std::string& path,
                      const std::vector<std::string>& params) {
    open(path, params);
}

static std::vector<char *> argv(std::string* path,
                                std::vector<std::string>* params) {
    std::vector<char*> result;
    result.reserve(params->size() + 2);

    result.push_back(path->data());

    for (auto& word : *params)
        result.push_back(word.data());

    result.push_back(nullptr);
    return result;
}

void prc::Process::open(const std::string& path,
                        const std::vector<std::string>& params) {
    if (isRunning()) {
        throw CreationError("Nested processes are forbidden");
    }
    int pipe_in[2], pipe_out[2];
    if (pipe2(pipe_out, O_CLOEXEC) == -1)
        throw CreationError(std::strerror(errno));

    if (pipe(pipe_in) == -1) {
        ::close(pipe_out[0]);
        ::close(pipe_out[1]);
        throw CreationError(std::strerror(errno));
    }

    Descriptor write_to_parent(pipe_out[1]);
    _read_from_child.set_fd(pipe_out[0]);

    Descriptor read_from_parent(pipe_in[0]);
    _write_to_child.set_fd(pipe_in[1]);

    if ((_cpid = fork()) == -1)
        throw CreationError(std::strerror(errno));

    if (_cpid == 0) { /* child process */
        if (::dup2(read_from_parent.fd(), STDIN_FILENO) == -1) {
            std::cerr << std::strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        read_from_parent.close();
        _write_to_child.close();

        if (::dup2(write_to_parent.fd(), STDOUT_FILENO) == -1) {
            std::cerr << std::strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        write_to_parent.close();
        _read_from_child.close();

        std::string path_copy = path;
        std::vector<std::string> params_copy = params;

        if (execv(path.c_str(), argv(&path_copy, &params_copy).data()) == -1) {
            std::cerr << std::strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

prc::Process::~Process() {
    try {
        close();
    } catch (const std::runtime_error& re) {}
}

size_t prc::Process::write(const void* data, size_t len) {
    ssize_t size = ::write(_write_to_child.fd(), data, len);
    if (size == -1)
        throw DescriptorError(std::strerror(errno), _write_to_child.fd());

    return static_cast<size_t> (size);
}

void prc::Process::writeExact(const void* data, size_t len) {
    size_t counter = 0u;
    const char* ch_data = static_cast<const char*> (data);
    while (counter < len)
        counter += write(ch_data + counter, len - counter);
}

size_t prc::Process::read(void* data, size_t len) {
    ssize_t size = ::read(_read_from_child.fd(), data, len);
    if (size == -1)
        throw DescriptorError(std::strerror(errno), _read_from_child.fd());

    return static_cast<size_t> (size);
}

void prc::Process::readExact(void* data, size_t len) {
    size_t counter = 0u;
    size_t current = 0u;
    char* ch_data = static_cast<char*> (data);
    while (counter < len) {
        current  = read(ch_data + counter, len - counter);
        if (current == 0)
            throw DescriptorError(std::strerror(errno), _read_from_child.fd());
        counter += current;
    }
}

void prc::Process::closeStdin() {
    _write_to_child.close();
}

void prc::Process::close() {
    _write_to_child.close();
    _read_from_child.close();

    handle_error(kill(_cpid, SIGINT));
    handle_error(waitpid(_cpid, nullptr, 0));
}

bool prc::Process::isRunning() const {
    return !waitpid(_cpid, nullptr, WNOHANG);
}
