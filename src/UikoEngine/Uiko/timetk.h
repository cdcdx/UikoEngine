#pragma once
#ifndef __timetk_h__
#define __timetk_h__

#include <time.h>

//this is a class based on second, that can handle time operater
class timetk
{
public:
    timetk();                                                               //use current time
    timetk(time_t t);                                                       //use time_t time
    timetk(int year, int month, int day, int hour, int minute, int second); //use specific time
    virtual ~timetk();

    static timetk get_current_time();

private:
    void time_to_tm();

public:                         //for get value
    time_t get_time() const;    //get time_t value
    int get_year() const;       //get year
    int get_month() const;      //get month
    int get_day() const;        //get day
    int get_hour() const;       //get hour
    int get_minute() const;     //get minute
    int get_second() const;     //get second
    int get_week_day() const;   //get the current day of week(from 0 to 6)
    int get_month_day() const;  //get the last days of current month
    int get_year_day() const;   //get the last days of current year(from 1.1)

    //for operator
    timetk& operator=(const timetk& timesrc);
    timetk& operator=(const time_t& timesrc);
    timetk operator+(const timetk& tk) const;
    timetk operator+(const time_t t) const;
    timetk operator-(const timetk& tk) const;
    timetk operator-(const time_t t) const;
    timetk& operator+=(const timetk& tk);
    timetk& operator+=(const time_t t);
    timetk& operator-=(const timetk& tk);
    timetk& operator-=(const time_t t);
    int operator==(const timetk& tk) const;
    int operator!=(const timetk& tk) const;
    int operator>(const timetk& tk) const;
    int operator>=(const timetk& tk) const;
    int operator<(const timetk& tk) const;
    int operator<=(const timetk& tk) const;
    
protected:
    time_t t_;
    tm tm_;
};

#endif // __timetk_h__
