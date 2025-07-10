/**
 * @file RTC_driver.c
 * @author Vishwajith.N.S (vishwajith@mindgrovetech.in)
 * @brief Contains function definitions of RTC application
 * @version 1.0
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) Mindgrove Technologies Pvt. Ltd 2024. All rights reserved.
 * 
 */
#include"RTC_driver.h"

uint8_t bcd2bin(uint8_t val)
{
	return val - 6 * (val >> 4);
}

uint8_t bin2bcd(uint8_t val)
{
	return val + 6 * (val / 10);
}

uint8_t bitSet(uint8_t data,uint8_t shift){
	return (data|(1<<shift));
}
uint8_t bitClear(uint8_t data,uint8_t shift){
	printf("bitclear\n");
	return (data&(~(1<<shift)));
}
uint8_t bitRead(uint8_t data,uint8_t shift){
	printf("bitread\n");
	return ((data&(1<<shift))>>shift);
}
void DS3231_begin(uint8_t instance_number,uint32_t clock_frequency)
{
	I2C_Init(instance_number,clock_frequency); 
}
uint8_t bitWrite(uint8_t data,uint8_t shift,uint8_t bit){
	printf("bitwrite\n");
	return ( (data & ~(1<<shift)) | (bit<<shift) );
}



void DS3231_setHourMode(uint8_t i2c_number,uint8_t h_mode)
{
	uint8_t data[2];
	data[0] = 0x02;

	if(h_mode == CLOCK_H12 || h_mode == CLOCK_H24)
	{
		I2C_Transmit(i2c_number,DS3231_ADDR,&data,1,START_BIT|STOP_BIT);
		I2C_Recieve(i2c_number,DS3231_ADDR,&data[1],1,START_BIT|STOP_BIT);
		data[1]=bitWrite(data[1], 6, h_mode);
		I2C_Transmit(i2c_number,DS3231_ADDR,&data,2,START_BIT|STOP_BIT);
	}
}

uint8_t DS3231_getHourMode(uint8_t i2c_number)
{
	bool h_mode;
	uint8_t data = 0x02;
	printf("DS3231_getHourMode\n");
	I2C_Transmit(i2c_number,DS3231_ADDR,&data,1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&data,1,START_BIT|STOP_BIT);
	h_mode = bitRead(data, 6);
	return (h_mode);
}


void DS3231_setMeridiem(uint8_t i2c_number,uint8_t meridiem)
{
	uint8_t data[2];
	data[0] = 0x02;
	if(meridiem == HOUR_AM || meridiem == HOUR_PM)
	{
		if (DS3231_getHourMode(i2c_number) == CLOCK_H12)
		{
		I2C_Transmit(i2c_number,DS3231_ADDR,&data,1,START_BIT|STOP_BIT);
		I2C_Recieve(i2c_number,DS3231_ADDR,&data[1],1,START_BIT|STOP_BIT);
		data[1]=bitWrite(data[1], 5, meridiem);
		I2C_Transmit(i2c_number,DS3231_ADDR,&data,2,START_BIT|STOP_BIT);
		}
	}
}

uint8_t DS3231_getMeridiem(uint8_t i2c_number)
{
	bool flag;
	uint8_t data = 0x02;
	if (DS3231_getHourMode(i2c_number) == CLOCK_H12)
	{
	I2C_Transmit(i2c_number,DS3231_ADDR,&data,1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&data,1,START_BIT|STOP_BIT);
		flag = bitRead(data, 5);
		return (flag);
	}
	else
		return (HOUR_24);
}

uint8_t DS3231_getSeconds(uint8_t i2c_number)
{
	uint8_t seconds = 0x00;
	I2C_Transmit(i2c_number,DS3231_ADDR,&seconds,1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&seconds,1,START_BIT|STOP_BIT);
	return (bcd2bin(seconds));
}

void DS3231_setSeconds(uint8_t i2c_number,uint8_t seconds)
{
	uint8_t data[2];
	data[0] = 0x00;
	data[1] = bin2bcd(seconds);
	if (seconds >= 00 && seconds <= 59)
		I2C_Transmit(i2c_number,DS3231_ADDR,data,2,START_BIT|STOP_BIT);
	printf("DS3231_setSeconds\n");
}


uint8_t DS3231_getMinutes(uint8_t i2c_number)
{
	uint8_t minutes = 0x01;
	I2C_Transmit(i2c_number,DS3231_ADDR,&minutes,1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&minutes,1,START_BIT|STOP_BIT);
	return (bcd2bin(minutes));
}

void DS3231_setMinutes(uint8_t i2c_number,uint8_t minutes)
{
	uint8_t data[2];
	data[0] = 0x01;
	data[1] = bin2bcd(minutes);
	printf("DS3231_setMinutes\n");
	if (minutes >= 00 && minutes <= 59)
		I2C_Transmit(i2c_number,DS3231_ADDR,data,2,START_BIT|STOP_BIT);
}


uint8_t DS3231_getHours(uint8_t i2c_number)
{
	uint8_t hours = 0x02;
	bool h_mode;
	h_mode = DS3231_getHourMode(i2c_number);
	I2C_Transmit(i2c_number,DS3231_ADDR,&hours,1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&hours,1,START_BIT|STOP_BIT);
    if (h_mode == CLOCK_H24)
	{
		return (bcd2bin(hours));
	}
	else if (h_mode == CLOCK_H12)
	{
		hours=bitClear(hours, 5);
		hours=bitClear(hours, 6);
		return (bcd2bin(hours));
	}
}

void  DS3231_setHours(uint8_t i2c_number,uint8_t hours)
{
	bool h_mode;
	uint8_t data[2];
	data[0]=0x02;
	printf("DS3231_setHours\n");
	if (hours >= 00 && hours <= 23)
	{
		h_mode = DS3231_getHourMode(i2c_number);
		if (h_mode == CLOCK_H24)
		{
			data[1]=bin2bcd(hours);
		}
		else if (h_mode == CLOCK_H12)
		{
			if (hours > 12)
			{
				hours = hours % 12;
				hours = bin2bcd(hours);
				hours=bitSet(hours, 6);
				hours=bitSet(hours, 5);
				data[1] = hours;
			}
			else
			{
				hours = bin2bcd(hours);
				hours=bitSet(hours, 6);
				hours=bitClear(hours, 5);
				data[1] = hours;
			}
		}
		I2C_Transmit(i2c_number,DS3231_ADDR,data,2,START_BIT|STOP_BIT);
	}
}


uint8_t DS3231_getWeek(uint8_t i2c_number)
{
	
	uint8_t week = 0x03;
	I2C_Transmit(i2c_number,DS3231_ADDR,&week,1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&week,1,START_BIT|STOP_BIT);
	return week;
}

void DS3231_setWeek(uint8_t i2c_number,uint8_t week)
{
	uint8_t data[2];
	data[0]= 0x03;
	data[1]= week;
	if (week >= 1 && week <= 7)
	{
		I2C_Transmit(i2c_number,DS3231_ADDR,data,2,START_BIT|STOP_BIT);
	}
}

void DS3231_updateWeek(uint8_t i2c_number)
{
	uint16_t y;
	uint8_t m, d, weekday;
	
	y=DS3231_getYear(i2c_number);
	m=DS3231_getMonth(i2c_number);
	d=DS3231_getDay(i2c_number);
	
	weekday  = (d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7;
	uint8_t data[2];
	data[0] = 0x03;
	data[1] = weekday;
	if (weekday >= 1 && weekday <= 7)
	{
		I2C_Transmit(i2c_number,DS3231_ADDR,data,2,START_BIT|STOP_BIT);
	}
}


uint8_t DS3231_getDay(uint8_t i2c_number)
{
	uint8_t day = 0x04;
	I2C_Transmit(i2c_number,DS3231_ADDR,&day,1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&day,1,START_BIT|STOP_BIT);
	return (bcd2bin(day));
}

void DS3231_setDay(uint8_t i2c_number,uint8_t day)
{
	uint8_t data[2];
	data[0] = 0x04;
	data[1] = bin2bcd(day);
	if (day >= 1 && day <= 31)
	{
		I2C_Transmit(i2c_number,DS3231_ADDR,data,2,START_BIT|STOP_BIT);
	}
}


uint8_t DS3231_getMonth(uint8_t i2c_number)
{
	uint8_t month;
	uint8_t data = 0x05;
	I2C_Transmit(i2c_number,DS3231_ADDR,&data,1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&month,1,START_BIT|STOP_BIT);
	month=bitClear(month,7);		//Clear Century Bit;
	return (bcd2bin(month));
}


void DS3231_setMonth(uint8_t i2c_number,uint8_t month)
{
	uint8_t data[2], century_bit;
	data[0] = 0x05;
	if (month >= 1 && month <= 12)
	{
		I2C_Transmit(i2c_number,DS3231_ADDR,&data[0],1,START_BIT|STOP_BIT);
		I2C_Recieve(i2c_number,DS3231_ADDR,&data[1],1,START_BIT|STOP_BIT);
		century_bit = bitRead(data[1], 7);
		month = bin2bcd(month);
		data[1]= month;
		I2C_Transmit(i2c_number,DS3231_ADDR,data,2,START_BIT|STOP_BIT);
	}
}


uint16_t DS3231_getYear(uint8_t i2c_number)
{
	uint8_t century_bit;
	uint16_t century,year;
	uint8_t data[2];
	data[0] = 0x05;
	// Read Month register for Century
	I2C_Transmit(i2c_number,DS3231_ADDR,&data[0],1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&data[1],1,START_BIT|STOP_BIT);
	century_bit = bitRead(data[1], 7);
	if(century_bit == 1)
	{
		century = 1900;
	}
	else if(century_bit == 0)
	{
		century = 2000;
	}
	data[0] = 0x06;
	//Read Year Register and add Century
	I2C_Transmit(i2c_number,DS3231_ADDR,&data[0],1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&year,1,START_BIT|STOP_BIT);
	year = bcd2bin(year) + century;
	return (year);
}

void DS3231_setYear(uint8_t i2c_number,uint16_t year)
{
	uint8_t century;

	if((year >= 00 && year <= 99) || (year >= 1900 && year <= 2099))
	{
		// If year is 2 digits set to 2000s.
		if(year >= 00 && year <= 99)
			year = year + 2000;

		//Century Calculation
		if(year >= 1900 && year <= 1999)
			century = 1;
		else if (year >= 2000 && year <= 2099)
			century = 0;

		// Find Century 
		year = year % 100;		//Converting to 2 Digit
		
		// Read Century bit from Month Register(0x05)
	uint8_t data[2];
	data[0] = 0x05;
	// Read Month register for Century
	I2C_Transmit(i2c_number,DS3231_ADDR,&data[0],1,START_BIT|STOP_BIT);
	I2C_Recieve(i2c_number,DS3231_ADDR,&data[1],1,START_BIT|STOP_BIT);
		
		// Set century bit to 1 for year > 2000;
		if(century == 1)
			data[1]=bitSet(data[1],7);
		else
			data[1]=bitClear(data[1],7);

		// Write Century bit to Month Register(0x05)
		I2C_Transmit(i2c_number,DS3231_ADDR,data,2,START_BIT|STOP_BIT);
		data[0] = 0x06;
		data[1] = bin2bcd(year);
		// Write 2 Digit year to Year Register(0x06)
		I2C_Transmit(i2c_number,DS3231_ADDR,data,2,START_BIT|STOP_BIT);
	}
}



void DS3231_setDateTime(uint8_t i2c_number,char* date, char* time)
{
	uint8_t day, month, hour, minute, second;
	uint16_t year;
	// sample input: date = "Dec 26 2009", time = "12:34:56"
	year = atoi(date + 9);
	DS3231_setYear(i2c_number,year);
	// Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
	switch (date[0]) {
		case 'J': month = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7); break;
		case 'F': month = 2; break;
		case 'A': month = date[2] == 'r' ? 4 : 8; break;
		case 'M': month = date[2] == 'r' ? 3 : 5; break;
		case 'S': month = 9; break;
		case 'O': month = 10; break;
		case 'N': month = 11; break;
		case 'D': month = 12; break;
		}
	DS3231_setMonth(i2c_number,month);
	day = atoi(date + 4);
	DS3231_setDay(i2c_number,day);
	hour = atoi(time);
	DS3231_setHours(i2c_number,hour);
	minute = atoi(time + 3);
	DS3231_setMinutes(i2c_number,minute);
	second = atoi(time + 6);
	DS3231_setSeconds(i2c_number,second);
}

//Changes made by Biancaa.R

// Returns seconds since 1970-01-01 00:00:00 UTC
uint32_t DS3231_getEpoch(uint8_t i2c_number) {
    uint16_t year = DS3231_getYear(i2c_number);
    uint8_t month = DS3231_getMonth(i2c_number); // 1-12
    uint8_t day = DS3231_getDay(i2c_number);     // 1-31
    uint8_t hour = DS3231_getHours(i2c_number);  // 0-23
    uint8_t minute = DS3231_getMinutes(i2c_number);
    uint8_t second = DS3231_getSeconds(i2c_number);

    // Unix time calculation (ignoring leap seconds)
    // Algorithm based on POSIX and C library conventions
    int y = year;
    int m = month;
    if (m < 3) {
        y--;
        m += 12;
    }
    uint32_t days = 365 * y + y / 4 - y / 100 + y / 400 + (153 * m - 457) / 5 + day - 306;
    uint32_t epoch = ((days * 24 + hour) * 60 + minute) * 60 + second;

    // Adjust for Unix epoch (days since 1970-01-01)
    // 719468 is the number of days from 0000-03-01 to 1970-01-01
    epoch -= 719468 * 24 * 60 * 60;

    return epoch;
	//This seems like the most confusing thing in the entire world
	//Screw this I am writing my own logic -> Not this georgian one.
}

//calc epoc seconds:
static const int days_in_month[12] = {
    31,28,31,30,31,30,31,31,30,31,30,31
};
int is_leap_year(int year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
	//zero or one depending on the logic
}
int days_since_epoch(int year, int month, int day) {
    int days = 0;

    // Add days for each year
    for (int y =1970; y <year; y++) {
        days += is_leap_year(y) ? 366 : 365;
    } //till the previous complete year
	//I am going crazy
    // Add days for each month in the current year
    for (int m = 1; m < month; m++) {
		//donot add for this half done hanging month
        if (m == 2 && is_leap_year(year))
            days += 29;
        else
            days += days_in_month[m - 1];
    }
    // Add days for the current month
    days += (day - 1);//Iniki add panna kudadhu
    return days;
}
uint32_t rtc_to_epoch(int year, int month, int day, int hour, int min, int sec) {
    int days = days_since_epoch(year, month, day);
    return (uint32_t)(days * 86400 + hour * 3600 + min * 60 + sec);
}

uint32_t DS3231_getEpoch_calc(uint8_t i2c_number) {
    uint16_t year = DS3231_getYear(i2c_number);
    uint8_t month = DS3231_getMonth(i2c_number); // 1-12
    uint8_t day = DS3231_getDay(i2c_number);     // 1-31
    uint8_t hour = DS3231_getHours(i2c_number);  // 0-23
    uint8_t minute = DS3231_getMinutes(i2c_number);
    uint8_t second = DS3231_getSeconds(i2c_number);
	//uint64_t days_count=days_since_epoch(year,month, day);
	uint32_t epoc=rtc_to_epoch(year,month, day, hour, minute, second);
	return epoc;
}