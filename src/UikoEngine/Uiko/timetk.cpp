#include "timetk.h"
#include <memory.h>
#include <assert.h>

#ifdef _WIN32
#   include <sys/timeb.h>
#   include <time.h>
#else
#   include <sys/time.h>
#endif

timetk::timetk()
{
    t_ = time(NULL);
    memset(&tm_, 0, sizeof(tm));
    time_to_tm();
}

timetk::timetk(time_t t)
{
    t_ = t;
    memset(&tm_, 0, sizeof(tm));
    time_to_tm();
}

timetk::timetk(int year, int month, int day, int hour, int minute, int second)
{
    t_ = 0;
    memset(&tm_, 0, sizeof(tm));
    tm ltm = {0};
    /* seconds after the minute - [0,59] */
    /* minutes after the hour - [0,59] */
    /* hours since midnight - [0,23] */
    /* day of the month - [1,31] */
    /* months since January - [0,11] */
    /* years since 1900 */
    if (second < 0 || second > 59)
    {
        second = 0;
    }
    if (minute < 0 || minute > 59)
    {
        minute = 0;
    }
    if (hour < 0 || hour > 23)
    {
        hour = 0;
    }
    ltm.tm_sec  = second;
    ltm.tm_min  = minute;
    ltm.tm_hour = hour;
    assert(day >= 1 && day <= 31);
    ltm.tm_mday = day;
    assert(month >= 1 && month <= 12);
    ltm.tm_mon  = month - 1;
    assert(year >= 1900);
    ltm.tm_year = year - 1900;
    if (mktime(&ltm) != ((time_t) - 1))
    {
        t_ = mktime(&ltm);
        time_to_tm();
    }
}

timetk::~timetk()
{
}

timetk timetk::get_current_time()
{
    return timetk(time(NULL));
}

void timetk::time_to_tm()
{
#ifdef _WIN32
    //tm* ltm = localtime(&t_);
	tm* ltm = { 0 };
	localtime_s(ltm, &t_);
#else
    tm rtm;
    tm* ltm = localtime_r(&t_, &rtm);
#endif
    if (ltm != NULL)
    {
        tm_ = *ltm;
    }
}

time_t timetk::get_time() const
{
    return t_;
}

int timetk::get_year() const
{
    return tm_.tm_year + 1900; //it's year since 1900
}

int timetk::get_month() const
{
    return tm_.tm_mon + 1; //it's mon base on 0
}

int timetk::get_day() const
{
    return tm_.tm_mday;
}

int timetk::get_hour() const
{
    return tm_.tm_hour;
}

int timetk::get_minute() const
{
    return tm_.tm_min;
}

int timetk::get_second() const
{
    return tm_.tm_sec;
}

int timetk::get_week_day() const
{
    return tm_.tm_wday;
}

int timetk::get_month_day() const
{
    return tm_.tm_mday;
}

int timetk::get_year_day() const
{
    return tm_.tm_yday + 1;
}

timetk& timetk::operator=(const timetk& timesrc)
{
    if (this == &timesrc)
        return *this;

    t_ = timesrc.t_;
    time_to_tm();
    return *this;
}

timetk& timetk::operator=(const time_t& timesrc)
{
    t_ = timesrc;
    time_to_tm();
    return *this;
}

timetk timetk::operator+(const timetk& tk) const
{
    return timetk(t_ + tk.t_);
}

timetk timetk::operator+(const time_t t) const
{
    return timetk(t_ + t);
}

timetk timetk::operator-(const timetk& tk) const
{
    return timetk(t_ - tk.t_);
}

timetk timetk::operator-(const time_t t) const
{
    return timetk(t_ - t);
}

timetk& timetk::operator+=(const timetk& tk)
{
    t_ += tk.t_;
    time_to_tm();
    return *this;
}

timetk& timetk::operator+=(const time_t t)
{
    t_ += t;
    time_to_tm();
    return *this;
}

timetk& timetk::operator-=(const timetk& tk)
{
    t_ -= tk.t_;
    time_to_tm();
    return *this;
}

timetk& timetk::operator-=(const time_t t)
{
    t_ -= t;
    time_to_tm();
    return *this;
}

int timetk::operator==(const timetk& tk) const
{
    return (t_ == tk.t_);
}

int timetk::operator!=(const timetk& tk) const
{
    return(t_ != tk.t_);
}

int timetk::operator>(const timetk& tk) const
{
    return (t_ > tk.t_);
}

int timetk::operator>=(const timetk& tk) const
{
    return (t_ >= tk.t_);
}

int timetk::operator<(const timetk& tk) const
{
    return (t_ < tk.t_);
}

int timetk::operator<=(const timetk& tk) const
{
    return (t_ <= tk.t_);
}
