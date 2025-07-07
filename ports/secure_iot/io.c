/***************************************************************************
 * Project                      :  Mindgrove Silicon's Secure-IoT SoC
 * Name of the file             :  io.c
 * Brief Description of file    :  Standard Library Functions
 * Name of Author               :  Kapil Shyam. M
 * Email ID                     :  kapil@mindgrovetech.in

 Copyright (C) 2024  Mindgrove Technologies. All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

***************************************************************************/
/**
@file io.c
@brief Standard Library functions
@details This file hosts a list of routines that helps in displaying variable's
value based on there data type.
*/

#include "io.h"
#include "errors.h"
#include "string_functions.h"

uint32_t _errno = 0;

static inline void itoa (uint64_t number, unsigned base,long int total_number_of_digits, char no_points);
void _print_(const char *fmt, va_list ap);
int printf(const char *fmt, ...);
static inline int linearSearch(char arr[], int n, char target);


// unsigned char *__sccl();

/**
 * vsscanf - Unformat a buffer into a list of arguments
 * @buf:	input buffer
 * @fmt:	format of buffer
 * @args:	arguments
 */
#define BUF             32      /* Maximum length of numeric string. */

/*
 * Flags used during conversion.
 */
#define LONG            0x01    /* l: long or double */
#define SHORT           0x04    /* h: short */
#define SUPPRESS        0x08    /* suppress assignment */
#define POINTER         0x10    /* weird %p pointer (`fake hex') */
#define NOSKIP          0x20    /* do not skip blanks */
#define QUAD            0x400
#define SHORTSHORT      0x4000  /** hh: char */

/*
 * The following are used in numeric conversions only:
 * SIGNOK, NDIGITS, DPTOK, and EXPOK are for floating point;
 * SIGNOK, NDIGITS, PFXOK, and NZDIGITS are for integral.
 */
#define SIGNOK          0x40    /* +/- is (still) legal */
#define NDIGITS         0x80    /* no digits detected */

#define DPTOK           0x100   /* (float) decimal point is still legal */
#define EXPOK           0x200   /* (float) exponent (e+3, etc) still legal */

#define PFXOK           0x100   /* 0x prefix is (still) legal */
#define NZDIGITS        0x200   /* no zero digits detected */

/*
 * Conversion types.
 */
#define CT_CHAR         0       /* %c conversion */
#define CT_CCL          1       /* %[...] conversion */
#define CT_STRING       2       /* %s conversion */
#define CT_INT          3       /* integer, i.e., Str2QInt or Str2UQInt */
typedef unsigned int (*ccfntype)(const char *, char **, int);

/**
 * __sccl() - Fill in the given table from the scanset at the given format
 * (just after `[')
 * @tab: table to fill in
 * @fmt: format of buffer
 *
 * The table has a 1 wherever characters should be considered part of the
 * scanset.
 *
 * Return: pointer to the character past the closing `]'
 */
static inline const unsigned char * __sccl(char *tab, const unsigned char *fmt)
{
	int c, n, v;
	/* first `clear' the whole table */
	c = *fmt++;             /* first char hat => negated scanset */
	if (c == '^') {
		v = 1;          /* default => accept */
		c = *fmt++;     /* get new first char */
	} else {
		v = 0;          /* default => reject */
	}
	/* XXX: Will not work if sizeof(tab*) > sizeof(char) */
	for (n = 0; n < 256; n++)
		tab[n] = v;        /* memset(tab, v, 256) */
	if (c == 0)
		return (fmt - 1);/* format ended before closing ] */
	/*
	 * Now set the entries corresponding to the actual scanset
	 * to the opposite of the above.
	 *
	 * The first character may be ']' (or '-') without being special;
	 * the last character may be '-'.
	 */
	v = 1 - v;
	for (;;) {
		tab[c] = v;             /* take character c */
doswitch:
		n = *fmt++;             /* and examine the next */
		switch (n) {
		case 0:                 /* format ended too soon */
			return (fmt - 1);
		case '-':
			/*
			 * A scanset of the form
			 *      [01+-]
			 * is defined as `the digit 0, the digit 1,
			 * the character +, the character -', but
			 * the effect of a scanset such as
			 *      [a-zA-Z0-9]
			 * is implementation defined.  The V7 Unix
			 * scanf treats `a-z' as `the letters a through
			 * z', but treats `a-a' as `the letter a, the
			 * character -, and the letter a'.
			 *
			 * For compatibility, the `-' is not considerd
			 * to define a range if the character following
			 * it is either a close bracket (required by ANSI)
			 * or is not numerically greater than the character
			 * we just stored in the table (c).
			 */
			n = *fmt;
			if (n == ']' || n < c) {
				c = '-';
				break;  /* resume the for(;;) */
			}
			fmt++;
			/* fill in the range */
			do {
				tab[++c] = v;
			} while (c < n);
			c = n;
			/*
			 * Alas, the V7 Unix scanf also treats formats
			 * such as [a-c-e] as `the letters a through e'.
			 * This too is permitted by the standard....
			 */
			goto doswitch;
			break;
		case ']':               /* end of scanset */
			return (fmt);
		default:                /* just another character */
			c = n;
			break;
		}
	}
	/* NOTREACHED */
}

/**
 * @brief * @fn int _scan_(const char* format, va_list)
 * @brief Reads formatted input from the console
 * @param format A string that specifies the expected format of the input
 * @param ... Additional arguments, as required by the format string
 * @return The number of successfully read and stored items
 */

static inline int _scan_(const char * format, va_list vl)
{
    int i = 0, j = 0, ret = 0;
    char c;
    char *out_loc;
    i = 0;

    while (format && format[i])
    {
        if (format[i] == '%') 
        {
            i++;
            switch (format[i]) 
            {

                case 's': 
                {
                    c = getchar();
                    char* buff = va_arg(vl, char*);
                    int k = 0;
                    while (c != '\r')
                    {
                        if(c == '\b'){
                            if(k > 0){
                                k--;
                                putchar('\b');
                                putchar(' ');
                                putchar('\b');
                                putchar(' ');
                                buff[k] = 0;
                            }
                        }
                        else
                        {
                          buff[k++] = c;
                        }
                        log_trace("%c", c);
                        c = getchar();
                    }
                    buff[k]='\0';
                    ret++;
                    break;
                }
                case 'c': 
                {   
                    int k=0;
                    c = getchar();
                    log_trace("%c", c);
                    if(c == '\b'){
                            if(k > 0){
                                k--;
                                putchar('\b');
                                putchar(' ');
                                putchar('\b');
                                putchar(' ');
                            }
                        }
                    *(char *)va_arg(vl, char*) = c;
                    ret++;
                    break;
                }
                case 'd': 
                {
                    int* d = va_arg(vl, int*);
                    c = getchar();
                    char buff[100] = {0};
                    int k = 0;
                    int result=0;
                    int digit;
                    while ((c != '\r') && (c != ' '))
                    {
                        if(c == '\b'){
                            if(k > 0){
                                k--;
                                putchar('\b');
                                putchar(' ');
                                putchar('\b');
                                putchar(' ');
                                buff[k] = 0;
                            }
                        }
                        else
                        {
                          buff[k++] = c;
                        }
                        log_trace("%c", c);
                        // buff[k] = c;
                        c = getchar();
                    }
                    buff[k]='\0';
                    for(int i=0;i<k; i++)
                    {
                        if (buff[i] >= '0' && buff[i] <= '9')
                        {
                            digit = buff[i] - '0';
                            result = result*10 + digit;
                        }
                        else
                        {
                            break;
                        }
                    }
                    
                    *d = result;
                    if((_errno == ERANGE && (*d == LONG_MAX || *d == LONG_MIN)) || (_errno != 0 && *d == 0))
                    {
                        log_error("strtol");
                        return EOF;
                    }
                    if(out_loc == buff)
                    {
                        log_error("Not a Number. No Digits Found!\n");
                        return EOF;
                    }
                    ret++;
                    break;
                 }
                 case 'f':
            		 {
                    float* f = (float *) va_arg(vl, double*);
                    _errno = 0;
                    int decimal_point = -1;
                    c = getchar();
                    char buff[100] = {0};
                    int k = 0;
                    while ((c != '\r') && (c != ' '))
                    {
                        if (c == '\b') {
                            if (k > 0) {
                                if (decimal_point != -1 && k > decimal_point) {
                                    decimal_point--;
                                } else if (buff[k - 1] == '.') {
                                    decimal_point = -1;
                                }
                                k--;
                                putchar('\b');
                                putchar(' ');
                                putchar('\b');
                                putchar(' ');
                                buff[k] = 0;
                            }
                        }
                        else {
                            if (c == '.') {
                                if (decimal_point == -1) {
                                    decimal_point = k;
                                } else {
                                    log_error("Invalid input: multiple decimal points\n");
                                    return EOF;
                                }
                            }
                            buff[k++] = c;
                        }
                        log_trace("%c", c, k);
                        // buff[k] = c;
                        c = getchar();
                    }
                    buff[k]='\0';
                    *f = Str2Float(buff, &out_loc);
                    if((_errno == ERANGE && (*f == HUGE_VALF || *f == -HUGE_VALF)) || (_errno != 0 && *f == 0))
                    {
                        log_error("strtof");
                        return EOF;
                    }
                    if(out_loc == buff)
                    {
                        log_error("Not a Number. No Digits Found!\n");
                        return EOF;
                    }
                    ret++;
                    break;
                }
                 case 'x':
                 {
                    int* x = va_arg(vl, int*);
                    c = getchar();
                    char buff[100] = {0};
                    int k = 0;
                    while ((c != '\r') && (c != ' '))
                    {
                        if(c == '\b'){
                            if(k > 0){
                                k--;
                                putchar('\b');
                                putchar(' ');
                                putchar('\b');
                                putchar(' ');
                                buff[k] = 0;
                            }
                        }
                        else
                        {
                          buff[k++] = c;
                        }
                        log_trace("%c", c);
                        // buff[k] = c;
                        c = getchar();
                    }
                    buff[k]='\0';
                    *x = Str2Long(buff, &out_loc, 16);
                    if((_errno == ERANGE && (*x == LONG_MAX || *x == LONG_MIN)) || (_errno != 0 && *x == 0))
                    {
                        log_error("strtol");
                        return EOF;
                    }
                    if(out_loc == buff)
                    {
                        log_error("Not a Number. No Digits Found!\n");
                        return EOF;
                    }
                    ret++;
                    break;
                 }
                  case 'o':
                  {
                      int* o = va_arg(vl, int*);
                      c = getchar();
                      char buff[100] = {0};
                      int k = 0;
                      while ((c != '\r') && (c != ' '))
                      {
                        if(c == '\b'){
                            if(k > 0){
                                k--;
                                putchar('\b');
                                putchar(' ');
                                putchar('\b');
                                putchar(' ');
                                buff[k] = 0;
                            }
                        }
                        else
                        {
                          buff[k++] = c;
                        }
                        log_trace("%c", c);
                        // buff[k] = c;
                        c = getchar();
                      }
                        buff[k]='\0';
                      *o = Str2Long(buff, &out_loc, 8);
                      if((_errno == ERANGE && (*o == LONG_MAX || *o == LONG_MIN)) || (_errno != 0 && *o == 0))
                      {
                          log_error("strtol");
                          return EOF;
                      }
                      if(out_loc == buff)
                      {
                          log_error("Not a Number. No Digits Found!\n");
                          return EOF;
                      }
                      ret++;
                      break;
                  }
                  case 'u':
                  {
                      int* u = va_arg(vl, unsigned int*);
                      c = getchar();
                      char buff[100] = {0};
                      int k = 0;
                      while ((c != '\r') && (c != ' '))
                      {
                        if(c == '\b'){
                            if(k > 0){
                                k--;
                                putchar('\b');
                                putchar(' ');
                                putchar('\b');
                                putchar(' ');
                                buff[k] = 0;
                            }
                        }
                        else
                        {
                          buff[k++] = c;
                        }
                        log_trace("%c", c);
                        // buff[k] = c;
                        c = getchar();
                      }
                      buff[k]='\0';
                      *u = Str2Long(buff, &out_loc, 10);
                      if((_errno == ERANGE && (*u == LONG_MAX || *u == LONG_MIN)) || (_errno != 0 && *u == 0))
                      {
                          log_error("strtol");
                          return EOF;
                      }
                      if(out_loc == buff)
                      {
                          log_error("Not a Number. No Digits Found!\n");
                          return EOF;
                      }
                      ret++;
                      break;
                  }
            }
        } 
        else 
        {
            c = getchar();
            if (c != format[i])
            {
                ret = -1;
                break;
            }
        }
        i++;
    }
    return ret;
}

static inline int _vsscanf_(const char *inp, char const *fmt0, va_list ap)
{
	int inr;
	const unsigned char *fmt = (const unsigned char *)fmt0;
	int c;                  /* character from format, or conversion */
	size_t width;           /* field width, or 0 */
	char *p;                /* points into all kinds of strings */
	int n;                  /* handy integer */
	int flags;              /* flags as defined above */
	char *p0;               /* saves original value of p when necessary */
	int nassigned;          /* number of fields assigned */
	int nconversions;       /* number of conversions */
	int nread;              /* number of characters consumed from fp */
	int base;               /* base argument to Str2QInt/Str2UQInt */
	ccfntype ccfn;          /* conversion function (Str2QInt/Str2UQInt) */
	char ccltab[256];       /* character class table for %[...] */
	char buf[BUF];          /* buffer for numeric conversions */

	/* `basefix' is used to avoid `if' tests in the integer scanner */
	static short basefix[17] = { 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
				     12, 13, 14, 15, 16 };

	inr = StrLen(inp);

	nassigned = 0;
	nconversions = 0;
	nread = 0;
	base = 0;               /* XXX just to keep gcc happy */
	ccfn = NULL;            /* XXX just to keep gcc happy */
	for (;;) {
		c = *fmt++;
		if (c == 0)
			return (nassigned);
		if (isspace(c)) {
			while (inr > 0 && isspace(*inp))
				nread++, inr--, inp++;
			continue;
		}
		if (c != '%')
			goto literal;
		width = 0;
		flags = 0;
		/*
		 * switch on the format.  continue if done;
		 * break once format type is derived.
		 */
again:          c = *fmt++;
		switch (c) {
		case '%':
literal:
			if (inr <= 0)
				goto input_failure;
			if (*inp != c)
				goto match_failure;
			inr--, inp++;
			nread++;
			continue;

		case '*':
			flags |= SUPPRESS;
			goto again;
		case 'l':
			if (flags & LONG) {
				flags &= ~LONG;
				flags |= QUAD;
			} else {
				flags |= LONG;
			}
			goto again;
		case 'q':
			flags |= QUAD;
			goto again;
		case 'h':
			if (flags & SHORT) {
				flags &= ~SHORT;
				flags |= SHORTSHORT;
			} else {
				flags |= SHORT;
			}
			goto again;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			width = width * 10 + c - '0';
			goto again;

		/*
		 * Conversions.
		 *
		 */
		case 'd':
			c = CT_INT;
			ccfn = (ccfntype)Str2QInt;
			base = 10;
			break;

		case 'i':
			c = CT_INT;
			ccfn = (ccfntype)Str2QInt;
			base = 0;
			break;

		case 'o':
			c = CT_INT;
			ccfn = Str2UQInt;
			base = 8;
			break;

		case 'u':
			c = CT_INT;
			ccfn = Str2UQInt;
			base = 10;
			break;

		case 'x':
			flags |= PFXOK; /* enable 0x prefixing */
			c = CT_INT;
			ccfn = Str2UQInt;
			base = 16;
			break;

		case 's':
			c = CT_STRING;
			break;

		case '[':
			fmt = __sccl(ccltab, fmt);
			flags |= NOSKIP;
			c = CT_CCL;
			break;

		case 'c':
			flags |= NOSKIP;
			c = CT_CHAR;
			break;

		case 'p':       /* pointer format is like hex */
			flags |= POINTER | PFXOK;
			c = CT_INT;
			ccfn = Str2UQInt;
			base = 16;
			break;

		case 'n':
			nconversions++;
			if (flags & SUPPRESS)   /* ??? */
				continue;
			if (flags & SHORTSHORT)
				*va_arg(ap, char *) = nread;
			else if (flags & SHORT)
				*va_arg(ap, short *) = nread;
			else if (flags & LONG)
				*va_arg(ap, long *) = nread;
			else if (flags & QUAD)
				*va_arg(ap, signed int *) = nread;
			else
				*va_arg(ap, int *) = nread;
			continue;
		}

		/*
		 * We have a conversion that requires input.
		 */
		if (inr <= 0)
			goto input_failure;

		/*
		 * Consume leading white space, except for formats
		 * that suppress this.
		 */
		if ((flags & NOSKIP) == 0) {
			while (isspace(*inp)) {
				nread++;
				if (--inr > 0)
					inp++;
				else
					goto input_failure;
			}
			/*
			 * Note that there is at least one character in
			 * the buffer, so conversions that do not set NOSKIP
			 * can no longer result in an input failure.
			 */
		}

		/*
		 * Do the conversion.
		 */
		switch (c) {
		case CT_CHAR:
			/* scan arbitrary characters (sets NOSKIP) */
			if (width == 0)
				width = 1;
			if (flags & SUPPRESS) {
				size_t sum = 0;

				n = inr;
				if (n < width) {
					sum += n;
					width -= n;
					inp += n;
					if (sum == 0)
						goto input_failure;
				} else {
					sum += width;
					inr -= width;
					inp += width;
				}
				nread += sum;
			} else {
				memcpy(va_arg(ap, char *), inp, width);
				inr -= width;
				inp += width;
				nread += width;
				nassigned++;
			}
			nconversions++;
			break;

		case CT_CCL:
			/* scan a (nonempty) character class (sets NOSKIP) */
			if (width == 0)
				width = (size_t)~0;     /* `infinity' */
			/* take only those things in the class */
			if (flags & SUPPRESS) {
				n = 0;
				while (ccltab[(signed int)*inp]) {
					n++, inr--, inp++;
					if (--width == 0)
						break;
					if (inr <= 0) {
						if (n == 0)
							goto input_failure;
						break;
					}
				}
				if (n == 0)
					goto match_failure;
			} else {
				p = va_arg(ap, char *);
				p0 = p;
				while (ccltab[(signed int)*inp]) {
					inr--;
					*p++ = *inp++;
					if (--width == 0)
						break;
					if (inr <= 0) {
						if (p == p0)
							goto input_failure;
						break;
					}
				}
				n = p - p0;
				if (n == 0)
					goto match_failure;
				*p = 0;
				nassigned++;
			}
			nread += n;
			nconversions++;
			break;

		case CT_STRING:
			/* like CCL, but zero-length string OK, & no NOSKIP */
			if (width == 0)
				width = (size_t)~0;
			if (flags & SUPPRESS) {
				n = 0;
				while (!isspace(*inp)) {
					n++, inr--, inp++;
					if (--width == 0)
						break;
					if (inr <= 0)
						break;
				}
				nread += n;
			} else {
				p = va_arg(ap, char *);
				p0 = p;
				while (!isspace(*inp)) {
					inr--;
					*p++ = *inp++;
					if (--width == 0)
						break;
					if (inr <= 0)
						break;
				}
				*p = 0;
				nread += p - p0;
				nassigned++;
			}
			nconversions++;
			continue;

		case CT_INT:
			/* scan an integer as if by Str2QInt/Str2UQInt */
#ifdef hardway
			if (width == 0 || width > sizeof(buf) - 1)
				width = sizeof(buf) - 1;
#else
			/* size_t is unsigned, hence this optimisation */
			if (--width > sizeof(buf) - 2)
				width = sizeof(buf) - 2;
			width++;
#endif
			flags |= SIGNOK | NDIGITS | NZDIGITS;
			for (p = buf; width; width--) {
				c = *inp;
				/*
				 * Switch on the character; `goto ok'
				 * if we accept it as a part of number.
				 */
				switch (c) {
				/*
				 * The digit 0 is always legal, but is
				 * special.  For %i conversions, if no
				 * digits (zero or nonzero) have been
				 * scanned (only signs), we will have
				 * base==0.  In that case, we should set
				 * it to 8 and enable 0x prefixing.
				 * Also, if we have not scanned zero digits
				 * before this, do not turn off prefixing
				 * (someone else will turn it off if we
				 * have scanned any nonzero digits).
				 */
				case '0':
					if (base == 0) {
						base = 8;
						flags |= PFXOK;
					}
					if (flags & NZDIGITS)
						flags &= ~(SIGNOK | NZDIGITS | NDIGITS);
					else
						flags &= ~(SIGNOK | PFXOK | NDIGITS);
					goto ok;

				/* 1 through 7 always legal */
				case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
					base = basefix[base];
					flags &= ~(SIGNOK | PFXOK | NDIGITS);
					goto ok;

				/* digits 8 and 9 ok iff decimal or hex */
				case '8': case '9':
					base = basefix[base];
					if (base <= 8)
						break;  /* not legal here */
					flags &= ~(SIGNOK | PFXOK | NDIGITS);
					goto ok;

				/* letters ok iff hex */
				case 'A': case 'B': case 'C':
				case 'D': case 'E': case 'F':
				case 'a': case 'b': case 'c':
				case 'd': case 'e': case 'f':
					/* no need to fix base here */
					if (base <= 10)
						break;  /* not legal here */
					flags &= ~(SIGNOK | PFXOK | NDIGITS);
					goto ok;

				/* sign ok only as first character */
				case '+': case '-':
					if (flags & SIGNOK) {
						flags &= ~SIGNOK;
						goto ok;
						}
					break;

				/* x ok iff flag still set & 2nd char */
				case 'x': case 'X':
					if (flags & PFXOK && p == buf + 1) {
						base = 16;      /* if %i */
						flags &= ~PFXOK;
						goto ok;
					}
					break;
				}

				/*
				 * If we got here, c is not a legal character
				 * for a number.  Stop accumulating digits.
				 */
				break;
ok:
				/*
				 * c is legal: store it and look at the next.
				 */
				*p++ = c;
				if (--inr > 0)
					inp++;
				else
					break;          /* end of input */
			}
			/*
			 * If we had only a sign, it is no good; push
			 * back the sign.  If the number ends in `x',
			 * it was [sign] '' 'x', so push back the x
			 * and treat it as [sign] ''.
			 */
			if (flags & NDIGITS) {
				if (p > buf) {
					inp--;
					inr++;
				}
				goto match_failure;
			}
			c = ((unsigned char *)p)[-1];
			if (c == 'x' || c == 'X') {
				--p;
				inp--;
				inr++;
			}
			if ((flags & SUPPRESS) == 0) {
				unsigned int res;

				*p = 0;
				res = (*ccfn)(buf, (char **)NULL, base);
				if (flags & POINTER)
					*va_arg(ap, void **) =
					(void *)(uintptr_t)res;
				else if (flags & SHORTSHORT)
					*va_arg(ap, char *) = res;
				else if (flags & SHORT)
					*va_arg(ap, short *) = res;
				else if (flags & LONG)
					*va_arg(ap, long *) = res;
				else if (flags & QUAD)
					*va_arg(ap, signed int *) = res;
				else
					*va_arg(ap, int *) = res;
				nassigned++;
			}
			nread += p - buf;
			nconversions++;
			break;
		}
	}
input_failure:
		return (nconversions != 0 ? nassigned : -1);
match_failure:
		return (nassigned);
}

static inline int linearSearch(char arr[], int n, char target) {
  for (int i = 0; i < n; i++) {
    if (arr[i] == target) {
      return target;
    }
  }
  return -1;
}

/** @fn  static inline void itoa (unsigned long long int number, unsigned base)
 * @brief integer to string conversion
 * @param unsigned long long int
 * @param unsigned
 */


static inline void itoa(uint64_t number, unsigned base,
                        long int total_number_of_digits, char no_points) {
  int i = 0;
  unsigned int intermediate = 0;
  uint64_t digits[sizeof(number) * 8]; 

  while (1) {
    digits[i] = number % base;
    
    if (number < base) {
      i++;
      break;
    }

    number /= base; 
    i++;
  }
  if (total_number_of_digits != -1) {
    for (long int prepend_index = 0; prepend_index < total_number_of_digits - i;
         prepend_index++) {
      putchar(48); 
    }
  }
  int y = ((int)no_points) - 48;
  if (y == 0)
  {
    while (i-- > 0) 
    {
        if (digits[i] >= 10)
            intermediate = 'a' - 10;
        else
            intermediate = '0';

        putchar(digits[i] + intermediate);
    }     
  }
  if (y != 0)
  {
      for (;;)
      {
          if (y>i)
          {
              putchar('0');
              y--;
          }
          else if (y<=i)
          {
              break;
          }
      }
      if (y==i)
      {
          while (i-- > 0)
          {
              if (digits[i] >= 10)
              {
                  intermediate = 'a' - 10;
              }
              else 
              {
                  intermediate = '0';
              }
              putchar(digits[i] + intermediate);
          }
      }
      else if (y<i)
      {
          while (i-- > 0)
          {
              if (digits[i] >= 10)
              {
                  intermediate = 'a' - 10;
              }
              else 
              {
                  intermediate = '0';
              }
              putchar(digits[i] + intermediate);
              --y;
              if (y==0)
              {
                  i=0;
              }
          }
      }
  }
}

void _print_(const char *fmt, va_list ap) {
  char arr[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  int n = sizeof(arr) / sizeof(arr[0]);
  int result;
  register const char *p;
  const char *last_fmt;
  register int ch;
  uint64_t num;
  int base, lflag, i;
  float float_num = 0;
  char float_arr[64] = {'\0'};
  //maybe if it gets extended the values gets corrupted
  char no_points= '0';
  int backtothebeginning;

  for (;;) {
    for (; (ch = *(unsigned char *)fmt) != '%'; fmt++) {
      if (ch == '\0')
        return;
      putchar(ch);
    }
    fmt++;

    // Process a %-escape sequence
    last_fmt = fmt;
    lflag = 0;
    long int total_number_of_digits = -1;

    backtothebeginning = 0;
    for (;;) {
      switch (ch = *(unsigned char *)fmt++) {
      // long flag (doubled for long long)
      case 'l':
        lflag++;
        backtothebeginning = 1;
        break;

      // character
      case 'c':
        putchar(va_arg(ap, int));
        break;

      // string
      case 's':
        if ((p = va_arg(ap, char *)) == NULL)
          p = "(null)";
        for (; (ch = *p) != '\0';) {
          putchar(ch);
          p++;
        }
        break;

      // (signed) decimal
      case 'd':
        base = 10;
        if (lflag >= 2)
          num = va_arg(ap, long long);
        else if (lflag == 1)
          num = va_arg(ap, long);
        else
          num = va_arg(ap, int);

        if ((long long)num < 0) {
          putchar('-');
          num = -(long long)num;
        }

        itoa(num, base, total_number_of_digits,no_points);
        total_number_of_digits = -1;
        break;

      // float
      case 'f':{
        double value;
        value= va_arg(ap, double);
        Float2Str(value, 4, no_points);
        break;
      }
        
      //point  
      case '.':
        ch = *(unsigned char *)fmt++;
        if (ch == '0') {
          ch = *(unsigned char *)fmt++;
        }
        no_points = ch;
        backtothebeginning = 1;
        break;

      // (unsigned) decimal
      case 'u':
        base = 10;

        if (lflag >= 2)
          num = va_arg(ap, unsigned long long);
        else if (lflag)
          num = va_arg(ap, unsigned long);
        else
          num = va_arg(ap, unsigned int);
        itoa(num, base, total_number_of_digits,no_points);
        total_number_of_digits = -1;

        break;

      // (unsigned) octal
      case 'o':
        // should do something with padding so it's always 3 octets
        base = 8;

        if (lflag >= 2)
          num = va_arg(ap, unsigned long long);
        else if (lflag)
          num = va_arg(ap, unsigned long);
        else
          num = va_arg(ap, unsigned int);

        itoa(num, base, total_number_of_digits,no_points);
        total_number_of_digits = -1;

        break;

      // hexadecimal
      case 'x':
        // printf("Hi i am hex\n");
        base = 16;

        if (lflag >= 2)
          num = va_arg(ap, unsigned long long);
        else if (lflag)
          num = va_arg(ap, unsigned long);
        else
          num = va_arg(ap, unsigned int);
        itoa(num, base, total_number_of_digits,no_points);
        total_number_of_digits = -1;

        break;

      // escaped '%' character
      case '%':
        putchar(ch);
        break;

      // prepend zeros
      case '0':
        ch = *(unsigned char *)fmt++;
        total_number_of_digits = 0;
        int index = 0;
        while ((ch >= 48) && (ch <= 57)) {
          total_number_of_digits =
              total_number_of_digits + (ch - 48) * powers(10, index);
          ch = *(unsigned char *)fmt++;
          index += 1;
        }
        fmt--;
        backtothebeginning = 1;
        break;

      // unrecognized escape sequence - just printf it literally
      default:
        /*putchar('%');
        fmt = last_fmt;
        break;*/
        result = linearSearch(arr, n, ch);
        if (result == -1) {
          putchar('%');
          fmt = last_fmt;
          break;
        } else {
          no_points = ch;
          backtothebeginning = 1;
          break;
        }
      }
      if (backtothebeginning) {
        backtothebeginning = 0;
        continue;
      } else
        break;
    }
  }
}

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt); //

  _print_(fmt, ap);

  va_end(ap);
  return 0; // incorrect return value, but who cares, anyway?
}

int scanf(const char* format, ...)
{
	va_list vl;
    int done;
 	va_start( vl, format);

	done = _scan_(format, vl);

    va_end(vl);
    return done;
}

int sscanf(const char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = _vsscanf_(buf, fmt, args);
	va_end(args);
	return i;
}

static inline void append_char(char **buffer, char c) {
    **buffer = c;
    (*buffer)++;
}

static inline void append_string(char **buffer, const char *str) {
    while (*str) {
        append_char(buffer, *str++);
    }
}

static inline void append_int(char **buffer, int num) {
    char temp[20]; // Sufficient for 32-bit integer
    char *p = temp;
    int sign = num < 0 ? -1 : 1;

    if (num < 0) {
        num = -num;
    }

    do {
        *p++ = '0' + (num % 10);
        num /= 10;
    } while (num > 0);

    if (sign < 0) {
        *p++ = '-';
    }

    while (p > temp) {
        append_char(buffer, *--p);
    }
}

static inline void append_float(char **buffer, float num, int precision) {
    int int_part = (int)num;
    float frac_part = num - int_part;

    append_int(buffer, int_part);  // Append the integer part

    append_char(buffer, '.');      // Append the decimal point

    // Handle fractional part with dynamic precision
    for (int i = 0; i < precision; i++) {
        frac_part *= 10;
    }

    int frac = (int)(frac_part + 0.5); // Adding 0.5 for rounding

    // Trim trailing zeros for any precision
    while (frac % 10 == 0 && frac > 0) {
        frac /= 10;
    }

    append_int(buffer, frac); // Append the fractional part
}

// #undef sprintf
// void sprintf(char *buffer, const char *format, ...) {
//     va_list args;
//     va_start(args, format);

//     char *buf_ptr = buffer;

//     while (*format) {
//         if (*format == '%') {
//             format++;
//             switch (*format) {
//                 case 'd':
//                     append_int(&buf_ptr, va_arg(args, int));
//                     break;
//                 case 'f':
//                     append_float(&buf_ptr, (float)va_arg(args, double), 8); // Default to 8 decimal places
//                     break;
//                 case 's':
//                     append_string(&buf_ptr, va_arg(args, char *));
//                     break;
//                 default:
//                     append_char(&buf_ptr, '%');
//                     append_char(&buf_ptr, *format);
//                     break;
//             }
//         } else {
//             append_char(&buf_ptr, *format);
//         }
//         format++;
//     }

//     *buf_ptr = '\0';
//     va_end(args);
// } 
