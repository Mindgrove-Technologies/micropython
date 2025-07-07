/***************************************************************************
 * Project           		: shakti devt board
 * Name of the file	     	: log.c
 * Brief Description of file    : printf control for all operation
 * Name of Author    	        : Abhinav ramnath & Sathya Narayanan N
 * Email ID                     : sathya281@gmail.com

 Copyright (C) 2019  IIT Madras. All rights reserved.

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
@file log.c
@brief logger application for debugging and normal uart display.
@details This file contains implementation of different log levels.
*/ 

#include "log.h"
#include "utils.h"
#include "io.h"

/** @fn void log_trace(const char*fmt, ...)
 * @brief Function to printf trace logs
 * @details This function printf trace logs if the log_level allows trace logs
 * @param const char* (printf formatted arguments with format specifiers)
 */
void log_trace(const char* fmt, ...)
{
	if (TRACE <= log_level) {
		va_list ap;
		va_start(ap, fmt);

		_print_(fmt, ap);

		va_end(ap);
	}
}

/** @fn void log_debug(const char*fmt, ...)
 * @brief Function to printf debug logs
 * @details This function printf debug logs if the log_level allows debug logs
 * @param const char* (printf formatted arguments with format specifiers)
 */
void log_debug(const char* fmt, ...)
{
	if (DEBUG <= log_level) {
		va_list ap;
		va_start(ap, fmt);

		_print_(fmt, ap);

		va_end(ap);
	}
}

/** @fn void log_info(const char*fmt, ...)
 * @brief Function to printf info logs
 * @details This function printf info logs if the log_level allows info logs
 * @param const char* (printf formatted arguments with format specifiers)
 */
void log_info(const char* fmt, ...)
{
	if (INFO <= log_level) {
		va_list ap;
		va_start(ap, fmt);

		_print_(fmt, ap);

		va_end(ap);
	}
}

/** @fn void log_warn(const char*fmt, ...)
 * @brief Function to printf warn logs
 * @details This function printf trace warn if the log_level allows warn logs
 * @param const char* (printf formatted arguments with format specifiers)
 */
void log_warn(const char* fmt, ...)
{
	if (WARN <= log_level) {
		va_list ap;
		va_start(ap, fmt);

		_print_(fmt, ap);

		va_end(ap);
	}
}

/** @fn void log_error(const char*fmt, ...)
 * @brief Function to printf error logs
 * @details This function printf error logs if the log_level allows error logs
 * @param const char* (printf formatted arguments with format specifiers)
 */
void log_error(const char* fmt, ...)
{
	if (ERROR <= log_level) {
		va_list ap;
		va_start(ap, fmt);

		_print_(fmt, ap);

		va_end(ap);
	}
}

/** @fn void log_fatal(const char*fmt, ...)
 * @brief Function to printf fatal logs
 * @details This function printf fatal logs if the log_level allows fatal logs
 * @param const char* (printf formatted arguments with format specifiers)
 */
void log_fatal(const char* fmt, ...)
{
	if (FATAL <= log_level) {
		va_list ap;
		va_start(ap, fmt);

		_print_(fmt, ap);

		va_end(ap);
	}

	while(1);
}
