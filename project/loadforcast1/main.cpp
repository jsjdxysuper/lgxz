#include <iostream>
#include<list>
#include "dmdb.h"
#include "inifile.h"
#include "LoadForNoTem.h"
#include"MyDate.h"


using namespace std;
#define BUFF    1024
int main(int argc, char *argv[])
{
    int i,j,ColumnNum;
    string usr,pwd,ip;


    CIniFile iniFile("./db.ini");
    usr = iniFile.GetValue("DB","usr");
    pwd = iniFile.GetValue("DB","pwd");
    ip = iniFile.GetValue("DB","ip");
    cout<<"usr:"<<usr<<" "<<"pwd:"<<pwd<<" "<<"ip:"<<ip<<endl;

    CLoadForNoTem LoadForNoTem("2013-09-01","2013-09-30");

    CDmDb dmdb(usr.c_str(),pwd.c_str(),ip.c_str(),"./log.txt");
    dmdb.init_database();


    char sql_str[BUFF];



    //get LDToyear
    cout<<"get LDToyear,start date:"<<LoadForNoTem.m_iToyearStart.GetStrDate().c_str()
       <<"end date:"<<LoadForNoTem.m_iToyearEnd.GetStrDate().c_str()<<endl;

    sprintf(sql_str,"select     A.srq, "
            "sdl, "
           " smax,"
           " stype  "
     "from  "
             "lnjndb.psidp.sjz A,lnjndb.psidp.datetype B "
    " where  "
         " A.srq=B.srq "
         " and   A.srq>='%s'  "
        " and A.srq<='%s' "
        " and A.sid = 'syllsyp_fhkh'",LoadForNoTem.m_iToyearStart.GetStrDate().c_str(),
            LoadForNoTem.m_iToyearEnd.GetStrDate().c_str());



    ColumnNum = 4;
    dmdb.exec_select(sql_str,ColumnNum,"Get ToyearLD");
    for(i=0;i<dmdb.get_row_num();i++)
    {
        LoadForNoTem.AddLDToYear(dmdb.get_select_data(i,0),
                                 atof(dmdb.get_select_data(i,1).c_str()),
                                 atof(dmdb.get_select_data(i,2).c_str()),
                                 dmdb.get_select_data(i,3));
    }
    LoadForNoTem.PrintLDs(LoadForNoTem.GetLDsToyear());
    cout<<endl<<endl<<endl;


    //get LDPast
    cout<<"get LDPast,start date:"<<LoadForNoTem.m_iPastStart.GetStrDate().c_str()
       <<"end date:"<<LoadForNoTem.m_iPastEnd.GetStrDate().c_str()<<endl;

    sprintf(sql_str,"select     A.srq, "
            "sdl, "
           " smax,"
           " stype  "
     "from  "
             "lnjndb.psidp.sjz A,lnjndb.psidp.datetype B "
    " where  "
         " A.srq=B.srq "
         " and   A.srq>='%s'  "
        " and A.srq<='%s' "
        " and A.sid = 'syllsyp_fhkh'",LoadForNoTem.m_iPastStart.GetStrDate().c_str(),
            LoadForNoTem.m_iPastEnd.GetStrDate().c_str());

    ColumnNum = 4;
    dmdb.exec_select(sql_str,ColumnNum,"get LDPast");
    for(i=0;i<dmdb.get_row_num();i++)
    {
        LoadForNoTem.AddLDPast(dmdb.get_select_data(i,0),
                                 atof(dmdb.get_select_data(i,1).c_str()),
                                 atof(dmdb.get_select_data(i,2).c_str()),
                                 dmdb.get_select_data(i,3));
    }
    LoadForNoTem.PrintLDs(LoadForNoTem.GetLDsPast());
    cout<<endl<<endl<<endl;


    //get LDComFuture
    cout<<"get LDComFuture,start date:"<<LoadForNoTem.m_iComFutureStart.GetStrDate().c_str()
       <<"end date:"<<LoadForNoTem.m_iComFutureEnd.GetStrDate().c_str()<<endl;

    sprintf(sql_str,"select     A.srq, "
            "sdl, "
           " smax,"
           " stype  "
     "from  "
             "lnjndb.psidp.sjz A,lnjndb.psidp.datetype B "
    " where  "
         " A.srq=B.srq "
         " and   A.srq>='%s'  "
        " and A.srq<='%s' "
        " and A.sid = 'syllsyp_fhkh'",LoadForNoTem.m_iComFutureStart.GetStrDate().c_str(),
            LoadForNoTem.m_iComFutureEnd.GetStrDate().c_str());

    ColumnNum = 4;
    dmdb.exec_select(sql_str,ColumnNum,"get LDComFuture");
    for(i=0;i<dmdb.get_row_num();i++)
    {
        LoadForNoTem.AddLDToComFuture(dmdb.get_select_data(i,0),
                                 atof(dmdb.get_select_data(i,1).c_str()),
                                 atof(dmdb.get_select_data(i,2).c_str()),
                                 dmdb.get_select_data(i,3));
    }
    LoadForNoTem.PrintLDs(LoadForNoTem.GetLDsComFuture());
    cout<<endl<<endl<<endl;



    //calculate Average acsent rate
    LoadForNoTem.CalAscRate();
    cout<<"PowerRate:"<<LoadForNoTem.GetPowerRate()<<"  "
       <<"EnergyRate:"<<LoadForNoTem.GetEnergyRate()<<endl;


    //get LDFuture
    cout<<"get LDComFuture,start date:"<<LoadForNoTem.m_iComFutureStart.GetStrDate().c_str()
       <<"end date:"<<LoadForNoTem.m_iComFutureEnd.GetStrDate().c_str()<<endl;

    sprintf(sql_str,"select     A.srq, "
            "sdl, "
           " smax,"
           " stype  "
     "from  "
             "lnjndb.psidp.sjz A,lnjndb.psidp.datetype B "
    " where  "
         " A.srq=B.srq "
         " and   A.srq>='%s'  "
        " and A.srq<='%s' "
        " and A.sid = 'syllsyp_fhkh'",LoadForNoTem.m_iFutureStart.GetStrDate().c_str(),
            LoadForNoTem.m_iFutureEnd.GetStrDate().c_str());

    ColumnNum = 4;
    dmdb.exec_select(sql_str,ColumnNum,"get LDComFuture");
    for(i=0;i<dmdb.get_row_num();i++)
    {
        LoadForNoTem.AddLDToFuture(dmdb.get_select_data(i,0),
                                 atof(dmdb.get_select_data(i,1).c_str()),
                                 atof(dmdb.get_select_data(i,2).c_str()),
                                 dmdb.get_select_data(i,3));
    }
    LoadForNoTem.PrintLDs(LoadForNoTem.GetLDsFuture());
    cout<<endl<<endl<<endl;



    return 0;
}

