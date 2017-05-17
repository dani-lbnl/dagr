#include "dagr_variant_array_operand.h"

#include <cstdlib>
#include <cerrno>

namespace dagr_variant_array_operand
{

// --------------------------------------------------------------------------
int resolver::get_constant(const char *s, p_dagr_variant_array &c)
{
    // match the length of the arrays already in the container
    // or fall back to a default length. mismatch will likely
    // cause a segv, the default length is only provided for
    // testing.
    unsigned long n = m_variables ? m_variables->size() ?
        m_variables->get(0)->size() : 1 : 1;

    // determine type and signedness from the last
    // 1 or 2 characters in the string
    unsigned int last = strlen(s) - 1;

    bool unsigned_type = false;
    if (s[last] == 'u')
    {
        if (last < 1) return -1;
        unsigned_type = true;
        --last;
    }

    // use cstdlib to make the conversion, check errno
    errno = 0;
    switch (s[last])
    {
        case 'd':
        {
            double val = strtod(s, nullptr);
            if (errno) return -1;
            c = dagr_double_array::New(n, val);
            return 0;
            break;
        }
        case 'f':
        {
            float val = strtof(s, nullptr);
            if (errno) return -1;
            c = dagr_float_array::New(n, val);
            return 0;
            break;
        }
        case 'L':
            if (unsigned_type)
            {
                unsigned long long val = strtoull(s, nullptr, 0);
                if (errno) return -1;
                c = dagr_unsigned_long_long_array::New(n, val);
            }
            else
            {
                long long val = strtoll(s, nullptr, 0);
                if (errno) return -1;
                c = dagr_long_long_array::New(n, val);
            }
            return 0;
            break;
        case 'l':
            if (unsigned_type)
            {
                unsigned long val = strtoul(s, nullptr, 0);
                if (errno) return -1;
                c = dagr_unsigned_long_array::New(n, val);
            }
            else
            {
                long val = strtol(s, nullptr, 0);
                if (errno) return -1;
                c = dagr_long_array::New(n, val);
            }
            return 0;
            break;
        case 'i':
        {
            if (unsigned_type)
            {
                unsigned int val = strtoul(s, nullptr, 0);
                if (errno) return -1;
                c = dagr_unsigned_int_array::New(n, val);
            }
            else
            {
                int val = strtol(s, nullptr, 0);
                if (errno) return -1;
                c = dagr_int_array::New(n, val);
            }
            return 0;
            break;
        }
        case 's':
        {
            if (unsigned_type)
            {
                unsigned short val = strtoul(s, nullptr, 0);
                if (errno) return -1;
                c = dagr_short_array::New(n, val);
            }
            else
            {
                short val = strtol(s, nullptr, 0);
                if (errno) return -1;
                c = dagr_short_array::New(n, val);
            }
            return 0;
            break;
        }
        case 'c':
        {
            if (unsigned_type)
            {
                unsigned char val = strtoul(s, nullptr, 0);
                if (errno) return -1;
                c = dagr_char_array::New(n, val);
            }
            else
            {
                char val = strtol(s, nullptr, 0);
                if (errno) return -1;
                c = dagr_char_array::New(n, val);
            }
            return 0;
            break;
        }
    }

    // untyped expression, use float
    float val = strtof(s, nullptr);
    if (errno) return -1;
    c = dagr_float_array::New(n, val);
    return 0;
}

};
