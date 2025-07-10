/**
 * @file RTC_driver.h
 * @author Vishwajith.N.S (vishwajith@mindgrovetech.in)
 * @brief Contains function prototypes of RTC application
 * @version 1.0
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2024. All rights reserved.
 * 
 */
#ifndef RTC_H
#define RTC_H
#include <stdint.h>
#include<stdbool.h>
#include "i2c.h"//Includes the definitions of i2c communication protocol//
#define CLOCK_H24 0
#define CLOCK_H12 1

#define HOUR_AM 0
#define HOUR_PM 1
#define HOUR_24 2

#define DS3231_ADDR 0x68


/**
 * @fn uint8_t bcd2bin(uint8_t val)
 * 
 * @brief This function converts given BCD number to Binary form.
 * 
 * @param val The parameter \a val is input BCD number.
 * @return returns Binary Equivalent of given BCD number. 
 */
uint8_t bcd2bin(uint8_t val);

/**
 * @fn uint8_t bin2bcd(uint8_t val)
 * 
 * @brief This function converts given Binary number to BCD form.
 * 
 * @param val The parameter \a val is input Binary number.
 * @return returns BCD Equivalent of given Binary number.  
 */
uint8_t bin2bcd(uint8_t val);

/**
 * @fn uint8_t bitSet(uint8_t data,uint8_t shift)
 * 
 * @brief This function sets a particular bit in the given data.
 * 
 * @param data The parameter \a data is value which has to be manipulated.
 * @param shift The parameter \a shift is the bit which has to be set.
 * @return returns the required manipulated value.
 */
uint8_t bitSet(uint8_t data,uint8_t shift);

/**
 * @fn uint8_t bitClear(uint8_t data,uint8_t shift)
 * 
 * @brief This function clears a particular bit in the given data.
 * 
 * @param data The parameter \a data is value which has to be manipulated.
 * @param shift The parameter \a shift is the bit which has to be cleared.
 * @return returns the required manipulated value.
 */
uint8_t bitClear(uint8_t data,uint8_t shift);

/**
 * @fn uint8_t bitRead(uint8_t data,uint8_t shift)
 * 
 * @brief This function reads a particular bit in the given data.
 * 
 * @param data The parameter \a data is value from which a particular bit has to be read.
 * @param shift The parameter \a shift is the bit position which has to be read.
 * @return returns the required bit position value.
 */
uint8_t bitRead(uint8_t data,uint8_t shift);

/**
 * @fn void DS3231_begin(uint8_t instance_number,uint32_t clock_frequency)
 * 
 * @brief This function is used to set SCL clock freqeuncy for I2C peripheral connected to RTC.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param shift The parameter \a clock_frequency is the bit which has to be set.
 * @return returns the required manipulated value.
 */
void DS3231_begin(uint8_t instance_number,uint32_t clock_frequency);

/**
 * @fn uint8_t bitWrite(uint8_t data,uint8_t shift,uint8_t bit)
 * 
 * @brief This function sets a particular bit/clears in the given data.
 * 
 * @param data The parameter \a data is value which has to be manipulated.
 * @param shift The parameter \a shift is the bit which has to be set.
 * @param bit The parameter \a bit is the value which is going to be placed in required place.
 */
uint8_t bitWrite(uint8_t data,uint8_t shift,uint8_t bit);

/**
 * @fn void DS3231_setHourMode(uint8_t i2c_number,uint8_t h_mode)
 * 
 * @brief This function sets hour mode either 24 or 12 hr clock mode.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param h_mode The parameter \a h_mode is the used to select hour mode.For 12hr mode use \a CLOCK_H12 amd for 24 hr mode use \a CLOCK_H24.
 */
void DS3231_setHourMode(uint8_t i2c_number,uint8_t h_mode);

/**
 * @fn uint8_t DS3231_getHourMode(uint8_t i2c_number)
 * 
 * @brief This function gets current hour mode.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @return  CLOCK_H12 if 12 hr mode is set and CLOCK_H24 if 24 hr mode is set.
 */
uint8_t DS3231_getHourMode(uint8_t i2c_number);

/**
 * @fn void DS3231_setMeridiem(uint8_t i2c_number,uint8_t meridiem)
 * 
 * @brief This function sets Meridiem either AM or PM in 12hr clock mode.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param meridiem The parameter \a meridiem is used to meridiem use HOUR_AM if AM and HOUR_PM if PM has to be set.
 * 
 */
void DS3231_setMeridiem(uint8_t i2c_number,uint8_t meridiem);

/**
 * @fn uint8_t DS3231_getMeridiem(uint8_t i2c_number)
 * 
 * @brief This function gets Meridiem either AM or PM in 12hr clock mode.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * 
 * @return returns HOUR_AM if AM and HOUR_PM if PM.
 */
uint8_t DS3231_getMeridiem(uint8_t i2c_number);

/**
 * @fn uint8_t DS3231_getSeconds(uint8_t i2c_number)
 * 
 * @brief This function used to get seconds value from RTC.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * 
 * @return returns Seconds value.
 */
uint8_t DS3231_getSeconds(uint8_t i2c_number);

/**
 * @fn void DS3231_setSeconds(uint8_t i2c_number,uint8_t seconds)
 * 
 * @brief This function used to set seconds value in RTC.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param seconds The parameter \a seconds is used to set seconds value.
 * 
 */
void DS3231_setSeconds(uint8_t i2c_number,uint8_t seconds);

/**
 * @fn uint8_t DS3231_getMinutes(uint8_t i2c_number)
 * 
 * @brief This function used to get minutes value from RTC.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * 
 * @return returns Minutes value.
 */
uint8_t DS3231_getMinutes(uint8_t i2c_number);

/**
 * @fn void DS3231_setMinutes(uint8_t i2c_number,uint8_t minutes)
 * 
 * @brief This function used to set minutes value in RTC.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param minutes The parameter \a minutes is used to set minutes value.
 * 
 */
void DS3231_setMinutes(uint8_t i2c_number,uint8_t minutes);

/**
 * @fn uint8_t DS3231_getHours(uint8_t i2c_number)
 * 
 * @brief This function used to get hours value from RTC.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * 
 * @return returns Hours value.
 */
uint8_t DS3231_getHours(uint8_t i2c_number);

/**
 * @fn void DS3231_setHours(uint8_t i2c_number,uint8_t minutes)
 * 
 * @brief This function used to set  hours value in RTC.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param hours The parameter \a hours is used to set hours value.
 * 
 */
void  DS3231_setHours(uint8_t i2c_number,uint8_t hours);

/**
 * @fn uint8_t DS3231_getWeek(uint8_t i2c_number)
 * 
 * @brief This function is used to get week value.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @return Returns week value.
 */
uint8_t DS3231_getWeek(uint8_t i2c_number);

/**
 * @fn void DS3231_setWeek(uint8_t i2c_number,uint8_t week)
 * 
 * @brief This function is used to set week.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param week The parameter \a year is used to set week.
 * 
 */
void DS3231_setWeek(uint8_t i2c_number,uint8_t week);

/**
 * @fn void DS3231_updateWeek(uint8_t i2c_number)
 * 
 * @brief This function is used to update week.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * 
 */
void DS3231_updateWeek(uint8_t i2c_number);

/**
 * @fn uint8_t DS3231_getDay(uint8_t i2c_number)
 * 
 * @brief This function is used to get day value.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @return Returns Day value.
 */
uint8_t DS3231_getDay(uint8_t i2c_number);

/**
 * @fn void DS3231_setDay(uint8_t i2c_number,uint8_t day)
 * 
 * @brief This function is used to set day.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param day The parameter \a year is used to set day.
 * 
 */
void DS3231_setDay(uint8_t i2c_number,uint8_t day);

/**
 * @fn uint16_t DS3231_getMonth(uint8_t i2c_number)
 * 
 * @brief This function is used to get Month value.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @return Returns Month value.
 */
uint8_t DS3231_getMonth(uint8_t i2c_number);

/**
 * @fn void DS3231_setMonth(uint8_t i2c_number,uint8_t month)
 * 
 * @brief This function is used to set month.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param month The parameter \a year is used to set month.
 * 
 */
void DS3231_setMonth(uint8_t i2c_number,uint8_t month);

/**
 * @fn uint16_t DS3231_getYear(uint8_t i2c_number)
 * 
 * @brief This function is used to get year.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @return Returns Year value.
 */
uint16_t DS3231_getYear(uint8_t i2c_number);

/**
 * @fn void DS3231_setYear(uint8_t i2c_number,uint16_t year)
 * 
 * @brief This function is used to set year.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param year The parameter \a year is used to set year.
 * 
 */
void DS3231_setYear(uint8_t i2c_number,uint16_t year);

/**
 * @fn void DS3231_setTime(uint8_t i2c_number,uint8_t hours, uint8_t minutes, uint8_t seconds)
 * 
 * @brief This function is used to set time.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param hours The parameter \a hours is used to set hours.
 * @param minutes The parameter \a minutes is used to set minutes.
 * @param seconds The parameter \a seconds is used to set seconds. 
 *
 */
void DS3231_setTime(uint8_t i2c_number,uint8_t hours, uint8_t minutes, uint8_t seconds);

/**
 * @fn void DS3231_setDate(uint8_t i2c_number,uint8_t day, uint8_t month, uint16_t year)
 * 
 * @brief This function is used to set date.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param hours The parameter \a hours is used to set hours.
 * @param minutes The parameter \a minutes is used to set minutes.
 * @param seconds The parameter \a seconds is used to set seconds. 
 *
 */
void DS3231_setDate(uint8_t i2c_number,uint8_t day, uint8_t month, uint16_t year);

/**
 * @fn void DS3231_setDateTime(uint8_t i2c_number,char* date, char* time)
 * 
 * @brief This function is used to set date and time by getting date and time as string.
 * 
 * @param instance_number The parameter \a instance_number specifies which instance of I2C has to be used.
 * @param date The parameter \a date is pointer to string which has date.
 * @param time The parameter \a time is pointer to string which has time.
 *
 */
void DS3231_setDateTime(uint8_t i2c_number,char* date, char* time);
uint32_t DS3231_getEpoch(uint8_t i2c_number);
int is_leap_year(int year);
uint32_t rtc_to_epoch(int year, int month, int day, int hour, int min, int sec);
uint32_t DS3231_getEpoch_calc(uint8_t i2c_number);
#endif





