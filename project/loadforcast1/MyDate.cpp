#include<stdlib.h>
#include<math.h>
#include<iostream>
#include<string>
#include<stdio.h>
#include"MyDate.h"
using std::string;
using std::cout;
using std::endl;

CMyDate::CMyDate(string Date):m_cDate(Date)
{
    m_nWeekDay = CalWeekDay(m_cDate);
    m_nWeekSeq = CalWeekSeq(m_cDate);
    m_nYear = atoi(m_cDate.substr(0,4).c_str());
    m_nMonth = atoi(m_cDate.substr(5,2).c_str());
    m_nDay = atoi(m_cDate.substr(8,2).c_str());
}
//calculate the day of week in Date
int CMyDate::CalWeekDay(string Date)
{
    int ret;
    int year,month,day;

    year = atoi(Date.substr(0,4).c_str());
    month = atoi(Date.substr(5,2).c_str());
    day = atoi(Date.substr(8,2).c_str());
    if(1 == month|| 2 == month)
    {
        month +=12;
        year--;
    }

    ret = (day+1+2*month+3*(month+1)/5+year+year/4-year/100+year/400)%7;
    if(0 == ret)
        ret = 7;
    return ret;
}
int CMyDate::CalWeekDay(CMyDate Date)
{
   return CalWeekDay(Date.GetStrDate());
}

//Year is a leap or not
bool CMyDate::IsLeapYear(int Year)
{
    if(0 == Year%4 || (0 == Year%100&& 0 == Year%400))
        return true;
    else
        return false;
}

//Calculate the days passed before Date
int CMyDate::CalDayNum(string Date)
{
    int MonthDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    int ret = 0, i;
    int Year = atoi(Date.substr(0,4).c_str());
    int Month = atoi(Date.substr(5,2).c_str());
    int Day = atoi(Date.substr(8,2).c_str());
    if(true == IsLeapYear(Year))
        MonthDays[2] = 29;

    for(i=0; i<Month; i++)
        ret += MonthDays[i];
    ret += Day;
    return ret;

}

static int CalDayNum(CMyDate Date)
{
    return CMyDate::CalDayNum(CMyDate::GetStrDate(Date));
}

// Calculate the first day weekday of Date
int CMyDate::CalFirstWeekDay(int Year)
{
    char cCharDate[15];
    sprintf(cCharDate,"%d-01-01",Year);
    int FirstWeekDay = CalWeekDay(string(cCharDate));
    return FirstWeekDay;
}


//Calculate: how many days in the first week of the year of Date
int CMyDate::CalFirstWeekDayNum(int Year)
{
    int FirstWeekDay,FirstWeekDayNum;
    FirstWeekDay = CalFirstWeekDay(Year);
    FirstWeekDayNum = 7-FirstWeekDay+1;
    return FirstWeekDayNum;
}


//the sequence of the week of Date
//from the begining the first day of year of Date
int CMyDate::CalWeekSeq(string Date)
{
    int Week,DayNum,FirstWeekDayNum;

    DayNum = CalDayNum(Date);
   // cout<<"DayNum:"<<DayNum<<endl;

    FirstWeekDayNum = CalFirstWeekDayNum(CMyDate::GetYear(Date));
    //cout<<"FirstWeekDayNum:"<<FirstWeekDayNum<<endl;

    if(DayNum<=7)
        Week = 1;
    else
        Week = ceil(double(DayNum-FirstWeekDayNum)/7)+1;
    return Week;
}

int CMyDate::CalWeekSeq(CMyDate Date)
{
    return CMyDate::CalWeekSeq(Date.GetStrDate());
}

//calculate the date of the DayNumth fo year
string CMyDate::CalDateFromDayNum(int Year,int DayNum)
{
    int MonthDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    int i;
    char cCharDate[15];

    int Month = 0;
    int Day = 0;
    if(true == IsLeapYear(Year))
        MonthDays[2] = 29;

    for(i=0;;i++)
    {

        if(DayNum-MonthDays[i] > 0)
        {
            Month++;
            DayNum -= MonthDays[i];
        }
        else
            break;
    }
    Day = DayNum;
    sprintf(cCharDate,"%4d-%02d-%02d",Year,Month,Day);
    return string(cCharDate);
}

CMyDate CMyDate::CalDateFromWeekDay(int Year, int WeekSeq, int WeekDay)
{

    int FirstWeekDayNum = CalFirstWeekDayNum(Year);
    int FirstWeekDay = CalFirstWeekDay(Year);
    int Days;
    CMyDate FirstDayDate(Year,1,1),RetDate(1900,1,1);
    if(WeekDay < 1 || WeekDay > 7)
    {
         cout<<"error in CalDateFromWeekDay, because of WeekDay"<<endl;
         exit(1);
    }
    if(1 == WeekSeq)
    {
        if(FirstWeekDay>WeekDay)
        {
            cout<<"error:Your WeekDay is incorrect!"<<endl;
            exit(1);
        }
        Days = WeekDay-FirstWeekDay;
        RetDate = CMyDate::AddDayNum(FirstDayDate,Days);
    }
    else if(2 <= WeekSeq)
    {
        Days = FirstWeekDayNum + 7*(WeekSeq-2) + WeekDay;
        RetDate = CMyDate::AddDayNum(FirstDayDate,Days-1);
    }
    else
    {
        cout<<"error in CalDateFromWeekDay, because of WeekSeq"<<endl;
        exit(1);
    }
    return RetDate;

}

//Calculate the difference between the two dates
int CMyDate::CalDifference(const CMyDate& a,const CMyDate& b)
{
    CMyDate MaxDate,MinDate;
    int MonthDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    int MaxYear,MinYear,MaxMonth,MinMonth,MaxDay,MinDay;
    int ret;
    if(a<b)
    {
        MaxDate = b;
        MinDate = a;
    }
    else
    {
        MaxDate = a;
        MinDate = b;
    }
    MaxYear = MaxDate.GetIntYear();
    MinYear = MinDate.GetIntYear();
    MaxMonth = MaxDate.GetIntMonth();
    MinMonth = MinDate.GetIntMonth();
    MaxDay = MaxDate.GetIntDay();
    MinDay = MinDate.GetIntDay();


    if(MaxYear == MinYear)
    {
        int EndDay = CalDayNum(GetStrDate(MaxDate));
        int BeginDay = CalDayNum(GetStrDate(MinDate));
        ret = EndDay - BeginDay;
        return ret;
    }
    else
    {
        ret = 0;
        int DiffYear = MaxYear - MinYear;


        int YearDays = 365;
        MonthDays[2] =28;
        if(IsLeapYear(MinYear))
        {
            MonthDays[2] =29;
            YearDays = 366;
        }
        ret +=YearDays-CalDayNum(GetStrDate(MinDate) );

        if(1 == DiffYear)
        {
            ret += CalDayNum(GetStrDate(MaxDate) );
            return ret;
        }
        else
        {
            int i;
            for(i=MinYear+1;i<MaxYear;i++)
            {
                YearDays = IsLeapYear(i)? 366:365;
                ret += YearDays;
            }
            ret+=CalDayNum(GetStrDate(MaxDate) );
            return ret;
        }

    }
}

CMyDate CMyDate::AddDayNum(string Date, int Num)
{
    return AddDayNum(CMyDate(Date),Num);
}
//Calculate the Date which is equal to Date+Num
CMyDate  CMyDate::AddDayNum(CMyDate Date, int Num)
{

    int DayNum = CalDayNum(CMyDate::GetStrDate(Date)) + Num;
    int Year,YearDays, Month=0, Day=0, i;
    int MonthDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    char cCharDate[15];

    Year = Date.GetYear(Date);
    if(Num >= 0){
        while(true)
        {
            if(IsLeapYear(Year))
                YearDays=366;
            else
                YearDays=365;

            if(0 == DayNum/YearDays)
                break;
            else
            {
                Year++;
                DayNum -=YearDays;
            }
        }

        if(IsLeapYear(Year))
           MonthDays[2] = 29;
        else
            MonthDays[2] = 28;

        for(i=0;;i++)
        {
            if(DayNum-MonthDays[i] > 0)
            {
                Month++;
                DayNum -= MonthDays[i];
            }
            else
                break;
        }
        Day = DayNum;
    }//end for if
    else
    {
        while(true)
        {
            if(DayNum >= 0)
                break;
            else
            {
                Year--;

                if(IsLeapYear(Year))
                    YearDays=366;
                else
                    YearDays=365;
                DayNum +=YearDays;
            }
        }

        if(IsLeapYear(Year))
            MonthDays[2] = 29;
        else
            MonthDays[2] = 28;

        for(i=0;;i++)
        {
            if(DayNum-MonthDays[i] > 0)
            {
                Month++;
                DayNum -= MonthDays[i];
            }
            else
                break;
        }
        Day = DayNum;
    }

    sprintf(cCharDate,"%d-%02d-%02d",Year,Month,Day);

    return CMyDate(string(cCharDate));
}

