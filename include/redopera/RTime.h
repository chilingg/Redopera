#ifndef RTIME_H
#define RTIME_H

#include <chrono>
#include <ctime>
#include <string>

namespace Redopera {

class RTime // 秒级年月日时钟
{
public:
    enum TimeZone {
        UniversalTime,
        LocatlTime
    };

    //1970.1.1至今的duration
    static std::time_t timeSinceEpoch() { return std::time(nullptr); }

    RTime(TimeZone zone = LocatlTime):
        transformation(zone == LocatlTime ? std::localtime : std::gmtime),
        format("%Y-%m-%d %H:%M:%S")
    { update(); }

    void update()
    {
        auto t = std::time(nullptr);
        time_ = *(transformation(&t));
    }

    int yearDay() const { return time_.tm_yday +1; }
    int year() const { return time_.tm_year + 1900; }
    int month() const { return time_.tm_mon + 1; }
    int week() const { return time_.tm_wday + 1; }
    int day() const { return time_.tm_mday; }
    int hour() const { return time_.tm_hour; }
    int minute() const { return time_.tm_min; }
    int second() const { return time_.tm_sec; }

    const std::tm& time() const { return time_; }

    /* 时间转换为字符串时指定的格式
     * %a 星期几的简写
     * %A 星期几的全称
     * %b 月份的简写
     * %B 月份的全称
     * %c 标准的日期的时间串
     * %C 年份的前两位数字
     * %d 十进制表示的每月的第几天
     * %D 月/天/年
     * %e 在两字符域中，十进制表示的每月的第几天
     * %F 年-月-日
     * %g 年份的后两位数字，使用基于周的年
     * %G 年份，使用基于周的年
     * %h 简写的月份名
     * %H 24小时制的小时
     * %I 12小时制的小时
     * %j 十进制表示的每年的第几天
     * %m 十进制表示的月份
     * %M 十时制表示的分钟数
     * %n 新行符
     * %p 本地的AM或PM的等价显示
     * %r 12小时的时间
     * %R 显示小时和分钟：hh:mm
     * %S 十进制的秒数
     * %t 水平制表符
     * %T 显示时分秒：hh:mm:ss
     * %u 每周的第几天，星期一为第一天 （值从1到7，星期一为1）
     * %U 第年的第几周，把星期日作为第一天（值从0到53）
     * %V 每年的第几周，使用基于周的年
     * %w 十进制表示的星期几（值从0到6，星期天为0）
     * %W 每年的第几周，把星期一做为第一天（值从0到53）
     * %x 标准的日期串
     * %X 标准的时间串
     * %y 不带世纪的十进制年份（值从0到99）
     * %Y 带世纪部分的十制年份
     * %z，%Z 时区名称，如果不能得到时区名称则返回空字符。
     * %% 百分号
     */
    std::string toString() const
    {
        static char buffer[64] {0};
        std::strftime(buffer, sizeof(buffer), format.c_str(), &time_);
        return buffer;
    }

    void setTimeFormat(const std::string &fmt = "")
    {
        static const std::string DEFAULT_FORMAT = "%Y-%m-%d %H:%M:%S";
        format = fmt.empty() ? DEFAULT_FORMAT : fmt;
    }

private:
    std::tm* (*transformation)(const std::time_t *clock);
    std::string format;
    std::tm time_;

};

} // Redopera

#endif // RTIME_H
