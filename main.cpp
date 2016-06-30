#include <iostream>

using namespace std;

#define mode_auto 0
#define mode_cool 1
#define mode_dry  2
#define mode_fan  3
#define mode_heat 4


//uint8_t mode = mode_auto; // режим работы кондиционера
uint16_t messageRAW[243] = {}; // Готовые для отправки данные в RAW формате.

struct IR_Fuji // Создаём структуру температурных датчиков ds18b20
{
    // Почти готовое сообщение - полный формат.
    // перед передачей к нему должен быть добавлен заголовок, окончание и оно должно быть переведено в RAW формат отправки
    //                                                    |                  |    |     |                        |
    uint8_t message[15] = {0x28, 0xC6, 0x00, 0x08, 0x08, 0x3F, 0x10, 0x0C, 0x86, 0x80, 0x80, 0x00, 0x00, 0x00, 0xb6};

    // биты в посылке перевёрнуты, потому и делаем массивы
    // битовый массив для температуры
//                           18    19    20    21    22    23    24    25    26    27    28    29    30
    uint8_t temp_arr[26] = {0x04, 0x0c, 0x02, 0x0A, 0x06, 0x0E, 0x01, 0x09, 0x05, 0x0D, 0x03, 0x0B, 0x07,
                            0x05, 0x09, 0x01, 0x0E, 0x06, 0x0A, 0x02, 0x0C, 0x04, 0x08, 0x00, 0xf, 0x07};
    // режим AUTO           1     2     3     4     5
    //uint8_t auto_arr[5] = {0x40, 0x20, 0x60, 0x10, 0x50}; // Массив бит режима AUTO
    //uint8_t mode_arr[5] = {0x40, 0x20, 0x60, 0x10, 0x50}; // массив режимов работы кондиционера

    // Массив перевёрнутых бит от 1 до 5
    // Используется в режиме работу кондиционера, режиме AUTO, скоростях турбины
    uint8_t array_mode[10] = {  0x00, 0x80, 0x40, 0xC0, 0x20,
                                0xA0, 0x60, 0xA0, 0x20, 0xC0};

    uint8_t array_fan[10] =  {  0x00, 0x80, 0x40, 0xC0, 0x20,
                                0xE0, 0x60, 0xA0, 0x20, 0xC0};


    // Используется в режиме AUTO
    uint8_t array_auto[10] = {  0x40, 0x20, 0x60, 0x10, 0x50,
                                0x30, 0x50, 0x10, 0x60, 0x20};


    uint8_t mode = mode_cool;
    uint8_t temp = 22; // Текушая температура (18 - 30)
    uint8_t auto_m = 1; // Уровень AUTO (1 - 5)
    uint8_t fan_speed = 2; // 0 - auto, 1 - high, 2 - med, 3 - low, 4 - quiet - скорость турбины

} Fuji;


int main() {


    // Формируем биты температуры

    // -------- Формируем 9й байт ----------- start
    Fuji.message[8]=0; // очищаем пятый байт
    Fuji.message[8] |= Fuji.temp_arr[Fuji.temp-18];
    cout << int(Fuji.message[8]) << endl;

    if (Fuji.mode == mode_auto) {
        Fuji.message[8] |= Fuji.array_auto[Fuji.auto_m];
        cout << "AUTO: ";
        cout << int(Fuji.message[8]) << endl;
    }

    // -------- Формируем 9й байт ------------ end

    // -------- Формируем 10й байт ----------- start

    Fuji.message[9] &= 0x1F; // Очищаем первые три бита
    Fuji.message[9] |= (Fuji.array_mode[Fuji.mode]);
    cout << "10 byte: ";
    cout << int(Fuji.message[9]) << endl;

    // -------- Формируем 10й байт ----------- end

    // -------- Формируем 11й байт ----------- start

    Fuji.message[10] &= 0x1F; // Очищаем первые три бита
    Fuji.message[10] |= (Fuji.array_fan[Fuji.fan_speed]);
    cout << "11 byte: ";
    cout << int(Fuji.message[10]) << endl;

    // -------- Формируем 11й байт ----------- end

    // -------- Формируем 15й байт ----------- start

    cout << "15 byte: ";
    Fuji.message[14] &= 0xF0; // Очищаем последние четыре бита
    Fuji.message[14] |= Fuji.temp_arr[Fuji.temp-18];
    cout << int(Fuji.message[14]) << endl;

    Fuji.message[14] &= 0x0F; // Очищаем 7-4 биты
    if (Fuji.mode == mode_auto) {
        Fuji.message[14] |= Fuji.array_auto[Fuji.auto_m];
        cout << "AUTO: ";
        cout << int(Fuji.message[14]) << endl;
    }

    Fuji.message[14] |= (Fuji.array_fan[Fuji.fan_speed+5]);
    cout << int(Fuji.message[14]) << endl;


    // -------- Формируем 15й байт ----------- end

    return 0;
}