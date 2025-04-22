#include <fcntl.h>      // open
#include <unistd.h>     // write, close, lseek,fsync
#include <cstring>      // memcpy
#include <stdio.h>      // remove
#include <sys/stat.h>   // stat
#include <iostream>


namespace dump {
    void log_error(const std::string& prefix) {
        std::cerr << prefix << ": " << strerror(errno) << std::endl;
    }

    bool file_exists(const char * filename) {
        return ( access( filename, F_OK ) != -1 );
    }

    bool create_file(const char * filename) {
        int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd == -1) {
            log_error("open. failed to create file.");
            return false;
        }
        close(fd);
        std::cout << "file created" << std::endl;
        return true;

    }

    int open_file(const char * filename) {
        int fd = open(filename, O_RDWR);
        if (fd == -1) {
            log_error("open. failed to open file.");
            return -1;
        }
        return fd;
    }

    bool delete_file(const char * filename) {
        int result = remove(filename);
        if (result == -1) {
            log_error("remove. failed to remove file.");
            return false;
        }

        std::cout << "file removed" << std::endl;
        return true;

    }

    off_t file_size(const char * filename) {
        struct stat st;
        stat(filename, &st);
        return st.st_size;
    }

    ssize_t write_to_file(const int fd, const size_t offset, const char *buffer, const size_t size) {

        off_t seekOffset = lseek(fd, offset, SEEK_SET);
        if (seekOffset == -1) {
            log_error("lseek. failed to set offest.");
            return -1;
        }

        ssize_t totalBytesWritten = 0;
        while (totalBytesWritten < static_cast<ssize_t>(size)) {

            ssize_t bytesWritten = write(fd, buffer + totalBytesWritten, size - totalBytesWritten);
            if (bytesWritten == -1) {
                log_error("write. failed write to file.");
                return -1;
            }
            if (bytesWritten == 0) {
                break; // EOF
            }
            totalBytesWritten += bytesWritten;
        }

        if (totalBytesWritten == size) {
            fsync(fd);
        }

        std::cout << "Written " << totalBytesWritten << " bytes to file." << std::endl;
        return totalBytesWritten;
    }

    ssize_t write_to_file(const char * filename, const size_t offset, const char *buffer, const size_t size) {
        int fd = open(filename, O_WRONLY);
        if (fd == -1) {
            log_error("open. failed to open file.");
            return -1;
        }

        int result = write_to_file(fd, offset, buffer, size);
        close(fd);
        return result;
    }

    ssize_t read_from_file(int fd, const size_t offset, const size_t size, char *result_buffer) {

        off_t seekOffset = lseek(fd, offset, SEEK_SET);
        if (seekOffset == -1) {
            log_error("lseek. failed to set offest.");
            return -1;
        }

        ssize_t totalBytesRead = 0;
        while (totalBytesRead < static_cast<ssize_t>(size)) {

            ssize_t bytesRead = read(fd, result_buffer + totalBytesRead, size - totalBytesRead);
            if (bytesRead == -1) {
                log_error("read. failed read from file.");
                return -1;
            }
            if (bytesRead == 0) {
                break; // EOF
            }
            totalBytesRead += bytesRead;
        }

        std::cout << "Read " << totalBytesRead << " bytes from file." << std::endl;
        return totalBytesRead;
    }

    ssize_t read_from_file(const char * filename, const size_t offset, const size_t size, char *result_buffer) {
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            log_error("open. failed to open file.");
            return -1;
        }

        int result = read_from_file(fd, offset, size, result_buffer);
        close(fd);
        return result;
    }

}
