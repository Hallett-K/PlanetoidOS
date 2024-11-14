#include <vga.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_BUFFER = (uint16_t*) 0xB8000;

static size_t vga_row;
static size_t vga_column;
static uint8_t vga_color;
static uint16_t* vga_buffer;

void vga_init(void)
{
    vga_row = 0;
    vga_column = 0;
    vga_color = vga_make_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_buffer = VGA_BUFFER;

    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color);
        }
    }
}

void vga_plot(unsigned char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    vga_buffer[index] = vga_entry(c, color);
}

void vga_putchar(char c)
{
    if (c == '\n')
    {
        vga_row++;
        vga_column = 0;
        if (vga_row == VGA_HEIGHT)
        {
            vga_row = 0;
        }
        return;
    }

    unsigned char uc = c;
    vga_plot(uc, vga_color, vga_column, vga_row);
    if (++vga_column == VGA_WIDTH)
    {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT)
        {
            vga_row = 0;
        }
    }
}

void vga_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        vga_putchar(data[i]);
    }
}

size_t strlen(const char* str) // TEMP
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void vga_writestring(const char* data)
{
    vga_write(data, strlen(data));
}

void vga_writeint(uint64_t n)
{
    char buffer[21];
    char* pos = buffer + sizeof(buffer) - 1;
    *pos = '\0';

    do {
        *--pos = '0' + n % 10;
        n /= 10;
    } while (n > 0);

    vga_write(pos, buffer + sizeof(buffer) - pos);
}