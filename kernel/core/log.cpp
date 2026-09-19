#include "log.hpp"

#include <cstdarg>

#include "memory/kernel_heap.hpp"


extern "C"
{
    void uart_putc(char c);
    void uart_puts(const char* s);
}

namespace
{
    struct FormatSpec
    {
        bool prefix;
        uint32_t width;
        char type;
    };
}

namespace
{
    FormatSpec parse_format(const char*& fmt)
    {
        FormatSpec spec;
        spec.prefix = false;
        spec.width = 0;
        spec.type = 0;

        if (*fmt == '#')
        {
            spec.prefix = true;
            fmt++;
        }

        while (*fmt >= '0' && *fmt <= '9')
        {
            spec.width = spec.width * 10 + (*fmt - '0');
            fmt++;
        }

        spec.type = *fmt;
        return spec;
    }

    uint64_t pad_left(char* buffer, uint64_t count, uint64_t width)
    {
        if (count >= width)
        {
            return count;
        }

        uint64_t pad = width - count;

        for (uint64_t i = count; i > 0; i--)
        {
            buffer[i + pad - 1] = buffer[i - 1];
        }

        for (uint64_t i = 0; i < pad; i++)
        {
            buffer[i] = '0';
        }

        return width;
    }

    uint64_t format_uint(uint64_t value, char* buffer)
    {
        char temp[20];
        uint64_t count = 0;

        if (value == 0)
        {
            buffer[0] = '0';
            return 1;
        }

        while (value > 0)
        {
            temp[count++] = char('0' + (value % 10));
            value /= 10;
        }

        for (uint64_t i = 0; i < count; i++)
        {
            buffer[i] = temp[count - 1 - i];
        }

        return count;
    }

    uint64_t format_int(int64_t value, char* buffer)
    {
        uint64_t pos = 0;

        if (value < 0)
        {
            buffer[pos++] = '-';
            value = -value;
        }

        return pos + format_uint(uint64_t(value), buffer + pos);
    }

    uint64_t format_hex(uint64_t value, char* buffer, bool prefix, uint32_t width)
    {
        const char* digits = "0123456789ABCDEF";
        char temp[32];
        uint64_t count = 0;

        if (value == 0)
        {
            temp[count++] = '0'; 
        }
        else
        {
            while (value > 0)
            {
                temp[count++] = digits[value & 0xF];
                value >>= 4;
            }
        }

        for (uint64_t i = 0; i < count; i++)
        {
            buffer[i] = temp[count - 1 - i];
        }

        count = pad_left(buffer, count, width);

        if (prefix)
        {
            for (uint64_t i = count; i > 0; i--)
            {
                buffer[i + 1] = buffer[i - 1];
            }

            buffer[0] = '0';
            buffer[1] = 'x';
            return count + 2;
        }

        return count;
    }

    uint64_t format_bin(uint64_t value, char* buffer, bool prefix, uint32_t width)
    {
        char temp[66];
        uint64_t count = 0;

        if (value == 0)
        {
            temp[count++] = '0';
        }
        else
        {
            while (value > 0)
            {
                temp[count++] = (value & 1) ? '1' : '0';
                value >>= 1;
            }
        }

        for (uint64_t i = 0; i < count; i++)
        {
            buffer[i] = temp[count - 1- i];
        }

        count = pad_left(buffer, count, width);

        if (prefix)
        {
            for (uint64_t i = count; i > 0; i--)
            {
                buffer[i + 1] = buffer[i - 1];
            }

            buffer[0] = '0';
            buffer[1] = 'b';
            return count + 2;
        }

        return count;
    }

    void format_string(char* out, uint64_t out_size, const char* fmt, va_list args)
    {
        uint64_t pos = 0;

        while (*fmt && pos < out_size - 1)
        {
            if (*fmt != '%')
            {
                out[pos++] = *fmt++;
                continue;
            }

            fmt++;
            FormatSpec spec = parse_format(fmt);

            switch (spec.type)
            {
                case 's':
                {
                    const char* str = va_arg(args, const char*);
                    if (!str)
                    {
                        str = "(null)";
                    }

                    while (*str && pos < out_size - 1)
                    {
                        out[pos++] = *str++;
                    }
                    break;
                }

                case 'u':
                {
                    unsigned int v = va_arg(args, unsigned int);
                    pos += format_uint((uint64_t)v, out + pos);
                    break;
                }

                case 'i':
                {
                    int v = va_arg(args, int);
                    pos += format_int((int64_t)v, out + pos);
                    break;
                }

                case 'x':
                {
                    uint64_t v = va_arg(args, uint64_t);
                    pos += format_hex(v, out + pos, spec.prefix, spec.width);
                    break;
                }

                case 'b':
                {
                    uint64_t v = va_arg(args, uint64_t);
                    pos += format_bin(v, out + pos, spec.prefix, spec.width);
                    break;
                }

                default:
                    out[pos++] = '%';
                    out[pos++] = *fmt;
                    break;
            }

            fmt++;
        }

        out[pos] = '\0';
    }
}

namespace Log
{
    void info(const char* format, ...)
    {
        uart_puts("[INFO] ");

        char buffer[1024];

        va_list args;
        va_start(args, format);

        format_string(buffer, sizeof(buffer), format, args);

        va_end(args);

        uart_puts(buffer);
        uart_puts("\r\n");
    }

    void error(const char* format, ...)
    {
        uart_puts("[ERROR] ");

        char buffer[1024];

        va_list args;
        va_start(args, format);

        format_string(buffer, sizeof(buffer), format, args);

        va_end(args);

        uart_puts(buffer);
        uart_puts("\r\n");
    }
}