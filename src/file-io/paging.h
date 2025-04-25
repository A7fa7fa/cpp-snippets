#pragma once
#include <cstring>      // memcpy
#include <assert.h>
#include "file.h"


namespace dump {
    constexpr char file_identifier[8] = {'_', 'L', 'S', 'T', 'H', '_', '\0', '\0'};

    struct Head {
        char magic[8];
        uint64_t version; // avoid implicit padding even if 64bit is oversized
        uint64_t page_size;
        uint64_t root_offset;
        uint64_t total_number_of_records;
        uint64_t free_list_offset;
        uint64_t created_unix;
        uint64_t checksum_signature;
        char free[32]; // free bytes for now
    };

    Head * create_head(uint64_t page_size) {
        Head * head = new Head{
            .version = 1,
            .page_size = page_size,
            .root_offset = 0,
            .total_number_of_records = 0,
            .free_list_offset = 0,
            .created_unix = static_cast<uint64_t>(time(nullptr)),
            .checksum_signature = 0,
            .free = {0},
        };

        std::memcpy(head->magic, file_identifier, sizeof(head->magic));
        return head;
    }

    Head * create_head() {
        return create_head(4096);
    }

    char * create_page(uint64_t page_size) {
        char * page = new char[page_size];
        return page;
    }

    ssize_t write_head_to_file(const int fd, Head *head) {
        ssize_t written = dump::write_to_file(fd, 0, (char *)head, sizeof(Head));
        return written;
    }

    ssize_t read_head_from_file(const int fd, Head *read_head) {
        char read_buffer[sizeof(Head)];
        ssize_t read = dump::read_from_file(fd, 0, sizeof(Head), read_buffer);

        char identifier[sizeof(file_identifier)];
        memcpy(identifier, read_buffer, sizeof(file_identifier));

        if( strcmp(identifier, file_identifier) != 0 ) { // strings are not equal
            read = -1;
        }

        if (read != -1) {
            memcpy(read_head, read_buffer, sizeof(Head));
        }

        return read;
    }

    ssize_t read_page_from_file(const int fd, const uint64_t page_size, const int page_num, char *result_buffer) {
        size_t offset = page_size * page_num + sizeof(Head);
        ssize_t read_bytes = read_from_file(fd, offset, page_size, result_buffer);
        return read_bytes;
    }

    ssize_t write_page_to_file(const int fd, const uint64_t page_size, const int page_num, char *page_buffer) {
        size_t offset = page_size * page_num + sizeof(Head);
        ssize_t written_bytes = write_to_file(fd, offset, page_buffer, page_size);
        return written_bytes;
    }

}
