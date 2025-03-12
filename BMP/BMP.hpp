#pragma once

#include <string>
#include <cstdint>
#include <vector>

class BMP {
private:
    #pragma pack(push, 1)  // отключаем выравнивание структур для чтения бинарного файла

    struct BMPFileHeader {
        uint16_t type;           
        uint32_t size;          // размер файла в байтах
        uint16_t reserved1;      
        uint16_t reserved2;      
        uint32_t offset_data;   // смещение до данных пикселей
    };

    struct BMPInfoHeader {
        uint32_t size;                  // размер заголовка
        int32_t width;                  // ширина изображения
        int32_t height;                 // высота изображения
        uint16_t planes;                // число цветовых плоскостей
        uint16_t bits_per_pixel;        // количество бит на пиксель
        uint32_t compression;           // тип сжатия
        uint32_t size_image;            // размер данных изображения
        int32_t x_pixels_per_meter;     // разрешение по оси X
        int32_t y_pixels_per_meter;     // разрешение по оси Y
        uint32_t colors_used;           // число используемых цветов
        uint32_t colors_important;      // число важных цветов
    };

    struct BMPColorHeader {
        uint32_t red_mask;
        uint32_t green_mask;
        uint32_t blue_mask;
        uint32_t alpha_mask;        // прозрачность
        uint32_t color_space_type;  // цветовое пространство
        uint32_t unused;            // неиспользованные данные 
    };

    #pragma pack(pop)  // возвращаем стандартное выравнивание

    std::string input_filename;
    std::string output_filename;
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    BMPColorHeader color_header;
    size_t row_size;  // длина строки, хранящей данные о пикселях 

    std::vector<char> pixels;  // пиксели для отрисовки в консоле

public:
    BMP (const std::string& filename);  // конструктор
    void read_file();  // функция чтения BMP файла
    void print();  // функция вывода изображения на консоль
    void draw_line(int x1, int y1, int x2, int y2);  // функция рисования линии
    void draw_cross();  // функция рисования креста
    void save();  // функция сохранения в файл
};