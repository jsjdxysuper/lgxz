#ifndef INIFILE_H
#define INIFILE_H
#include<string>
#include<stdio.h>
using namespace std;
using std::string;

#define 	M_MAX_BUFFER_SIZE           (6*1024)
#define 	M_MAX_INTVAL_BUFFER_SIZE    32
#define 	M_MAX_SECTION_SIZE          32
#define DBGPRINT(string, args...)
#define MAXBUFFERSIZE 1024
typedef struct{
    FILE*   p_inifile;        //
    long    i_filesize;       //

    long    i_filebuffer_size; //
    char    sz_filebufstatic[M_MAX_BUFFER_SIZE]; //
    char    *sz_filebuffer;     //
    int     b_bufferchanged;                      // if 1, save file when close

    char    sz_lastsection[M_MAX_SECTION_SIZE];   // previous section name
    int     b_sectionfound;
    long    i_sc_startpos;                        // start/end position of the
    long    i_sc_endpos;                          // following lines of section

    long    i_value_startpos;                     // start/end position of key
    long    i_value_endpos;                       // value
    unsigned char u_mode;                          //mode == 0 ֻ  mode > 0 д
}IniFileData_TS;

class CIniFile{
private:
    void *fpini;
    string  m_cPath;

public:

    CIniFile(){}
    CIniFile(string path):m_cPath(path){}

    string GetPath(){return m_cPath;}
    void    setPath(string path){m_cPath = path;}



    string GetValue(string domain, string key)
    {
        char cValue[MAXBUFFERSIZE];

        fpini = IniOpenFile(m_cPath.c_str(), 1);
        if(fpini == NULL)
        {
            char buffer[512];
            printf("%s",buffer);
            printf("read ini file failed!\n");
            return NULL;
        }

        IniGetString(domain.c_str(), key.c_str(), cValue, MAXBUFFERSIZE, fpini);
        printf("ip:%s\n", cValue);

        return string(cValue);
    }
    //mode == 0 ֻ  mode > 0 д
    void    *IniOpenFile(const char * psz_file, const unsigned char mode);
    int     IniCleanFile(void *fpini);
    int     IniCloseFile(void *fpini);
    int     IniFileSizeGet(void *fpini);

    int     IniGetString(const char * psz_section, const char * psz_key, char * psz_value, int value_size, void *fpini);
    int     IniSetString(const char * psz_section, const char * psz_key, const char * psz_value, void *fpini);

protected:
    int IniInit(IniFileData_TS *f_inifile);
    int IniFileBufferModify(long size, IniFileData_TS *f_inifile);
    int IniSearchContentStart(const long i_position, IniFileData_TS *f_inifile);
    int IniSearchContentEnd(const long i_startpos, IniFileData_TS *f_inifile);
    int IniSearchSection(const char *psz_section, IniFileData_TS *f_inifile);
};



#endif // INIFILE_H
