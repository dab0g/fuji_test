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

uint8_t message[15] = {}; // Готовые для отправки данные в RAW формате.
//uint16_t messageClean[15] = {0x28, 0xC6, 0x00, 0x08, 0x08, 0x3F, 0x10, 0x0C, 0x86, 0x80, 0x80, 0x00, 0x00, 0x00, 0xb6};


uint8_t mode = mode_dry;
uint8_t temp = 18; // Текушая температура (18 - 30)
uint8_t auto_m = 0; // Уровень AUTO (0 - 4)
uint8_t fan_speed = fan_auto; // 4 - auto, 3 - high, 2 - med, 1 - low, 0 - quiet - скорость турбины
bool poweron = true;
// короткая посылка - 6 быйт
bool poweroff = false;
bool air_dir = false;
bool swing = false;


uint8_t* buildMsg(uint8_t mode, uint8_t temp, uint8_t auto_m, uint8_t fan_speed, bool poweron, bool poweroff, bool air_dir, bool swing) {



    struct IR_Fuji // Создаём структуру температурных датчиков ds18b20
    {
        // Почти готовое сообщение - полный формат.
        // перед передачей к нему должен быть добавлен заголовок, окончание и оно должно быть переведено в RAW формат отправки
        //                                                    |                  |    |     |                        |
        uint8_t message[15] = {0x28, 0xC6, 0x00, 0x08, 0x08, 0x3F, 0x10, 0x0C, 0x86, 0x80, 0x80, 0x00, 0x00, 0x00, 0xb6};

        // биты в посылке перевёрнуты, потому и делаем массивы
        // битовый массив для температуры
//                           16    17    18    19    20    21    22    23    24    25    26    27    28    29    30
        uint8_t temp_arr[30] = {0x00, 0x08, 0x04, 0x0c, 0x02, 0x0A, 0x06, 0x0E, 0x01, 0x09, 0x05, 0x0D, 0x03, 0x0B, 0x07,
                                0x03, 0x0D, 0x05, 0x09, 0x01, 0x0E, 0x06, 0x0A, 0x02, 0x0C, 0x04, 0x08, 0x00, 0x0f, 0x07};

//                            16     17  18    19    20    21    22    23    24    25    26    27    29    29    30
        //uint8_t temp_arr2[16] = {0x00, 0x08,0x04, 0x0c, 0x02, 0x0A, 0x06, 0x0E, 0x01, 0x09, 0x05, 0x0D, 0x03, 0x0B, 0x07,
        //                        0x0f};




        // Массив перевёрнутых бит от 1 до 5
        // Используется в режиме работу кондиционера, режиме AUTO, скоростях турбины
        uint8_t array_mode[5] = {  0x20, 0xC0, 0x00, 0x80, 0x40 };
        //0xA0, 0x60, 0xA0, 0x20, 0xC0};

        //                          quiet  low  med   high  auto
        uint8_t array_fan[13] =  {  0x20, 0xC0, 0x40, 0x80, 0x00,
                                    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0}; // у каждого режима в 15 байте своё смещение в массиве

        uint8_t array_fan_dry[5] =  {0x50, 0xD0, 0x30, 0xB0, 0x70}; // у каждого режима в 15 байте своё смещение в массиве


        // Используется в режиме AUTO
        // режим AUTO                 0     1     2     3     4
        uint8_t array_auto[10] = {  0x40, 0x20, 0x60, 0x10, 0x50,
                                    0x50, 0x10, 0x60, 0x20, 0x40};


        uint8_t mode; // = mode_dry;
        uint8_t temp; // = 18; // Текушая температура (18 - 30)
        uint8_t auto_m; // = 0; // Уровень AUTO (0 - 4)
        uint8_t fan_speed; // = fan_auto; // 4 - auto, 3 - high, 2 - med, 1 - low, 0 - quiet - скорость турбины
        bool poweron; // = true;
        // короткая посылка - 6 быйт
        bool poweroff; // = false;
        bool air_dir; // = false;
        bool swing; // = false;

    } Fuji;

    Fuji.mode = mode;
    Fuji.temp = temp;
    Fuji.auto_m = auto_m;
    Fuji.fan_speed = fan_speed;
    Fuji.poweron = poweron;
    Fuji.poweroff = poweroff;
    Fuji.air_dir = air_dir;
    Fuji.swing = swing;

    for (int v=18; v<31; v++) {
        Fuji.temp = v;
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
            for (uint8_t i = 0; i < lenght; i++) {
                cout << hex << int(Fuji.message[i]) << " ";
            }

        } else {


            // режим работы
            Fuji.message[9] = 0;
            //Fuji.message[9] &= 0x1F; // Очищаем первые три бита
            Fuji.message[9] |= (Fuji.array_mode[Fuji.mode]);

            Fuji.message[8] = 0; // очищаем девятый байт


            if (Fuji.temp > 30) Fuji.temp = 30;

            Fuji.message[14] = 1;
            Fuji.message[14] = Fuji.message[14] << 4;

            if (Fuji.temp < 16 and Fuji.mode == mode_heat) {
                Fuji.temp = 16;
            } else if (Fuji.temp < 18 and Fuji.mode != mode_heat) {
                Fuji.temp = 18;
            }

            //Fuji.message[10] &= 0x1F; // Очищаем первые три бита
            Fuji.message[10] = 0x00; // Очищаем
            Fuji.message[10] |= (Fuji.array_fan[Fuji.fan_speed]);

            if (Fuji.mode == mode_heat) {
                Fuji.message[8] |= Fuji.temp_arr[Fuji.temp - 16];
                Fuji.message[14] |= Fuji.temp_arr[Fuji.temp - 1];
                Fuji.message[14] |= (Fuji.array_fan[Fuji.fan_speed + 5]);
            } else if (Fuji.mode == mode_fan) {
                Fuji.message[8] |= Fuji.temp_arr[Fuji.temp - 16];
                Fuji.message[14] |= Fuji.temp_arr[Fuji.temp - 1];
                Fuji.message[14] |= (Fuji.array_fan[Fuji.fan_speed + 6]);
            } else if (Fuji.mode == mode_auto) {
                Fuji.message[14] = 0;
                // температура
                Fuji.message[8] |= Fuji.temp_arr[Fuji.temp - 16];
                Fuji.message[14] |= Fuji.temp_arr[Fuji.temp - 1];

                // вентилятор
                Fuji.message[14] &= 0x1F; // Очищаем первые три бита
                Fuji.message[14] |= Fuji.array_fan[Fuji.fan_speed+7];

                // уровень режима AUTO (0 - 4)
                Fuji.message[14] &= 0x8F; // Очищаем первые три бита
                Fuji.message[8]  |= Fuji.array_auto[Fuji.auto_m];
                Fuji.message[14] |= Fuji.array_auto[Fuji.auto_m + 5];

            } else if (Fuji.mode == mode_cool) {
                Fuji.message[8] |= Fuji.temp_arr[Fuji.temp - 16];
                Fuji.message[14] |= Fuji.temp_arr[Fuji.temp - 1];
                Fuji.message[14] |= (Fuji.array_fan[Fuji.fan_speed + mode_cool + 5]);
            } else {
// mode DRY
                Fuji.message[8] |= Fuji.temp_arr[Fuji.temp - 16];
                Fuji.message[14] &= 0x8F; // Очищаем первые три бита
                Fuji.message[14] |= Fuji.temp_arr[Fuji.temp - 1];
                Fuji.message[14] &= 0x1F; // Очищаем первые три бита
                Fuji.message[14] |= (Fuji.array_fan_dry[Fuji.fan_speed]); //7
            }

            // Формируем биты температуры
            lenght = 15;

            cout << std::dec << int(Fuji.temp) << " - ";
            for (uint8_t i = 0; i < lenght; i++) {
                cout << hex << int(Fuji.message[i]) << " ";
            }
            cout << endl;
        }

    }
    //return Fuji.message;
}



//uint8_t  = mode_dry;
//uint8_t  = 18; // Текушая температура (18 - 30)
//uint8_t  = 0; // Уровень AUTO (0 - 4)
//uint8_t  = fan_auto; // 4 - auto, 3 - high, 2 - med, 1 - low, 0 - quiet - скорость турбины
//bool  = true;
// короткая посылка - 6 быйт
//bool  = false;
//bool  = false;
//bool  = false;


int main() {

    buildMsg(mode, temp, auto_m, fan_speed, poweron, poweroff, air_dir, swing);
    return 0;
}