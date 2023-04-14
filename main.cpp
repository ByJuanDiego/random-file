#include "randomFile.hpp"

void writeFile(const std::string &filename, const std::string& index_filename) {
    RandomFile randomFile(filename, index_filename);
    Record record {};
    for (int i = 0; i < 4; i++) {
        record.setData();
        randomFile.write(record);
    }
}

void readFile(const std::string &filename, const std::string& index_filename) {
    RandomFile randomFile(filename, index_filename);
    std::cout << "-------- show data --- \n";
    randomFile.scanAll();
    std::cout << "-------- show sorted data --- \n";
    randomFile.scanAllByIndex();
}

int main() {
    writeFile("data.dat", "index.dat");
    readFile("data.dat", "index.dat");
}
