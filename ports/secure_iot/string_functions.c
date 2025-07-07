/*******************************************************************************
 * Project                     : Mindgrove Silicon's Secure-IoT SoC
 * Name of the file	           : string_functions.c
 * Brief Description of file   : Source File for String related functionalities
 * Name of Author    	       : Kapil Shyam. M
 * Email ID                    : kapil@mindgrovetech.in

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
 *********************************************************************************/

/**
@file string_functions.c
@brief source file for strings related functionalities
@details Functions like HextoInt Conversion
*/

#include "string_functions.h"
#include "memory_functions.h"
#include "uart.h"

int atoi(const char *str) {
    // Initialize variables
    int result = 0;
    int sign = 1;
    
    // Skip whitespace characters
    while (*str == ' ') {
        str++;
    }
    
    // Handle optional sign
    if (*str == '+' || *str == '-') {
        if (*str == '-') {
            sign = -1;
        }
        str++;
    }
    
    // Convert characters to integer
    while (*str >= '0' && *str <= '9') {
        // Check for overflow
        if (result > (INT_MAX - (*str - '0')) / 10) {
            return (sign == 1) ? INT_MAX : INT_MIN; // Return INT_MAX or INT_MIN on overflow
        }
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

size_t StrLen(const char *str) {
    const char *s = str;
    while (*s) {  // Iterate until we reach the null terminator
        s++;
    }
    return s - str;  // Calculate the length by subtracting the pointer
}

void ReverseStr(char *str, int length)
{
	int i = 0;
	int j = length - 1;
	char tmp;

	while (i<j)
	{
		tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;

		i++;
		j--;
	}
}

int Int2Str(int number, char str[], unsigned int afterpoint)
{
	uint32_t i = 0;

	/*extract each digit and put into str[i]*/
	while (number != 0)
	{
		str[i] = ((number%10) + '0');
		i++;
		number = number/10;
	}

	/*insert 0 after the numbers, if count of digits less than afterpoint*/

	while (i < afterpoint)
	{
		str[i] = '0';
		i++;
	}

	/*
	   zeroth digit is in oth position in array,
	   To read digits properly, reverse array
	 */
	ReverseStr(str, i);
	str[i] = '\0';

	return i;
}

#define MAX_PRECISION (10)

static const double rounders[MAX_PRECISION + 1] = {
    0.5,          // 0
    0.05,         // 1
    0.005,        // 2
    0.0005,       // 3
    0.00005,      // 4
    0.000005,     // 5
    0.0000005,    // 6
    0.00000005,   // 7
    0.000000005,  // 8
    0.0000000005, // 9
    0.00000000005 // 10
};

void Float2Str(double f, int precision, char no_points) {
  char *p1;
  int c;
  int z = ((int)no_points) - 48;

  // check precision bounds
  if (precision > MAX_PRECISION)
    precision = MAX_PRECISION;

  // sign stuff
  if (f < 0) {
    f = -f;
    putchar('-'); // ASCII value - 45
  }
  //
  if (precision < 0) // negative precision == automatic precision guess
  {
    if (f < 1.0)
      precision = 6;
    else if (f < 10.0)
      precision = 5;
    else if (f < 100.0)
      precision = 4;
    else if (f < 1000.0)
      precision = 3;
    else if (f < 10000.0)
      precision = 2;
    else if (f < 100000.0)
      precision = 1;
    else
      precision = 0;
  }

  // round value according the precision
  if (precision)
    f += rounders[precision];

  double decimalPart, intPart_d;
  decimalPart = modfloat(f, &intPart_d);
  long intPart = (int)intPart_d;

  // integer part
  if (!intPart)
    putchar('0');
  else 
  {
    // convert (reverse order)
    long copy_intPart = intPart;
    int len_intPart = 0;
    // Get the length of int
    while (copy_intPart) {
      copy_intPart /= 10;
      len_intPart++;
    }
    int y = len_intPart;
    // Reverse the int part
    long reversed_intPart = 0;
    while (intPart) {
      reversed_intPart += (intPart % 10) * powers(10, --len_intPart);
      intPart /= 10;
    }
    // Reverse the int part and printf - reversing to two times will printf int in
    // correct format. This is done because the following algo prints only from
    // the least significant digit
    while (y) {
      if (reversed_intPart > (10 ^ (y - 1))) {
        putchar('0' + reversed_intPart % 10);
        reversed_intPart /= 10;
      } else {
        putchar('0' + reversed_intPart % 10);
        reversed_intPart /= 10;
        // putchar('0');
      }
      y--;
    }
  }
  if (no_points == 48)
  {
    // decimal part
    if (precision) 
    {
        // place decimal point
        putchar('.');

        // convert
        while (precision--) 
        {
            decimalPart *= 10.0;
            c = (int)decimalPart;
            putchar('0' + c);
            decimalPart -= c;
        }
    }
  }
  if (no_points != 48)
  {
    // decimal part
    if (precision) 
    {
        // place decimal point
        putchar('.');

        // convert
        while (no_points != 48) 
        {
            decimalPart *= 10.0;
            c = (int)decimalPart;
            putchar('0' + c);
            decimalPart -= c;
            no_points--;
        }
    }
  }
}

int Hex2Int(const char p[], int pos) {
	int i = 0;
	int val = 0x0;
	for(i = 0; pos >= 0; pos--,i++)
	{
		switch(p[i])
		{
			case '0': val |= 0x0<<pos*4; break;
			case '1': val |= 0x1<<pos*4; break;
			case '2': val |= 0x2<<pos*4; break;
			case '3': val |= 0x3<<pos*4; break;
			case '4': val |= 0x4<<pos*4; break;
			case '5': val |= 0x5<<pos*4; break;
			case '6': val |= 0x6<<pos*4; break;
			case '7': val |= 0x7<<pos*4; break;
			case '8': val |= 0x8<<pos*4; break;
			case '9': val |= 0x9<<pos*4; break;
			case 'A':
			case 'a':val |= 0xa<<pos*4; break;
			case 'B':
			case 'b':val |= 0xb<<pos*4; break;
			case 'C':
			case 'c':val |= 0xc<<pos*4; break;
			case 'D':
			case 'd':val |= 0xd<<pos*4; break;
			case 'E':
			case 'e':val |= 0xe<<pos*4; break;
			case 'F':
			case 'f':val |= 0xf<<pos*4; break;
			default:
					 return val;
		}
	}
	return val;
}

long Str2Long(const char *nptr, char **endptr, int base)
{
  long val = 0;
  int c, neg = 0;
  const char *p = nptr;

  /* skip leading white space */
  while (isspace((unsigned char)*p))
    p++;

  /* check for a sign */
  if (*p == '-' || *p == '+') {
    neg = (*p == '-');
    p++;
  }

  /* check for a base specifier */
  if (base == 0) {
    if (*p == '0') {
      if (*++p == 'x' || *p == 'X') {
        p++;
        base = 16;
      } else {
        base = 8;
      }
    } else {
      base = 10;
    }
  } else if (base == 16) {
    if (*p == '0' && (*++p == 'x' || *p == 'X'))
      p++;
  }

  /* convert the string to an integer */
  while ((c = *p) != '\0') {
    int digit;
    if (isdigit((unsigned char)c))
      digit = c - '0';
    else if (isalpha((unsigned char)c))
      digit = toupper((unsigned char)c) - 'A' + 10;
    else
      break;
    if (digit >= base)
      break;
    val = val * base + digit;
    p++;
  }

  /* store the end pointer */
  if (endptr != NULL)
    *endptr = (char *)p;

  /* return the result */
  return neg ? -val : val;
}


float Str2Float(const char* str, char** endptr)
{
	float result = 0.0;
	int sign = 1;

	// Checking the integer whether +ve or -ve
	if(*str == '-')
	{
		sign = -1;
		str++;
	}
	else if (*str == '+')
	{
		str++;
	}

	// Calculate the integer part
	while(*str >= '0' && *str <= '9')
	{
		result = result*10 + (*str - '0');
		str++;
	}

	// Calculate the floating point part
	if (*str == '.')
	{
		str++;
		float base = 0.1;
		while(*str >= '0' && *str <= '9')
		{
			result += base*(*str - '0');
			base /= 10.0;
			str++;
		}
	}

        // Returning the value with sign
	*endptr = (char*)&str;

	return result*sign;
}

struct str_info *Str2Int(const char **nptr, int base, unsigned int unsign)
{
	const char *s = *nptr;
	unsigned int acc;
	signed int c;
	unsigned long long int cutoff;
	int neg, any, cutlim;
	unsigned int qbase;
	struct str_info *info;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	info = (struct str_info *)malloc(sizeof(struct str_info));
	if (!info)
		return NULL;

	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == '+')
			c = *s++;
	}
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for quads is
	 * [-9223372036854775808..9223372036854775807] and the input base
	 * is 10, cutoff will be set to 922337203685477580 and cutlim to
	 * either 7 (neg==0) or 8 (neg==1), meaning that if we have
	 * accumulated a value > 922337203685477580, or equal but the
	 * next digit is > 7 (or 8), the number is too big, and we will
	 * return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	qbase = (unsigned int)base;

	if (!unsign) {
		cutoff = neg ? -(LLONG_MIN+1) : LLONG_MAX;
		cutlim = cutoff % qbase;
		cutoff /= qbase;
	} else {
		cutoff = (unsigned int)ULLONG_MAX / qbase;
		cutlim = (unsigned int)ULLONG_MAX % qbase;
	}

	for (acc = 0, any = 0;; c = *s++) {
		if (!isascii(c))
			break;
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
			any = -1;
		} else {
			any = 1;
			acc *= qbase;
			acc += c;
		}
	}

	info->any = any;
	info->neg = neg;
	info->acc = acc;

	*nptr = s;

	return info;
}

signed int Str2QInt(const char *nptr, char **endptr, int base)
{
	const char *s = nptr;
	unsigned int acc;
	int unsign = 0;
	struct str_info *info;

	info = Str2Int(&s, base, unsign);
	if (!info)
		return -1;

	acc = info->acc;

	if (info->any < 0)
		acc = info->neg ? LLONG_MIN : LLONG_MAX;
	else if (info->neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = __DECONST(char *, info->any ? s - 1 : nptr);

	free(info);

	return acc;
}

unsigned int Str2UQInt(const char *nptr, char **endptr, int base)
{
		const char *s = nptr;
	unsigned long long int acc;
	int unsign = 1;
	struct str_info *info;

	info = Str2Int(&s, base, unsign);
	if (!info)
		return -1;

	acc = info->acc;

	if (info->any < 0)
		acc = ULLONG_MAX;
	else if (info->neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = __DECONST(char *, info->any ? s - 1 : nptr);

	free(info);

	return acc;
}