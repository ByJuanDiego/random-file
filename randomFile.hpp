//
// Created by juandiego on 4/13/23.
//

#ifndef RANDOM_FILE_HPP
#define RANDOM_FILE_HPP

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

struct Record {
    char name[30];
    char career[20];
    int cycle;

    void setData() {
        std::cout << "Name: ";
        std::cin >> name;
        name[29] = '\0';

        std::cout << "Career: ";
        std::cin >> career;
        career[19] = '\0';

        std::cout << "Cycle: ";
        std::cin >> cycle;
    }

    std::string getKey() {
        return name;
    }

    std::string toString() {
        std::stringstream ss;
        ss << "(" << name << ", " << career << ", " << cycle << ")";
        return ss.str();
    }
};

class RandomFile {
private:

    std::fstream data_file;
    std::string data_file_name;

    std::fstream index_file;
    std::string index_file_name;

    std::map<std::string, long> index;  //< Ordered access to records logical position

public:

    explicit RandomFile(std::string data_file_name, std::string index_file_name)
            : data_file_name(std::move(data_file_name)),
              index_file_name(std::move(index_file_name)) {
    }

    /*
    * Truncates the index-file information with the pairs key-position stored in `std::map`
    */
    ~RandomFile() {
        writeIndex();
    }

    /*
    * Writes the record information in the data disk-file and in the memory `std::map`
    */
    void write(Record &record) {
        data_file.open(this->data_file_name, std::ios::app | std::ios::ate | std::ios::binary);

        //< Writes the record in the data file
        long physicalPos = data_file.tellp();
        data_file.write((char *) &record, sizeof(Record));

        //< Stores the logical position of the record in memory
        this->index[record.getKey()] = (long) (physicalPos / sizeof(Record));

        data_file.close();
    }

    /*
    * Shows all the pairs key-position in the `std::map`
    */
    void showIndex() {
        for (auto &[key, position]: this->index) {
            std::cout << key << ": " << position << std::endl;
        }
    }

    void scanAll() {
        std::vector<Record> records;

        data_file.open(data_file_name, std::ios::in | std::ios::binary);

        if (!data_file.is_open()) {
            return;
        }

        data_file.seekg(0, std::ios::end);
        long length = data_file.tellg();
        data_file.seekg(0, std::ios::beg);

        /// Creates a buffer with the size of the data-file in bytes
        char *buffer = new char[length];
        /// Stores the data information in the buffer
        data_file.read(buffer, length);

        for (int i = 0; i < length; i += sizeof(Record)) {
            Record record{};

            // Access the buffer position where the ith record is located
            char *current = (buffer + i);

            // Copy fields from buffer to struct

            strncpy(record.name, current, sizeof(record.name));
            current += sizeof(record.name); //< advances `record.name` positions

            strncpy(record.career, current, sizeof(record.career));
            current += sizeof(record.career); //< advances `record.career` positions

            memcpy(&record.cycle, current, sizeof(record.cycle));

            records.push_back(record);
        }

        delete[] buffer;

        for (Record &record: records) {
            std::cout << record.toString() << std::endl;
        }
    }

    /*
    * Shows all the records sorted by key using the disk-file
    */
    void scanAllByIndex() {
        for (auto const &[key, position]: index) {
            data_file.open(data_file_name, std::ios::in | std::ios::binary);

            if (!data_file.is_open()) {
                throw std::runtime_error("Cannot open the file");
            }

            long offset = (long) (position * sizeof(Record));
            data_file.seekg(offset, std::ios::beg);

            Record record{};
            data_file.read((char *) &record, sizeof(Record));

            std::cout << record.toString() << std::endl;
        }
    }

    /*
    * Writes the index to the disk-file
    */
    void writeIndex() {
        index_file.open(this->index_file_name, std::ios::binary | std::ios::trunc);

        // Write pair info to file
        for (auto &[key, position]: index) {

            // Write the record-key
            char str[30]{-1};
            strcpy(str, key.c_str());
            index_file.write(str, 30);

            // Write record logical position
            index_file.write((char *) &position, sizeof(long));
        }

        index_file.close();
    }
};


#endif //RANDOM_FILE_HPP
