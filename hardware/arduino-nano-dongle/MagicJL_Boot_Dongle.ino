/*
 * MagicJL Arduino Nano boot/programming dongle
 *
 * AC6905, AC6925 and AC6956 family chips have been tested with this approach.
 * It disconnects USB, power-cycles the target, then sends 0x16EF over D+/D-
 * immediately after power-up to enter the Jieli bootloader.
 */

#define PIN_DATA 11        // Chip D- / white wire
#define PIN_CLOCK 10       // Chip D+ / green wire
#define PIN_CHIP_VCC A5    // 0 = target power on, 1 = target power off
#define PIN_USB_CONNECT 2  // USB connection relay
#define PIN_PU 3           // External pull-up
#define PIN_SW A0          // 1 = normal mode, 0 = boot mode
#define PIN_LED_R 5        // Boot mode indicator
#define PIN_LED_G 4        // Normal mode indicator

#define BOOT_MODE 1
#define NORMAL_MODE 0

uint8_t mode = 255;
uint8_t boot_mode_ok = 0;

void send_key()
{
    for (int8_t i = 15; i >= 0; i--) {
        delayMicroseconds(15);
        digitalWrite(PIN_DATA, (0x16EF >> i) & 0x1);
        digitalWrite(PIN_CLOCK, 0);
        delayMicroseconds(15);
        digitalWrite(PIN_CLOCK, 1);
    }
}

void go_boot_mode()
{
    if (mode != BOOT_MODE) {
        mode = BOOT_MODE;
        digitalWrite(PIN_LED_G, 0);
        digitalWrite(PIN_USB_CONNECT, 0);
        digitalWrite(PIN_CHIP_VCC, 1);
        delay(1000); // Discharge target VCC capacitors.
        digitalWrite(PIN_CHIP_VCC, 0);
        delay(10);

        while (!boot_mode_ok) {
            pinMode(PIN_CLOCK, OUTPUT);
            pinMode(PIN_DATA, OUTPUT);
            send_key();

            pinMode(PIN_CLOCK, INPUT);
            pinMode(PIN_DATA, INPUT);
            pinMode(PIN_PU, OUTPUT);
            digitalWrite(PIN_PU, 1);

            if (!digitalRead(PIN_DATA)) {
                boot_mode_ok = 1; // Chip pulled D- down.
                pinMode(PIN_PU, INPUT);
                digitalWrite(PIN_USB_CONNECT, 1);
                digitalWrite(PIN_LED_R, 1);
            }

            pinMode(PIN_PU, INPUT);
            if (digitalRead(PIN_SW)) {
                break;
            }
        }
    }
}

void go_normal_mode()
{
    if (mode != NORMAL_MODE) {
        digitalWrite(PIN_LED_R, 0);
        digitalWrite(PIN_USB_CONNECT, 0);
        digitalWrite(PIN_CHIP_VCC, 1);
        delay(1000); // Discharge target VCC capacitors.
        digitalWrite(PIN_CHIP_VCC, 0);
        digitalWrite(PIN_USB_CONNECT, 1);
        digitalWrite(PIN_LED_G, 1);
        boot_mode_ok = 0;
        mode = NORMAL_MODE;
    }
}

void setup()
{
    pinMode(PIN_LED_G, OUTPUT);
    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_CHIP_VCC, OUTPUT);
    pinMode(PIN_USB_CONNECT, OUTPUT);
    pinMode(PIN_SW, INPUT_PULLUP);
    pinMode(PIN_CLOCK, INPUT);
    pinMode(PIN_DATA, INPUT);
    pinMode(PIN_PU, INPUT);

    digitalWrite(PIN_LED_G, 0);
    digitalWrite(PIN_LED_R, 0);
    digitalWrite(PIN_CHIP_VCC, 1);
    digitalWrite(PIN_USB_CONNECT, 0);

    if (digitalRead(PIN_SW)) {
        go_normal_mode();
    } else {
        go_boot_mode();
    }
}

void loop()
{
    if (digitalRead(PIN_SW)) {
        go_normal_mode();
    } else {
        go_boot_mode();
    }
}
