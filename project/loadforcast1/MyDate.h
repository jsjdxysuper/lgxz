#ifndef MYDATE_H
#define MYDATE_H
#include<string>
#include<stdio.h>
using std::string;
class CMyDate
{
private:
  string m_cDate;
  int m_nYear,m_nMonth,m_nDay;
  int m_nWeekSeq;
  int m_nWeekDay;


public:
  CMyDate()
  {
      CMyDate(1900,1,1);
  }

  explicit CMyDate(string Date);

  CMyDate(const CMyDate & Date):m_cDate(Date.m_cDate),m_nYear(Date.m_nYear),
      m_nMonth(Date.m_nMonth),m_nDay(Date.m_nDay),
      m_nWeekSeq(m_nWeekSeq),m_nWeekDay(m_nWeekDay)
  {}

  CMyDate(int Year, int Month, int Day):
      m_nYear(Year),m_nMonth(Month),m_nDay(Day)
  {
      char cCharDate[15];
      sprintf(cCharDate,"%4d-%02d-%02d",m_nYear,m_nMonth,m_nDay);
      m_cDate = string(cCharDate);
      m_nWeekDay = CalWeekDay(m_cDate);
      m_nWeekSeq = CalWeekSeq(m_cDate);
  }


  static int CalWeekDay(string Date);
  static int CalWeekDay(CMyDate Date);

  static bool IsLeapYear(int Year);

  static int CalDayNum(string Date);
  static int CalDayNum(CMyDate Date);

  static int CalFirstWeekDay(int Year);
  static int CalFirstWeekDayNum(int Year);

  static int CalWeekSeq(string Date);
  static int CalWeekSeq(CMyDate Date);

  static string CalDateFromDayNum(int Year,int DayNum);
  static CMyDate CalDateFromWeekDay(int Year,int WeekSeq, int WeekDay);
  static int CalDifference(const CMyDate& a,const CMyDate& b);

  int GetIntYear(){return this->m_nYear;}
  int GetIntMonth(){return this->m_nMonth;}
  int GetIntDay(){return this->m_nDay;}

  string GetStrDate(){return this->m_cDate;}
  static string GetStrDate(const CMyDate& Date){return Date.m_cDate;}
  static int GetYear(CMyDate Date){return Date.m_nYear;}
  static int GetMonth(CMyDate Date){return Date.m_nMonth;}
  static int GetDay(CMyDate Date){return Date.m_nDay;}


  static int GetYear(string Date)
  {
      int Year = atoi(Date.substr(0,4).c_str());
      return Year;
  }

  static int GetMonth(string Date)
  {
      int Month = atoi(Date.substr(5,2).c_str());
      return Month;
  }

  static int GetDay(string Date)
  {
      int Day = atoi(Date.substr(8,2).c_str());
      return Day;
  }



 static CMyDate AddDayNum(CMyDate Date, int Num);
 static CMyDate AddDayNum(string Date, int Num);

 static int Compare(const CMyDate& a, const CMyDate& b)
 {
     string a1 = CMyDate::GetStrDate(a),b1 = CMyDate::GetStrDate(b);
     return a1.compare(b1);
 }
// static CMyDate CalDateFromWeekDay(int Year, int WeekSeq, int WeekDay);

friend bool operator ==(const CMyDate& a, const CMyDate& b);
friend bool operator <(const CMyDate& a, const CMyDate& b);
friend bool operator >(const CMyDate& a, const CMyDate& b);
friend bool operator <=(const CMyDate& a, const CMyDate& b);
friend bool operator >=(const CMyDate& a, const CMyDate& b);





};

inline bool operator ==(const CMyDate& a, const CMyDate& b)
{
    if(0 == CMyDate::GetStrDate(a).compare(CMyDate::GetStrDate(b)) )
        return true;
    else
        return false;
}
inline bool operator <(const CMyDate& a, const CMyDate& b)
{
    if(0 > CMyDate::GetStrDate(a).compare(CMyDate::GetStrDate(b)) )
        return true;
    else
        return false;
}
inline bool operator >(const CMyDate& a, const CMyDate& b)
{
    if(0 < CMyDate::GetStrDate(a).compare(CMyDate::GetStrDate(b)))
        return true;
    else
        return false;
}
inline bool operator <=(const CMyDate& a, const CMyDate& b)
{
    if(0 > CMyDate::GetStrDate(a).compare(CMyDate::GetStrDate(b)) ||
           0 == CMyDate::GetStrDate(a).compare(CMyDate::GetStrDate(b)))
        return true;
    else
        return false;
}
inline bool operator >=(const CMyDate& a, const CMyDate& b)
{
    if(0 < CMyDate::GetStrDate(a).compare(CMyDate::GetStrDate(b)) ||
           0 == CMyDate::GetStrDate(a).compare(CMyDate::GetStrDate(b)))
        return true;
    else
        return false;
}
#endif // MYDATE_H
