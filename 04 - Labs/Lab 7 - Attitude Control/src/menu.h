#pragma once
#include <SerialMenu.hpp>
//
// const SerialMenu& menu = SerialMenu::get();
//
//
//
// // declare flight phases ////////
// enum MENU_CHOICES{ // define valid flight phase names
//     main = 0,
//     communication = 1,
//     electrical = 2,
//     att_determ = 3,
//     att_control = 4,
//     files = 5,
//   };
// MENU_CHOICES menu_status = main;  // variable flight_phase can hold one of the 5 valid phases
//
// const SerialMenuEntry communication_menu[];
// const uint8_t communication_menu_size;
//
// const SerialMenuEntry main_menu[] = {
//     {"1: communication", false, '1',
//         [](){menu.load(communication_menu, communication_menu_size);
//             menu.show(); } },
//     {"2: electrical", false, '2', Serial.println("electrical")},
// };
// constexpr uint8_t main_menu_size = GET_MENU_SIZE(main_menu);
//
// const SerialMenuEntry communication_menu[] = {
//     {"0: return to main menu", false, '0',
//         [](){menu.load(main_menu, main_menu_size);
//             menu.show(); } },
//     {"1: do lab A", false, '2', Serial.println("do lab A")},
// };
// constexpr uint8_t communication_menu_size = GET_MENU_SIZE(communication_menu);
//
//


 SerialMenu& menu = SerialMenu::get();

    // Forward declaration of menu2, because it is referenced before definition
    extern  SerialMenuEntry menu2[];
    extern  uint8_t menu2Size;

    // You can declare menu strings separately (a must for PROGMEM FLASH)
     char menu1String1[] = "Y - residplay this menu (Text in SRAM)";
     char menu1String2[] = "Z - second menu (Text in FLASH)";

    // Definition of menu1:
    // A menu entry is defined with four fields.
    // -Text can be embedded directly or you can reference a string name
    // -Text in FLASH via PROGMEM is flagged as true, else flagged as false
    // -Declare the keypress assigned to a menu entry (converts to lowercase)
    // -Declare the callback as a lambda function or use a function pointer
     SerialMenuEntry menu1[] = {
     {"X (Text in SRAM)", false, '1', [](){ Serial.println("choice X!"); } },
     {menu1String1,       false, 'y', [](){ menu.show(); } },
     {menu1String2,       false,  'z', [](){ menu.load(menu2, menu2Size);
                                            menu.show(); } }
    };
    constexpr uint8_t menu1Size = GET_MENU_SIZE(menu1);

    // Global variables updated by menu2
    uint16_t var1, var2;

    // Function called by menu2
    void foo()
    {
       // uint16_t var1  menu.get();
       Serial.println("Running foo!");
    }

    // Definition of menu2:
    // Notice that:
    // -Embedded strings can't be declared PROGMEM so we declare "false"
    // -Using 'B' vs 'b' doesn't matter (lowercase auto-conversion)
    // -We call the function foo() instead of a lambda function
    SerialMenuEntry menu2[] = {
     {"Execute foo()", false, 'e', foo },
     {"Set var2",      false, 'S', [](){ var2 = menu.getNumber<uint16_t>(); } },
     {"Redisplay menu",false, 'r', [](){ menu.show(); } },
     {"Back to menu1", false, 'B', [](){ menu.load(menu1, menu1Size);
                                         menu.show(); } }
    };
    uint8_t menu2Size = GET_MENU_SIZE(menu2);
