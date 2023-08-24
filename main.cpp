#include "randomFile.hpp"

void writeFile(RandomFile& randomFile, int n) {
    Record record {};
    for (int i = 0; i < n; i++) {
        record.setData();

        try {
            randomFile.write(record);
        }
        catch (std::exception& e) {
            std::cout << std::endl << e.what();
        }

        std::cout << std::endl;
    }
}

[[maybe_unused]] void readFile(RandomFile& randomFile) {
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
    writeFile(randomFile, 4);
    searchRecord(randomFile);

    return EXIT_SUCCESS;
}
