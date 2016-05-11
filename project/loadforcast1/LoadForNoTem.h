#ifndef LOADFOR_H
#define LOADFOR_H
#include<string>
#include <iostream>
#include<list>
#include"LoadDay.h"
#include "LoadForNoTem.h"
#include"MyDate.h"
using std::string;
using std::list;
using std::cout;
using std::endl;
class CLoadForNoTem
{
  private:
    list<CLoadDay> m_lLDsFuture,m_lLDsComFuture;
    list<CLoadDay> m_lLDsPast,m_lLDsToyear;
    CLoadDay m_iLoadDay;
    list<double> m_fPowerAscRate,m_fEnergyAscRate;

    static double m_fPowerRate;
    static double m_fEnergyRate;


public:

    CMyDate m_iFutureStart,m_iFutureEnd,m_iComFutureStart,m_iComFutureEnd;
    CMyDate m_iPastStart,m_iPastEnd,m_iToyearStart,m_iToyearEnd;

    CLoadForNoTem(){}

    CLoadForNoTem(string Start,string End);

    list<CLoadDay> GetLDsFuture(){return m_lLDsFuture;}
    list<CLoadDay> GetLDsComFuture(){return m_lLDsComFuture;}
    list<CLoadDay> GetLDsPast(){return m_lLDsPast;}
    list<CLoadDay> GetLDsToyear(){return m_lLDsToyear;}

    static double GetPowerRate(){return m_fPowerRate;}
    static double GetEnergyRate(){return m_fEnergyRate;}

    void CalAscRate();
    void CalFutureLoad();

    CMyDate CalComDay(CMyDate Date);



    void AddLDToComFuture(string Date,double MaxPower,double Energy,
                          string Type)
    {
        m_lLDsComFuture.push_back(CLoadDay(Date,MaxPower,Energy,Type));
    }

    void AddLDToFuture(string Date,double MaxPower,double Energy,
                       string Type)
    {
        m_lLDsFuture.push_back(CLoadDay(Date,MaxPower,Energy,Type));
    }

    void AddLDPast(string Date,double MaxPower,double Energy,
                   string Type)
    {
        m_lLDsPast.push_back(CLoadDay(Date,MaxPower,Energy,Type));
    }

    void AddLDToYear(string Date,double MaxPower,double Energy,
                     string Type)
    {
        m_lLDsToyear.push_back(CLoadDay(Date,MaxPower,Energy,Type));
    }

    void PrintLDs(list<CLoadDay> LoadDays);
};

#endif // LOADFOR_H
