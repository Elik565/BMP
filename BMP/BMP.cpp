#include "BMP.hpp"
#include <fstream>
#include <iostream>

// Реализация конструктора
BMP::BMP(const std::string& filename) {
    input_filename = filename;
    read_file();
}

// Реализация функции чтения BMP файла
void BMP::read_file() {
    std::ifstream fin(input_filename, std::ios::binary);
    if (!fin) {
        std::cerr << "Ошибка открытия файла!\n";
        exit(1);
    }

    fin.read((char*)&file_header, sizeof(file_header));  // читаем заголовок файла

    if (file_header.type != 0x4D42) {  // формат файла должен быть 'BM', что соответствует 0x4D42
        std::cerr << "Необходимо ввести BMP файл!\n";
        exit(2);
    }

    fin.read((char*)&info_header, sizeof(info_header));  // читаем заголовок информации

    if (info_header.bits_per_pixel != 24 && info_header.bits_per_pixel != 32) {
        std::cerr << "Поддерживаются только BMP файлы 24 или 32 бита!\n";
        exit(3);
    }

    fin.read((char*)&color_header, sizeof(color_header));  // читаем заголовок цветов

    fin.seekg(file_header.offset_data, std::ios::beg);  // переходим к пиксельным данным

    // вычисляем длину строк, хранящих данные пикселей
    row_size = info_header.width * info_header.bits_per_pixel / 8;
    while (row_size % 4 != 0) {
        row_size++;
    }

    // Читаем пиксельные данные
    std::vector<uint8_t> row_data(row_size);

    // если высота отрицательная, то пиксели идут сверху вниз, поэтому возьмем ее по модулю
    info_header.height = abs(info_header.height);

    // проходимся по каждой строке пикселей
    for (size_t i = 0; i < info_header.height; i++) {
        fin.read((char*)row_data.data(), row_data.size());

        // проходимся по пикселям в строке
        for (size_t j = 0; j < info_header.width; j++) {
            int id = j * info_header.bits_per_pixel / 8;
            
            if (row_data[id] == 0 && row_data[id + 1] == 0 && row_data[id + 2] == 0) {
                pixels.push_back('*');  // если черный цвет
            }
            else if (row_data[id] == 255 && row_data[id + 1] == 255 && row_data[id + 2] == 255) {
                pixels.push_back(' ');  // если белый цвет
            }
            else {
                std::cerr << "В файлы есть не только белый и черный цвета!\n";
                exit(4);
            }
        }
    }

    fin.close();
}

// Реализация функции вывода изображения в консоль
void BMP::print() {
    // т.к. BMP хранит пиксели снизу вверх, то выводим соответственно
    for (int i = info_header.height - 1; i >= 0; i--) {
        for (int j = 0; j < info_header.width; j++) {
            std::cout << pixels[i * info_header.width + j];
        }
        std::cout << "\n";
    }
}

// Реализация функции рисования линии
void BMP::draw_line(int x1, int y1, int x2, int y2) {
    // будем использовать алгоритм Брезенхэма
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int direction_x = (x1 < x2) ? 1 : -1;  // направление движения по оси X
    int direction_y = (y1 < y2) ? 1 : -1;  // направление движения по оси Y
    int err = dx - dy;

    while (true) {
        // проверяем, что пиксели в пределах изображения
        if (x1 >= 0 && x1 < info_header.width && y1 >= 0 && y1 < info_header.height) {
            pixels[y1 * info_header.width + x1] = '#';
        }

        // если дошли до конца, выходим из цикла
        if (x1 == x2 && y1 == y2) {
            break;
        } 

        int e2 = err * 2;
        if (e2 > -dy) { err -= dy; x1 += direction_x; }
        if (e2 < dx) { err += dx; y1 += direction_y; }
    }
}

// Реализация функции рисования креста
void BMP::draw_cross() {
    draw_line(0, 0, info_header.width - 1, info_header.height - 1);  // из левого нижнего угла 
    draw_line(info_header.width - 1, 0, 0, info_header.height - 1);  // из правого нижнего угла
}

void BMP::save() {
    std::cout << "Введите название файла для сохранения: ";
    std::cin >> output_filename;

    // проверяем на корректность ввода
    if (output_filename.size() < 4 || output_filename.substr(output_filename.size() - 4) != ".bmp") {
        std::cerr << "Файл должен иметь расширение '.bmp'!\n";
        exit(5);
    }

    std::ofstream fout(output_filename, std::ios::binary);

    if (!fout) {
        std::cerr << "Ошибка при создании файла!\n";
        exit(6);
    }

    fout.write((char*)&file_header, sizeof(file_header));  // записываем заголовок файла
    fout.write((char*)&info_header, sizeof(info_header));  // записываем заголовок информации
    fout.write((char*)&color_header, sizeof(color_header));  // записываем заголовок цветов

    fout.seekp(file_header.offset_data, std::ios::beg);  // переходим к пиксельным данным

    std::vector<uint8_t> row_data(row_size, 255);
    
    for (int i = info_header.height - 1; i >= 0; i--) {
        for (int j = 0; j < info_header.width; j++) {
            uint8_t* pixel = &row_data[j * info_header.bits_per_pixel / 8];

            if (pixels[i * info_header.width + j] == '*') {
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
            }
            else if (pixels[i * info_header.width + j] == ' ') {
                pixel[0] = 255;
                pixel[1] = 255;
                pixel[2] = 255;
            }
            else {
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 255;
            }
        }
        fout.write((char*)row_data.data(), row_data.size());  // записываем строку пикселей
    }

    fout.close();
}