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

    std::map<std::string, long> index;  //< Ordered access to records physical position

    /*
    * Writes the index in the disk-file
    */
    void writeIndex() {
        index_file.open(this->index_file_name, std::ios::binary | std::ios::out);

        // Write pair info to file
        for (auto &[key, position]: index) {
            // Write the record-key
            char str[30];
            strcpy(str, key.c_str());
            index_file.write(str, 30);

            // Write record physical position
            index_file.write((char *) &position, sizeof(long));
        }

        index_file.close();
    }

    /*
    * Reads the disk-file index to load the information in RAM memory
    */
    void readIndex() {
        index_file.open(index_file_name, std::ios::in | std::ios::binary);
        if (!index_file.is_open()) {
            throw std::runtime_error("Cannot open the file: " + index_file_name);
        }

        int index_record_sz = 30 + sizeof(long);

        while (!index_file.eof()) {
            char *buffer = new char[index_record_sz];
            index_file.read(buffer, index_record_sz);

            char key[30];
            long position;

            std::memcpy(key, buffer, 30);
            std::memcpy(&position, buffer + 30, sizeof(long));

            delete[] buffer;

            if (index_file.eof()) {
                break;
            }
            index[std::string(key)] = position;
        }

        index_file.close();
    }

public:

    /// Loads the index-file information in `index`
    explicit RandomFile(std::string data_file_name, std::string index_file_name)
            : data_file_name(std::move(data_file_name)),
              index_file_name(std::move(index_file_name)) {
        readIndex();
    }

    /// Truncates the index-file information with the pairs key-position stored in `std::map`
    ~RandomFile() {
        writeIndex();
    }

    /*
    * Writes the record information in the data disk-file and in the memory `std::map`
    */
    void write(Record &record) {
        data_file.open(this->data_file_name, std::ios::app | std::ios::ate | std::ios::binary);

        //< Writes the record in the data file
        long position = data_file.tellp();
        data_file.write((char *) &record, sizeof(Record));

        //< Stores the physical position of the record in memory
        this->index[record.getKey()] = position;

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

    /*
    * Shows all the records (unordered) using the main disk-file `data.dat`
    */
    void scanAll() {
        std::vector<Record> records;
        data_file.open(data_file_name, std::ios::in | std::ios::binary);

        if (!data_file.is_open()) {
            throw std::runtime_error("Cannot open the file: " + data_file_name);
        }

        data_file.seekg(0, std::ios::end);
        long length = data_file.tellg();

        for (int i = 0; i < length; i += sizeof(Record)) {
            Record record{};

            // Access the record position
            data_file.seekg(i);

            // Copy fields from disk to RAM struct
            data_file.read((char *) &record, sizeof(Record));

            // push the record
            records.push_back(record);
        }

        for (Record &record: records) {
            std::cout << record.toString() << std::endl;
        }

        data_file.close();
    }

    /*
    * Shows all the records sorted by key using the `index` to locate the records in `data.dat`
    */
    void scanAllByIndex() {
        data_file.open(data_file_name, std::ios::in | std::ios::binary);
        if (!data_file.is_open()) {
            throw std::runtime_error("Cannot open the file: " + data_file_name);
        }

        for (auto const &[key, position]: index) {
            Record record{};
            data_file.seekg(position);
            data_file.read((char *) &record, sizeof(Record));

            std::cout << record.toString() << std::endl;
        }
        data_file.close();
    }

    Record search(const std::string &key) {
        auto it = index.find(key);
        if (it == index.end()) {
            throw std::runtime_error("The record with key: " + key + " do not exists");
        }
        long position = it->second;

        data_file.open(data_file_name, std::ios::in | std::ios::binary);
        if (!data_file.is_open()) {
            throw std::runtime_error("Cannot open the file: " + data_file_name);
        }

        Record record{};

        data_file.seekg(position);
        data_file.read((char *) &record, sizeof(Record));
        data_file.close();
        return record;
    }
};


#endif //RANDOM_FILE_HPP
