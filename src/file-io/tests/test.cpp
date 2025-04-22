#include "../file.h"
#include <unistd.h>     // write, close, lseek,fsync
#include <assert.h>
#include <ctime>        // time


static bool test_file_use_case() {

    std::string filename = "testfile";
    if (dump::file_exists(filename.c_str())) {
        // remove if exists from prev run
        dump::delete_file(filename.c_str());
    }

    bool created = dump::create_file(filename.c_str());
    bool exists = dump::file_exists(filename.c_str());
    off_t size = dump::file_size(filename.c_str());
    assert(created == true);
    assert(exists == true);
    assert(size == 0);

    int fd = dump::open_file(filename.c_str());
    assert(fd > -1);

    // write to file
    char buffer[10] = {'H', 'e', 'l', 'l', 'o', '\0', '\0', '\0', '\0', '\0'};
    ssize_t written = dump::write_to_file(fd, 0, &buffer[0], 5);
    assert(written == 5);
    size = dump::file_size(filename.c_str());
    assert(size == 5);

    // read from file
    char readBuffer[10];
    ssize_t read = dump::read_from_file(fd, 0, 5, &readBuffer[0]);
    assert(read == 5);
    std::string result_string = std::string(readBuffer, 5);
    assert(result_string == "Hello");

    // overwrite some chars in file
    buffer[0] = 'M';
    buffer[1] = 'W';
    written = dump::write_to_file(fd, 2, &buffer[0], 2);
    assert(written == 2);
    read = dump::read_from_file(fd, 0, 5, &readBuffer[0]);
    assert(read == 5);
    result_string = std::string(readBuffer, 5);
    assert(result_string == "HeMWo");
    size = dump::file_size(filename.c_str());
    assert(size == 5);

    // remove file
    bool deleted = dump::delete_file(filename.c_str());
    assert(deleted == true);

    exists = dump::file_exists(filename.c_str());
    assert(exists == false);

    close(fd);
    return true;

}

static bool test_file_use_case_header() {

    struct Head {
        char magic[8] = "_LSTH_";
        uint64_t version; // avoid implicit padding even if 64bit is oversized for version
        uint64_t page_size;
        uint64_t root_offset;
        uint64_t total_number_of_records;
        uint64_t free_list_offset;
        uint64_t created_unix;
        uint64_t checksum_signature;
        char free[8]; // free bytes
    };

    Head head = {
        .version = 1,
        .page_size = 4096,
        .root_offset = 0,
        .total_number_of_records = 0,
        .free_list_offset = 0,
        .created_unix = static_cast<uint64_t>(time(nullptr)),
        .checksum_signature = 0,
        .free = {0},
    };

    std::string filename = "testfile";
    if (dump::file_exists(filename.c_str())) {
        // remove if exists from prev run
        dump::delete_file(filename.c_str());
    }

    bool created = dump::create_file(filename.c_str());
    bool exists = dump::file_exists(filename.c_str());
    off_t size = dump::file_size(filename.c_str());
    assert(created == true);
    assert(exists == true);
    assert(size == 0);

    int fd = dump::open_file(filename.c_str());
    assert(fd > -1);

    // write to file
    ssize_t written = dump::write_to_file(fd, 0, (char *)&head, sizeof(head));

    assert(written == sizeof(head));
    size = dump::file_size(filename.c_str());
    assert(size == sizeof(head));

    // read from file
    char readBuffer[sizeof(head)];
    ssize_t read = dump::read_from_file(fd, 0, sizeof(head), &readBuffer[0]);
    assert(read == sizeof(head));
    Head read_head;
    memcpy(&read_head, readBuffer, sizeof(Head));

    assert(head.magic[0] == read_head.magic[0]);
    assert(head.magic[4] == read_head.magic[4]);
    assert(head.magic[5] == read_head.magic[5]);
    assert(head.magic[7] == read_head.magic[7]);
    assert(head.version == read_head.version);
    assert(head.page_size == read_head.page_size);
    assert(head.created_unix == read_head.created_unix);

    // remove file
    bool deleted = dump::delete_file(filename.c_str());
    assert(deleted == true);

    exists = dump::file_exists(filename.c_str());
    assert(exists == false);

    close(fd);
    return true;

}

int main() {
    try {
        test_file_use_case();
        test_file_use_case_header();
        std::cout << "Test with no errors" << std::endl;
        return EXIT_SUCCESS;
    } catch(const std::exception& e) {
        std::cerr << "Test with errors" << std::endl;
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

}