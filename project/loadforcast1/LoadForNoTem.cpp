#include<stdlib.h>
#include<math.h>
#include<iostream>
#include<list>
#include"LoadForNoTem.h"
using std::string;
using std::cout;
using std::endl;
using std::list;
double CLoadForNoTem::m_fPowerRate = 0.0;
double CLoadForNoTem::m_fEnergyRate = 0.0;


CLoadForNoTem::CLoadForNoTem(string Start,string End)
{
    CMyDate s(Start),e(End);

    int DayNum = CMyDate::CalDifference(s,e);

    m_iFutureStart = s;
    m_iFutureEnd = e;

    m_iToyearEnd = CMyDate::AddDayNum(m_iFutureStart, -1);
    m_iToyearStart = CMyDate::AddDayNum(m_iToyearEnd, -120);

    m_iPastStart = CalComDay(m_iToyearStart);
    m_iPastEnd = CMyDate::AddDayNum(m_iPastStart, 120);

    m_iComFutureStart = CalComDay(m_iFutureStart);
    m_iComFutureEnd = CMyDate::AddDayNum(m_iComFutureStart, DayNum);

}

CMyDate CLoadForNoTem::CalComDay(CMyDate Date)
{
    int WeekSeq,WeekDay;
    CMyDate ret;

    WeekDay = CMyDate::CalWeekDay(Date);
    WeekSeq = CMyDate::CalWeekSeq(Date);
    ret = CMyDate::CalDateFromWeekDay(Date.GetIntYear()-1,WeekSeq,WeekDay);
    return ret;
}
void CLoadForNoTem::PrintLDs(list<CLoadDay> LoadDays)
{
    list<CLoadDay>::iterator iter,End;
    iter = LoadDays.begin();
    End = LoadDays.end();

    for(;iter!=End;iter++)
    {
        cout<<"Date:"<<(*iter).GetDate().GetStrDate().c_str()<<"  "
           <<"MaxPower:"<<(*iter).GetMaxPower()<<"  "
          <<"Energy:"<<(*iter).GetEnergy()<<"  "
            <<"Type:"<<(*iter).GetType()<<endl;
    }
}

void CLoadForNoTem::CalAscRate()
{
    int Num = 0;
    double EnergyRateSum = 0,PowerRateSum = 0;
    list<CLoadDay>::iterator PastIter = m_lLDsPast.begin();
    list<CLoadDay>::iterator PastIterEnd = m_lLDsPast.end();
    list<CLoadDay>::iterator ToyearIter = m_lLDsToyear.begin();
    list<CLoadDay>::iterator ToyearIterEnd = m_lLDsToyear.end();

    if(m_lLDsPast.size()!=m_lLDsToyear.size())
    {
        cout<<"CalPowerRate Error because of size"<<endl;
        exit(1);
    }
    for(; PastIter!=PastIterEnd; PastIter++,ToyearIter++)
    {

        if((*PastIter).GetType() == "00" || (*ToyearIter).GetType() == "00")
        {
            Num++;
            PowerRateSum += (*ToyearIter).GetMaxPower()/(*PastIter).GetMaxPower();
            EnergyRateSum += (*ToyearIter).GetEnergy()/(*PastIter).GetEnergy();
        }
    }
    CLoadForNoTem::m_fPowerRate = PowerRateSum/Num;
    CLoadForNoTem::m_fEnergyRate = EnergyRateSum/Num;

}
////计算三个时间段
//void CLoadForNoTem::CalDates()
//{

//}
void CLoadForNoTem::CalFutureLoad()
{




}
