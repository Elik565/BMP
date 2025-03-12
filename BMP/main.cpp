#include "BMP.hpp"
#include <iostream>

int main() {
    // ввод DMP файла
    std::string filename;
    std::cout << "Enter input BMP file name: ";
    std::cin >> filename;

    BMP BMP_reader(filename);
    BMP_reader.print();
    BMP_reader.draw_cross();
    std::cout << "Изображение после обработки:\n";
    BMP_reader.print();
    BMP_reader.save();

    return 0;
}