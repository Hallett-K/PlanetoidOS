#include <keyboard.h>
#include <idt.h>
#include <io.h>

// 1 = esc
static const char scancode_to_ascii[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
        '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
    };

    static const char shift_scancode_to_ascii[] = {
        0, 0, '!', '"', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '@', '~', 0,
        '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
    };

    static const char caps_lock_scancode_to_ascii[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0,
        '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' '
    };

uint8_t shift_state = 0;
uint8_t caps_lock_state = 0;

void keyboard_init(void)
{
    irq_set_handler(1, on_irq1);
    (void)scancode_to_ascii;
    (void)shift_scancode_to_ascii;
    (void)caps_lock_scancode_to_ascii;
}

void on_irq1(struct interrupt_frame* frame)
{
    (void)frame; // Unused but hides warning

    uint8_t indata = _io_inb(0x60);
    uint8_t scancode = indata & 0x7F;
    uint8_t action = (indata >> 7) & 0x1; // 0 if key pressed, 1 if key released

    serial_write_string(SERIAL_PORT_COM1, "Scancode: ");
    serial_write_int(SERIAL_PORT_COM1, scancode);
    serial_write_string(SERIAL_PORT_COM1, " Action: ");
    serial_write_int(SERIAL_PORT_COM1, action);
    serial_write_string(SERIAL_PORT_COM1, "\n");

    switch (scancode)
    {
        case PL_KEY_LEFT_SHIFT:
        case PL_KEY_RIGHT_SHIFT:
            shift_state = !action;
            break;
        case PL_KEY_CAPS_LOCK:
            if (action == 0)
                caps_lock_state = !caps_lock_state;
            break;
    }
}