#ifndef LOADDAY_H
#define LOADDAY_H
#include"MyDate.h"
class CLoadDay
{
private:
    CMyDate m_iDate;
    double m_fMaxPower, m_fEnergy;
    string m_cType;
public:
    CLoadDay(){}
    CLoadDay(string Date,double MaxPower,double Energy,string Type):
        m_iDate(Date),m_fMaxPower(MaxPower),m_fEnergy(Energy),m_cType(Type)
    {}
    double GetMaxPower(){return m_fMaxPower;}
    double GetEnergy(){return m_fEnergy;}
    string GetType(){return m_cType;}
    CMyDate GetDate(){return m_iDate;}

    void SetMaxPower(double MaxPower){this->m_fMaxPower = MaxPower;}
    void SetEnergy(double Energy){this->m_fEnergy = Energy;}
    void SetType(string Type){this->m_cType = Type;}
    void SetDate(CMyDate Date){this->m_iDate = Date;}
};
#endif // LOADDAY_H
