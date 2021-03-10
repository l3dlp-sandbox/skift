#pragma once

#include <libutils/Scanner.h>
#include <libutils/String.h>

#include <libsystem/io_new/MemoryWriter.h>
#include <libsystem/io_new/Writer.h>

class String;

namespace System
{

struct Formating
{
    enum Align
    {
        LEFT,
        CENTER,
        RIGHT,
    };

    enum Type
    {
        DEFAULT,
        STRING,
        CHARACTER,
        BINARY,
        DECIMAL,
        OCTAL,
        HEXADECIMAL,
    };

    bool prefix;
    Type type = DEFAULT;

    static Formating parse(Scanner &scanner)
    {
        Formating format{};

        scanner.skip('{');

        auto parse_type = [&]() {
            switch (scanner.current())
            {
            case 's':
                format.type = STRING;
                break;

            case 'c':
                format.type = CHARACTER;
                break;

            case 'b':
                format.type = BINARY;
                break;

            case 'd':
                format.type = DECIMAL;
                break;

            case 'o':
                format.type = OCTAL;
                break;

            case 'x':
                format.type = HEXADECIMAL;
                break;

            default:
                break;
            }
        };

        if (scanner.current_is("scbdox"))
        {
            parse_type();
            scanner.foreward();
        }

        while (!scanner.ended() && scanner.current() != '}')
        {

            scanner.foreward();
        }

        scanner.skip('}');

        return format;
    }
};

class Format
{
public:
    virtual ResultOr<size_t> format(Writer &writer);
};

template <typename T>
concept Formatable = IsBaseOf<Format, T>::value || requires(Writer &writer, const Formating &formating, const T &t)
{
    format(writer, formating, t);
};

ResultOr<size_t> format(Writer &, const Formating &, char);

ResultOr<size_t> format(Writer &, const Formating &, unsigned char);

ResultOr<size_t> format(Writer &, const Formating &, short int);

ResultOr<size_t> format(Writer &, const Formating &, unsigned short int);

ResultOr<size_t> format(Writer &, const Formating &, int);

ResultOr<size_t> format(Writer &, const Formating &, unsigned int);

ResultOr<size_t> format(Writer &, const Formating &, long int);

ResultOr<size_t> format(Writer &, const Formating &, unsigned long int);

ResultOr<size_t> format(Writer &, const Formating &, float);

ResultOr<size_t> format(Writer &, const Formating &, double);

ResultOr<size_t> format(Writer &, const Formating &, const char *);

ResultOr<size_t> format(Writer &, const Formating &, const String);

static inline ResultOr<size_t> format(Writer &writer, Scanner &scanner)
{
    size_t written = 0;

    while (!scanner.ended())
    {
        written += TRY(writer.write(scanner.current()));
        scanner.foreward();
    }

    return written;
}

template <Formatable First, Formatable... Args>
static inline ResultOr<size_t> format(Writer &writer, Scanner &scanner, First first, Args... args)
{
    size_t written = 0;

    while (!(scanner.ended() || scanner.current() == '{'))
    {
        written += TRY(writer.write(scanner.current()));
        scanner.foreward();
    }

    if (scanner.current() == '{')
    {
        auto formating = Formating::parse(scanner);

        auto format_proxy = [&]() {
            if constexpr (IsBaseOf<Format, First>::value)
            {
                return first.format(writer);
            }
            else if constexpr (requires(const First &t) {
                                   format(writer, formating, t);
                               })
            {
                return format(writer, formating, first);
            }
            else
            {
                ASSERT_NOT_REACHED();
            }
        };

        written += TRY(format_proxy());
    }

    if (!scanner.ended())
    {
        written += TRY(format(writer, scanner, forward<Args>(args)...));
    }

    return written;
}

template <Formatable... Args>
static inline ResultOr<size_t> format(Writer &writer, const char *fmt, Args... args)
{
    StringScanner scan{fmt, strlen(fmt)};
    return format(writer, scan, forward<Args>(args)...);
}

static inline ResultOr<size_t> format(Writer &writer, const char *fmt)
{
    return writer.write(fmt);
}

template <Formatable... Args>
static inline String format(const char *fmt, Args... args)
{
    MemoryWriter memory{};
    format(memory, fmt, forward<Args>(args)...);

    return memory.string();
}

} // namespace System
