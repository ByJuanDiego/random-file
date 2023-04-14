#include "randomFile.hpp"

void writeFile(RandomFile& randomFile) {
    Record record {};
    for (int i = 0; i < 4; i++) {
        record.setData();
        randomFile.write(record);
        std::cout << std::endl;
    }
}

void readFile(RandomFile& randomFile) {
    std::cout << "-------- show index --- \n";
    randomFile.showIndex();
    std::cout << "-------- show data --- \n";
    randomFile.scanAll();
    std::cout << "-------- show sorted data --- \n";
    randomFile.scanAllByIndex();
    std::cout << "\n";
}

void searchRecord(RandomFile& randomFile) {
    std::string search;
    std::cout << "Key to be searched: ";
    std::cin >> search;

    Record record = randomFile.search(search);
    std::cout << "Record information: " << record.toString() << std::endl;
}

int main() {
    RandomFile randomFile("data.dat", "index.dat");
    writeFile(randomFile);
    readFile(randomFile);
    searchRecord(randomFile);
}
