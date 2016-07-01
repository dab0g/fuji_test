#include <iostream>

using namespace std;

#define mode_heat 0
#define mode_fan  1
#define mode_auto 2
#define mode_cool 3
#define mode_dry  4

#define fan_auto  4
#define fan_high  3
#define fan_med   2
#define fan_low   1
#define fan_quiet 0

uint16_t messageRAW[243] = {}; // Готовые для отправки данные в RAW формате.
//uint16_t messageClean[15] = {0x28, 0xC6, 0x00, 0x08, 0x08, 0x3F, 0x10, 0x0C, 0x86, 0x80, 0x80, 0x00, 0x00, 0x00, 0xb6};

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
                            0x05, 0x09, 0x01, 0x0E, 0x06, 0x0A, 0x02, 0x0C, 0x04, 0x08, 0x00, 0x0f, 0x07};

//                            16     17  18    19    20    21    22    23    24    25    26    27    29    29    30
    uint8_t temp_arr2[16] = {0x00, 0x08,0x04, 0x0c, 0x02, 0x0A, 0x06, 0x0E, 0x01, 0x09, 0x05, 0x0D, 0x03, 0x0B, 0x07,
                             0x0f};




    // Массив перевёрнутых бит от 1 до 5
    // Используется в режиме работу кондиционера, режиме AUTO, скоростях турбины
    uint8_t array_mode[5] = {  0x20, 0xC0, 0x00, 0x80, 0x40 };
                                //0xA0, 0x60, 0xA0, 0x20, 0xC0};

    //                          auto  high   med   low  quiet
    uint8_t array_fan[13] =  {  0x20, 0xC0, 0x40, 0x80, 0x00,
                                0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0}; // у каждого режима в 15 байте своё смещение в массиве


    //uint8_t array_fan[13] =  {  0x00, 0x80, 0x40, 0xC0, 0x20,
    //                            0xE0, 0x60, 0xA0, 0x20, 0xC0, 0x40, 0x80, 0x00}; // у каждого режима в 15 байте своё смещение в массиве

    uint8_t array_fan_offset[5] = {};


    // Используется в режиме AUTO
    // режим AUTO                 1     2     3     4     5
    uint8_t array_auto[10] = {  0x40, 0x20, 0x60, 0x10, 0x50,
                                0x30, 0x50, 0x10, 0x60, 0x20};


    uint8_t mode = mode_cool;
    uint8_t temp = 30; // Текушая температура (18 - 30)
    uint8_t auto_m = 1; // Уровень AUTO (1 - 5)
    uint8_t fan_speed = 0; // 0 - auto, 1 - high, 2 - med, 3 - low, 4 - quiet - скорость турбины
    bool poweron = false;
    // короткая посылка - 6 быйт
    bool poweroff = false;
    bool air_dir = false;
    bool swing = false;

} Fuji;


int main() {

    uint8_t lenght; // длинна посылки в байтах
    if (Fuji.poweroff == true or Fuji.air_dir == true or Fuji.swing == true) {
        lenght = 6;
        if (Fuji.poweroff == true) {
            Fuji.message[5] = 0x40;
        } else if (Fuji.air_dir == true) {
            Fuji.message[5] = 0x36;
        } else {
            Fuji.message[5] = 0xB6;
        }
        for (uint8_t i=0; i<lenght; i++){
            cout << hex << int(Fuji.message[i]) << " ";
        }

    } else {

        // -------- Формируем 10й байт ----------- start
        Fuji.message[9] = 0;
        Fuji.message[9] &= 0x1F; // Очищаем первые три бита
        Fuji.message[9] |= (Fuji.array_mode[Fuji.mode]);
        cout << "10 byte: ";
        cout << hex << int(Fuji.message[9]) << endl;

        // -------- Формируем 10й байт ----------- end
        if (Fuji.mode == mode_heat) {

        } else if (Fuji.mode == mode_fan) {

        } else if (Fuji.mode == mode_auto) {

        } else if (Fuji.mode == mode_cool) {

        } else {
// mode DRY

        }
    // Формируем биты температуры
/*
    // -------- Формируем 9й байт ----------- start
    Fuji.message[8]=0; // очищаем девятый байт
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

        lenght = 15;

        for (uint8_t i=0; i<lenght; i++){
            cout << hex << int(Fuji.message[i]) << " ";
        }
*/
    }


    return 0;
}