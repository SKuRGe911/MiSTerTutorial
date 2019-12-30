// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

#include <stdarg.h>
#include <stdint.h>

#include <string.h>

int isspace(int c)
{
	return ((c == ' ') || (c == '\n') || (c == '\t'));
}

long atol(const char* string)
{
    register long int result = 0;
    register unsigned int digit;
    int sign;

    /*
     * Skip any leading blanks.
     */

    while (isspace(*string)) {
        string += 1;
    }

    /*
     * Check for a sign.
     */

    if (*string == '-') {
        sign = 1;
        string += 1;
    }
    else {
        sign = 0;
        if (*string == '+') {
            string += 1;
        }
    }

    for (; ; string += 1) {
        digit = *string - '0';
        if (digit > 9) {
            break;
        }
        result = (10 * result) + digit;
    }

    if (sign) {
        return -result;
    }
    return result;
}

#ifdef STRNLEN
# define __strnlen STRNLEN
#endif
size_t
strnlen(const char* str, size_t maxlen)
{
    const char* char_ptr, * end_ptr = str + maxlen;
    const unsigned long int* longword_ptr;
    unsigned long int longword, himagic, lomagic;
    if (maxlen == 0)
        return 0;
    /* Handle the first few characters by reading one character at a time.
       Do this until CHAR_PTR is aligned on a longword boundary.  */
    for (char_ptr = str; ((unsigned long int) char_ptr
        & (sizeof(longword) - 1)) != 0;
        ++char_ptr)
        if (*char_ptr == '\0')
        {
            if (char_ptr > end_ptr)
                char_ptr = end_ptr;
            return char_ptr - str;
        }
    /* All these elucidatory comments refer to 4-byte longwords,
       but the theory applies equally well to 8-byte longwords.  */
    longword_ptr = (unsigned long int*) char_ptr;
    /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
       the "holes."  Note that there is a hole just to the left of
       each byte, with an extra at the end:
       bits:  01111110 11111110 11111110 11111111
       bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD
       The 1-bits make sure that carries propagate to the next 0-bit.
       The 0-bits provide holes for carries to fall into.  */
    himagic = 0x80808080L;
    lomagic = 0x01010101L;
    if (sizeof(longword) > 4)
    {
        /* 64-bit version of the magic.  */
        /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
        himagic = ((himagic << 16) << 16) | himagic;
        lomagic = ((lomagic << 16) << 16) | lomagic;
    }
    /* Instead of the traditional loop which tests each character,
       we will test a longword at a time.  The tricky part is testing
       if *any of the four* bytes in the longword in question are zero.  */
    while (longword_ptr < (unsigned long int*) end_ptr)
    {
        /* We tentatively exit the loop if adding MAGIC_BITS to
           LONGWORD fails to change any of the hole bits of LONGWORD.
           1) Is this safe?  Will it catch all the zero bytes?
           Suppose there is a byte with all zeros.  Any carry bits
           propagating from its left will fall into the hole at its
           least significant bit and stop.  Since there will be no
           carry from its most significant bit, the LSB of the
           byte to the left will be unchanged, and the zero will be
           detected.
           2) Is this worthwhile?  Will it ignore everything except
           zero bytes?  Suppose every byte of LONGWORD has a bit set
           somewhere.  There will be a carry into bit 8.  If bit 8
           is set, this will carry into bit 16.  If bit 8 is clear,
           one of bits 9-15 must be set, so there will be a carry
           into bit 16.  Similarly, there will be a carry into bit
           24.  If one of bits 24-30 is set, there will be a carry
           into bit 31, so all of the hole bits will be changed.
           The one misfire occurs when bits 24-30 are clear and bit
           31 is set; in this case, the hole at bit 31 is not
           changed.  If we had access to the processor carry flag,
           we could close this loophole by putting the fourth hole
           at bit 32!
           So it ignores everything except 128's, when they're aligned
           properly.  */
        longword = *longword_ptr++;
        if ((longword - lomagic) & himagic)
        {
            /* Which of the bytes was the zero?  If none of them were, it was
               a misfire; continue the search.  */
            const char* cp = (const char*)(longword_ptr - 1);
            char_ptr = cp;
            if (cp[0] == 0)
                break;
            char_ptr = cp + 1;
            if (cp[1] == 0)
                break;
            char_ptr = cp + 2;
            if (cp[2] == 0)
                break;
            char_ptr = cp + 3;
            if (cp[3] == 0)
                break;
            if (sizeof(longword) > 4)
            {
                char_ptr = cp + 4;
                if (cp[4] == 0)
                    break;
                char_ptr = cp + 5;
                if (cp[5] == 0)
                    break;
                char_ptr = cp + 6;
                if (cp[6] == 0)
                    break;
                char_ptr = cp + 7;
                if (cp[7] == 0)
                    break;
            }
        }
        char_ptr = end_ptr;
    }
    if (char_ptr > end_ptr)
        char_ptr = end_ptr;
    return char_ptr - str;
}

#undef strncpy
#ifndef STRNCPY
#define STRNCPY strncpy
#endif

char*
STRNCPY(char* s1, const char* s2, size_t n)
{
	size_t size = strnlen(s2, n);
	if (size != n)
		memset(s1 + size, '\0', n - size);
	return memcpy(s1, s2, size);
}

#undef strncmp
#ifndef STRNCMP
#define STRNCMP strncmp
#endif

int
STRNCMP(const char* s1, const char* s2, size_t n)
{
    unsigned char c1 = '\0';
    unsigned char c2 = '\0';
    if (n >= 4)
    {
        size_t n4 = n >> 2;
        do
        {
            c1 = (unsigned char)*s1++;
            c2 = (unsigned char)*s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
            c1 = (unsigned char)*s1++;
            c2 = (unsigned char)*s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
            c1 = (unsigned char)*s1++;
            c2 = (unsigned char)*s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
            c1 = (unsigned char)*s1++;
            c2 = (unsigned char)*s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
        } while (--n4 > 0);
        n &= 3;
    }
    while (n > 0)
    {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0' || c1 != c2)
            return c1 - c2;
        n--;
    }
    return c1 - c2;
}

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>

#ifdef isdigit
#undef isdigit
#endif
#define isdigit(c) ((c) >= '0' && (c) <= '9')

 /* some other values of possible interest: */
 /* #define breakeven_point  8 */  /* VAX 4000          - vaxc */
 /* #define breakeven_point 19 */  /* VAX 4000          - gcc 2.7.0 */

#ifndef breakeven_point
#  define breakeven_point   6	/* some reasonable one-size-fits-all value */
#endif

#define fast_memcpy(d,s,n) \
  { register size_t nn = (size_t)(n); \
    if (nn >= breakeven_point) memcpy((d), (s), nn); \
    else if (nn > 0) { /* proc call overhead is worth only for large strings*/\
      register char *dd; register const char *ss; \
      for (ss=(s), dd=(d); nn>0; nn--) *dd++ = *ss++; } }

#define fast_memset(d,c,n) \
  { register size_t nn = (size_t)(n); \
    if (nn >= breakeven_point) memset((d), (int)(c), nn); \
    else if (nn > 0) { /* proc call overhead is worth only for large strings*/\
      register char *dd; register const int cc=(int)(c); \
      for (dd=(d); nn>0; nn--) *dd++ = cc; } }

/* prototypes */

#if defined(NEED_ASPRINTF)
int asprintf(char** ptr, const char* fmt, /*args*/ ...);
#endif
#if defined(NEED_VASPRINTF)
int vasprintf(char** ptr, const char* fmt, va_list ap);
#endif
#if defined(NEED_ASNPRINTF)
int asnprintf(char** ptr, size_t str_m, const char* fmt, /*args*/ ...);
#endif
#if defined(NEED_VASNPRINTF)
int vasnprintf(char** ptr, size_t str_m, const char* fmt, va_list ap);
#endif

#if defined(HAVE_SNPRINTF)
/* declare our portable snprintf  routine under name portable_snprintf  */
/* declare our portable vsnprintf routine under name portable_vsnprintf */
#else
/* declare our portable routines under names snprintf and vsnprintf */
#define portable_snprintf snprintf
#if !defined(NEED_SNPRINTF_ONLY)
#define portable_vsnprintf vsnprintf
#endif
#endif

#if !defined(HAVE_SNPRINTF) || defined(PREFER_PORTABLE_SNPRINTF)
int portable_snprintf(char* str, size_t str_m, const char* fmt, /*args*/ ...);
#if !defined(NEED_SNPRINTF_ONLY)
int portable_vsnprintf(char* str, size_t str_m, const char* fmt, va_list ap);
#endif
#endif

/* declarations */

static char credits[] = "\n\
@(#)snprintf.c, v2.2: Mark Martinec, <mark.martinec@ijs.si>\n\
@(#)snprintf.c, v2.2: Copyright 1999, Mark Martinec. Frontier Artistic License applies.\n\
@(#)snprintf.c, v2.2: http://www.ijs.si/software/snprintf/\n";

#if defined(NEED_ASPRINTF)
int asprintf(char** ptr, const char* fmt, /*args*/ ...) {
    va_list ap;
    size_t str_m;
    int str_l;

    *ptr = NULL;
    va_start(ap, fmt);                            /* measure the required size */
    str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap);
    va_end(ap);
    assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
    *ptr = (char*)malloc(str_m = (size_t)str_l + 1);
    if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
    else {
        int str_l2;
        va_start(ap, fmt);
        str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
        va_end(ap);
        assert(str_l2 == str_l);
    }
    return str_l;
}
#endif

#if defined(NEED_VASPRINTF)
int vasprintf(char** ptr, const char* fmt, va_list ap) {
    size_t str_m;
    int str_l;

    *ptr = NULL;
    { va_list ap2;
    va_copy(ap2, ap);  /* don't consume the original ap, we'll need it again */
    str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap2);/*get required size*/
    va_end(ap2);
    }
    assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
    *ptr = (char*)malloc(str_m = (size_t)str_l + 1);
    if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
    else {
        int str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
        assert(str_l2 == str_l);
    }
    return str_l;
}
#endif

#if defined(NEED_ASNPRINTF)
int asnprintf(char** ptr, size_t str_m, const char* fmt, /*args*/ ...) {
    va_list ap;
    int str_l;

    *ptr = NULL;
    va_start(ap, fmt);                            /* measure the required size */
    str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap);
    va_end(ap);
    assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
    if ((size_t)str_l + 1 < str_m) str_m = (size_t)str_l + 1;      /* truncate */
    /* if str_m is 0, no buffer is allocated, just set *ptr to NULL */
    if (str_m == 0) {  /* not interested in resulting string, just return size */
    }
    else {
        *ptr = (char*)malloc(str_m);
        if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
        else {
            int str_l2;
            va_start(ap, fmt);
            str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
            va_end(ap);
            assert(str_l2 == str_l);
        }
    }
    return str_l;
}
#endif

#if defined(NEED_VASNPRINTF)
int vasnprintf(char** ptr, size_t str_m, const char* fmt, va_list ap) {
    int str_l;

    *ptr = NULL;
    { va_list ap2;
    va_copy(ap2, ap);  /* don't consume the original ap, we'll need it again */
    str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap2);/*get required size*/
    va_end(ap2);
    }
    assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
    if ((size_t)str_l + 1 < str_m) str_m = (size_t)str_l + 1;      /* truncate */
    /* if str_m is 0, no buffer is allocated, just set *ptr to NULL */
    if (str_m == 0) {  /* not interested in resulting string, just return size */
    }
    else {
        *ptr = (char*)malloc(str_m);
        if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
        else {
            int str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
            assert(str_l2 == str_l);
        }
    }
    return str_l;
}
#endif

/*
 * If the system does have snprintf and the portable routine is not
 * specifically required, this module produces no code for snprintf/vsnprintf.
 */
#if !defined(HAVE_SNPRINTF) || defined(PREFER_PORTABLE_SNPRINTF)

#if !defined(NEED_SNPRINTF_ONLY)
int portable_snprintf(char* str, size_t str_m, const char* fmt, /*args*/ ...) {
    va_list ap;
    int str_l;

    va_start(ap, fmt);
    str_l = portable_vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    return str_l;
}
#endif

#if defined(NEED_SNPRINTF_ONLY)
int portable_snprintf(char* str, size_t str_m, const char* fmt, /*args*/ ...) {
#else
int portable_vsnprintf(char* str, size_t str_m, const char* fmt, va_list ap) {
#endif

#if defined(NEED_SNPRINTF_ONLY)
    va_list ap;
#endif
    size_t str_l = 0;
    const char* p = fmt;

    /* In contrast with POSIX, the ISO C99 now says
     * that str can be NULL and str_m can be 0.
     * This is more useful than the old:  if (str_m < 1) return -1; */

#if defined(NEED_SNPRINTF_ONLY)
    va_start(ap, fmt);
#endif
    if (!p) p = "";
    while (*p) {
        if (*p != '%') {
            /* if (str_l < str_m) str[str_l++] = *p++;    -- this would be sufficient */
            /* but the following code achieves better performance for cases
             * where format string is long and contains few conversions */
            const char* q = strchr(p + 1, '%');
            size_t n = !q ? strlen(p) : (q - p);
            if (str_l < str_m) {
                size_t avail = str_m - str_l;
                fast_memcpy(str + str_l, p, (n > avail ? avail : n));
            }
            p += n; str_l += n;
        }
        else {
            const char* starting_p;
            size_t min_field_width = 0, precision = 0;
            int zero_padding = 0, precision_specified = 0, justify_left = 0;
            int alternate_form = 0, force_sign = 0;
            int space_for_positive = 1; /* If both the ' ' and '+' flags appear,
                                           the ' ' flag should be ignored. */
            char length_modifier = '\0';            /* allowed values: \0, h, l, L */
            char tmp[32];/* temporary buffer for simple numeric->string conversion */

            const char* str_arg;      /* string address in case of string argument */
            size_t str_arg_l;         /* natural field width of arg without padding
                                         and sign */
            unsigned char uchar_arg;
            /* unsigned char argument value - only defined for c conversion.
               N.B. standard explicitly states the char argument for
               the c conversion is unsigned */

            size_t number_of_zeros_to_pad = 0;
            /* number of zeros to be inserted for numeric conversions
               as required by the precision or minimal field width */

            size_t zero_padding_insertion_ind = 0;
            /* index into tmp where zero padding is to be inserted */

            char fmt_spec = '\0';
            /* current conversion specifier character */

            str_arg = credits;/* just to make compiler happy (defined but not used)*/
            str_arg = NULL;
            starting_p = p; p++;  /* skip '%' */
         /* parse flags */
            while (*p == '0' || *p == '-' || *p == '+' ||
                *p == ' ' || *p == '#' || *p == '\'') {
                switch (*p) {
                case '0': zero_padding = 1; break;
                case '-': justify_left = 1; break;
                case '+': force_sign = 1; space_for_positive = 0; break;
                case ' ': force_sign = 1;
                    /* If both the ' ' and '+' flags appear, the ' ' flag should be ignored */
#ifdef PERL_COMPATIBLE
     /* ... but in Perl the last of ' ' and '+' applies */
                    space_for_positive = 1;
#endif
                    break;
                case '#': alternate_form = 1; break;
                case '\'': break;
                }
                p++;
            }
            /* If the '0' and '-' flags both appear, the '0' flag should be ignored. */

            /* parse field width */
            if (*p == '*') {
                int j;
                p++; j = va_arg(ap, int);
                if (j >= 0) min_field_width = j;
                else { min_field_width = -j; justify_left = 1; }
            }
            else if (isdigit((int)(*p))) {
                /* size_t could be wider than unsigned int;
                   make sure we treat argument like common implementations do */
                unsigned int uj = *p++ - '0';
                while (isdigit((int)(*p))) uj = 10 * uj + (unsigned int)(*p++ - '0');
                min_field_width = uj;
            }
            /* parse precision */
            if (*p == '.') {
                p++; precision_specified = 1;
                if (*p == '*') {
                    int j = va_arg(ap, int);
                    p++;
                    if (j >= 0) precision = j;
                    else {
                        precision_specified = 0; precision = 0;
                        /* NOTE:
                         *   Solaris 2.6 man page claims that in this case the precision
                         *   should be set to 0.  Digital Unix 4.0, HPUX 10 and BSD man page
                         *   claim that this case should be treated as unspecified precision,
                         *   which is what we do here.
                         */
                    }
                }
                else if (isdigit((int)(*p))) {
                    /* size_t could be wider than unsigned int;
                       make sure we treat argument like common implementations do */
                    unsigned int uj = *p++ - '0';
                    while (isdigit((int)(*p))) uj = 10 * uj + (unsigned int)(*p++ - '0');
                    precision = uj;
                }
            }
            /* parse 'h', 'l' and 'll' length modifiers */
            if (*p == 'h' || *p == 'l') {
                length_modifier = *p; p++;
                if (length_modifier == 'l' && *p == 'l') {   /* double l = long long */
#ifdef SNPRINTF_LONGLONG_SUPPORT
                    length_modifier = '2';                  /* double l encoded as '2' */
#else
                    length_modifier = 'l';                 /* treat it as a single 'l' */
#endif
                    p++;
                }
            }
            fmt_spec = *p;
            /* common synonyms: */
            switch (fmt_spec) {
            case 'i': fmt_spec = 'd'; break;
            case 'D': fmt_spec = 'd'; length_modifier = 'l'; break;
            case 'U': fmt_spec = 'u'; length_modifier = 'l'; break;
            case 'O': fmt_spec = 'o'; length_modifier = 'l'; break;
            default: break;
            }
            /* get parameter value, do initial processing */
            switch (fmt_spec) {
            case '%': /* % behaves similar to 's' regarding flags and field widths */
            case 'c': /* c behaves similar to 's' regarding flags and field widths */
            case 's':
                length_modifier = '\0';          /* wint_t and wchar_t not supported */
             /* the result of zero padding flag with non-numeric conversion specifier*/
             /* is undefined. Solaris and HPUX 10 does zero padding in this case,    */
             /* Digital Unix and Linux does not. */
#if !defined(SOLARIS_COMPATIBLE) && !defined(HPUX_COMPATIBLE)
                zero_padding = 0;    /* turn zero padding off for string conversions */
#endif
                str_arg_l = 1;
                switch (fmt_spec) {
                case '%':
                    str_arg = p; break;
                case 'c': {
                    int j = va_arg(ap, int);
                    uchar_arg = (unsigned char)j;   /* standard demands unsigned char */
                    str_arg = (const char*)&uchar_arg;
                    break;
                }
                case 's':
                    str_arg = va_arg(ap, const char*);
                    if (!str_arg) str_arg_l = 0;
                    /* make sure not to address string beyond the specified precision !!! */
                    else if (!precision_specified) str_arg_l = strlen(str_arg);
                    /* truncate string if necessary as requested by precision */
                    else if (precision == 0) str_arg_l = 0;
                    else {
                        /* memchr on HP does not like n > 2^31  !!! */
                        const char* q = memchr(str_arg, '\0',
                            precision <= 0x7fffffff ? precision : 0x7fffffff);
                        str_arg_l = !q ? precision : (q - str_arg);
                    }
                    break;
                default: break;
                }
                break;
            case 'd': case 'u': case 'o': case 'x': case 'X': case 'p': {
                /* NOTE: the u, o, x, X and p conversion specifiers imply
                         the value is unsigned;  d implies a signed value */

                int arg_sign = 0;
                /* 0 if numeric argument is zero (or if pointer is NULL for 'p'),
                  +1 if greater than zero (or nonzero for unsigned arguments),
                  -1 if negative (unsigned argument is never negative) */

                int int_arg = 0;  unsigned int uint_arg = 0;
                /* only defined for length modifier h, or for no length modifiers */

                long int long_arg = 0;  unsigned long int ulong_arg = 0;
                /* only defined for length modifier l */

                void* ptr_arg = NULL;
                /* pointer argument value -only defined for p conversion */

#ifdef SNPRINTF_LONGLONG_SUPPORT
                long long int long_long_arg = 0;
                unsigned long long int ulong_long_arg = 0;
                /* only defined for length modifier ll */
#endif
                if (fmt_spec == 'p') {
                    /* HPUX 10: An l, h, ll or L before any other conversion character
                     *   (other than d, i, u, o, x, or X) is ignored.
                     * Digital Unix:
                     *   not specified, but seems to behave as HPUX does.
                     * Solaris: If an h, l, or L appears before any other conversion
                     *   specifier (other than d, i, u, o, x, or X), the behavior
                     *   is undefined. (Actually %hp converts only 16-bits of address
                     *   and %llp treats address as 64-bit data which is incompatible
                     *   with (void *) argument on a 32-bit system).
                     */
#ifdef SOLARIS_COMPATIBLE
#  ifdef SOLARIS_BUG_COMPATIBLE
                     /* keep length modifiers even if it represents 'll' */
#  else
                    if (length_modifier == '2') length_modifier = '\0';
#  endif
#else
                    length_modifier = '\0';
#endif
                    ptr_arg = va_arg(ap, void*);
                    if (ptr_arg != NULL) arg_sign = 1;
                }
                else if (fmt_spec == 'd') {  /* signed */
                    switch (length_modifier) {
                    case '\0':
                    case 'h':
                        /* It is non-portable to specify a second argument of char or short
                         * to va_arg, because arguments seen by the called function
                         * are not char or short.  C converts char and short arguments
                         * to int before passing them to a function.
                         */
                        int_arg = va_arg(ap, int);
                        if (int_arg > 0) arg_sign = 1;
                        else if (int_arg < 0) arg_sign = -1;
                        break;
                    case 'l':
                        long_arg = va_arg(ap, long int);
                        if (long_arg > 0) arg_sign = 1;
                        else if (long_arg < 0) arg_sign = -1;
                        break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
                    case '2':
                        long_long_arg = va_arg(ap, long long int);
                        if (long_long_arg > 0) arg_sign = 1;
                        else if (long_long_arg < 0) arg_sign = -1;
                        break;
#endif
                    }
                }
                else {  /* unsigned */
                    switch (length_modifier) {
                    case '\0':
                    case 'h':
                        uint_arg = va_arg(ap, unsigned int);
                        if (uint_arg) arg_sign = 1;
                        break;
                    case 'l':
                        ulong_arg = va_arg(ap, unsigned long int);
                        if (ulong_arg) arg_sign = 1;
                        break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
                    case '2':
                        ulong_long_arg = va_arg(ap, unsigned long long int);
                        if (ulong_long_arg) arg_sign = 1;
                        break;
#endif
                    }
                }
                str_arg = tmp; str_arg_l = 0;
                /* NOTE:
                 *   For d, i, u, o, x, and X conversions, if precision is specified,
                 *   the '0' flag should be ignored. This is so with Solaris 2.6,
                 *   Digital UNIX 4.0, HPUX 10, Linux, FreeBSD, NetBSD; but not with Perl.
                 */
#ifndef PERL_COMPATIBLE
                if (precision_specified) zero_padding = 0;
#endif
                if (fmt_spec == 'd') {
                    if (force_sign && arg_sign >= 0)
                        tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
                    /* leave negative numbers for sprintf to handle,
                       to avoid handling tricky cases like (short int)(-32768) */
#ifdef LINUX_COMPATIBLE
                }
                else if (fmt_spec == 'p' && force_sign && arg_sign > 0) {
                    tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
#endif
                }
                else if (alternate_form) {
                    if (arg_sign != 0 && (fmt_spec == 'x' || fmt_spec == 'X'))
                    {
                        tmp[str_arg_l++] = '0'; tmp[str_arg_l++] = fmt_spec;
                    }
                    /* alternate form should have no effect for p conversion, but ... */
#ifdef HPUX_COMPATIBLE
                    else if (fmt_spec == 'p'
                        /* HPUX 10: for an alternate form of p conversion,
                         *          a nonzero result is prefixed by 0x. */
#ifndef HPUX_BUG_COMPATIBLE
                         /* Actually it uses 0x prefix even for a zero value. */
                        && arg_sign != 0
#endif
                        ) {
                        tmp[str_arg_l++] = '0'; tmp[str_arg_l++] = 'x';
                    }
#endif
                }
                zero_padding_insertion_ind = str_arg_l;
                if (!precision_specified) precision = 1;   /* default precision is 1 */
                if (precision == 0 && arg_sign == 0
#if defined(HPUX_BUG_COMPATIBLE) || defined(LINUX_COMPATIBLE)
                    && fmt_spec != 'p'
                    /* HPUX 10 man page claims: With conversion character p the result of
                     * converting a zero value with a precision of zero is a null string.
                     * Actually HP returns all zeroes, and Linux returns "(nil)". */
#endif
                    ) {
                    /* converted to null string */
                    /* When zero value is formatted with an explicit precision 0,
                       the resulting formatted string is empty (d, i, u, o, x, X, p).   */
                }
                else {
                    char f[5]; int f_l = 0;
                    f[f_l++] = '%';    /* construct a simple format string for sprintf */
                    if (!length_modifier) {}
                    else if (length_modifier == '2') { f[f_l++] = 'l'; f[f_l++] = 'l'; }
                    else f[f_l++] = length_modifier;
                    f[f_l++] = fmt_spec; f[f_l++] = '\0';
                    if (fmt_spec == 'p') str_arg_l += sprintf(tmp + str_arg_l, f, ptr_arg);
                    else if (fmt_spec == 'd') {  /* signed */
                        switch (length_modifier) {
                        case '\0':
                        case 'h': str_arg_l += sprintf(tmp + str_arg_l, f, int_arg);  break;
                        case 'l': str_arg_l += sprintf(tmp + str_arg_l, f, long_arg); break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
                        case '2': str_arg_l += sprintf(tmp + str_arg_l, f, long_long_arg); break;
#endif
                        }
                    }
                    else {  /* unsigned */
                        switch (length_modifier) {
                        case '\0':
                        case 'h': str_arg_l += sprintf(tmp + str_arg_l, f, uint_arg);  break;
                        case 'l': str_arg_l += sprintf(tmp + str_arg_l, f, ulong_arg); break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
                        case '2': str_arg_l += sprintf(tmp + str_arg_l, f, ulong_long_arg); break;
#endif
                        }
                    }
                    /* include the optional minus sign and possible "0x"
                       in the region before the zero padding insertion point */
                    if (zero_padding_insertion_ind < str_arg_l &&
                        tmp[zero_padding_insertion_ind] == '-') {
                        zero_padding_insertion_ind++;
                    }
                    if (zero_padding_insertion_ind + 1 < str_arg_l &&
                        tmp[zero_padding_insertion_ind] == '0' &&
                        (tmp[zero_padding_insertion_ind + 1] == 'x' ||
                            tmp[zero_padding_insertion_ind + 1] == 'X')) {
                        zero_padding_insertion_ind += 2;
                    }
                }
                { size_t num_of_digits = str_arg_l - zero_padding_insertion_ind;
                if (alternate_form && fmt_spec == 'o'
#ifdef HPUX_COMPATIBLE                                  /* ("%#.o",0) -> ""  */
                    && (str_arg_l > 0)
#endif
#ifdef DIGITAL_UNIX_BUG_COMPATIBLE                      /* ("%#o",0) -> "00" */
#else
                    /* unless zero is already the first character */
                    && !(zero_padding_insertion_ind < str_arg_l
                        && tmp[zero_padding_insertion_ind] == '0')
#endif
                    ) {        /* assure leading zero for alternate-form octal numbers */
                    if (!precision_specified || precision < num_of_digits + 1) {
                        /* precision is increased to force the first character to be zero,
                           except if a zero value is formatted with an explicit precision
                           of zero */
                        precision = num_of_digits + 1; precision_specified = 1;
                    }
                }
                /* zero padding to specified precision? */
                if (num_of_digits < precision)
                    number_of_zeros_to_pad = precision - num_of_digits;
                }
                /* zero padding to specified minimal field width? */
                if (!justify_left && zero_padding) {
                    int n = min_field_width - (str_arg_l + number_of_zeros_to_pad);
                    if (n > 0) number_of_zeros_to_pad += n;
                }
                break;
            }
            default: /* unrecognized conversion specifier, keep format string as-is*/
                zero_padding = 0;  /* turn zero padding off for non-numeric convers. */
#ifndef DIGITAL_UNIX_COMPATIBLE
                justify_left = 1; min_field_width = 0;                /* reset flags */
#endif
#if defined(PERL_COMPATIBLE) || defined(LINUX_COMPATIBLE)
     /* keep the entire format string unchanged */
                str_arg = starting_p; str_arg_l = p - starting_p;
                /* well, not exactly so for Linux, which does something inbetween,
                 * and I don't feel an urge to imitate it: "%+++++hy" -> "%+y"  */
#else
     /* discard the unrecognized conversion, just keep *
      * the unrecognized conversion character          */
                str_arg = p; str_arg_l = 0;
#endif
                if (*p) str_arg_l++;  /* include invalid conversion specifier unchanged
                                         if not at end-of-string */
                break;
            }
            if (*p) p++;      /* step over the just processed conversion specifier */
         /* insert padding to the left as requested by min_field_width;
            this does not include the zero padding in case of numerical conversions*/
            if (!justify_left) {                /* left padding with blank or zero */
                int n = min_field_width - (str_arg_l + number_of_zeros_to_pad);
                if (n > 0) {
                    if (str_l < str_m) {
                        size_t avail = str_m - str_l;
                        fast_memset(str + str_l, (zero_padding ? '0' : ' '), (n > avail ? avail : n));
                    }
                    str_l += n;
                }
            }
            /* zero padding as requested by the precision or by the minimal field width
             * for numeric conversions required? */
            if (number_of_zeros_to_pad <= 0) {
                /* will not copy first part of numeric right now, *
                 * force it to be copied later in its entirety    */
                zero_padding_insertion_ind = 0;
            }
            else {
                /* insert first part of numerics (sign or '0x') before zero padding */
                int n = zero_padding_insertion_ind;
                if (n > 0) {
                    if (str_l < str_m) {
                        size_t avail = str_m - str_l;
                        fast_memcpy(str + str_l, str_arg, (n > avail ? avail : n));
                    }
                    str_l += n;
                }
                /* insert zero padding as requested by the precision or min field width */
                n = number_of_zeros_to_pad;
                if (n > 0) {
                    if (str_l < str_m) {
                        size_t avail = str_m - str_l;
                        fast_memset(str + str_l, '0', (n > avail ? avail : n));
                    }
                    str_l += n;
                }
            }
            /* insert formatted string
             * (or as-is conversion specifier for unknown conversions) */
            { int n = str_arg_l - zero_padding_insertion_ind;
            if (n > 0) {
                if (str_l < str_m) {
                    size_t avail = str_m - str_l;
                    fast_memcpy(str + str_l, str_arg + zero_padding_insertion_ind,
                        (n > avail ? avail : n));
                }
                str_l += n;
            }
            }
            /* insert right padding */
            if (justify_left) {          /* right blank padding to the field width */
                int n = min_field_width - (str_arg_l + number_of_zeros_to_pad);
                if (n > 0) {
                    if (str_l < str_m) {
                        size_t avail = str_m - str_l;
                        fast_memset(str + str_l, ' ', (n > avail ? avail : n));
                    }
                    str_l += n;
                }
            }
        }
    }
#if defined(NEED_SNPRINTF_ONLY)
    va_end(ap);
#endif
    if (str_m > 0) { /* make sure the string is null-terminated
                        even at the expense of overwriting the last character
                        (shouldn't happen, but just in case) */
        str[str_l <= str_m - 1 ? str_l : str_m - 1] = '\0';
    }
    /* Return the number of characters formatted (excluding trailing null
     * character), that is, the number of characters that would have been
     * written to the buffer if it were large enough.
     *
     * The value of str_l should be returned, but str_l is of unsigned type
     * size_t, and snprintf is int, possibly leading to an undetected
     * integer overflow, resulting in a negative return value, which is illegal.
     * Both XSH5 and ISO C99 (at least the draft) are silent on this issue.
     * Should errno be set to EOVERFLOW and EOF returned in this case???
     */
    return (int)str_l;
}
#endif
