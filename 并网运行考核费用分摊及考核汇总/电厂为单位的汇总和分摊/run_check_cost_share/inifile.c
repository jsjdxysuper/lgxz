/*******************************************************************************
 * class for reading Ini File on Unix/Linux/Window
*******************************************************************************/
//*************************  begin of -- include  ******************************
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include "inifile.h"
#include <unistd.h>

//*************************  **end of -- include  ******************************

//*************************  begin of -- macro definitions  ********************
#define 	M_MAX_BUFFER_SIZE           (6*1024)
#define 	M_MAX_INTVAL_BUFFER_SIZE    32
#define 	M_MAX_SECTION_SIZE          32
#define DBGPRINT(string, args...)
//#define DBGPRINT(string, args...) printf(string, ##args)
//*************************  **end of -- macro definitions  ********************

//*************************  begin of -- type definitions  *********************
typedef struct{
    FILE*   p_inifile;        //文件句柄
    long    i_filesize;       //文件大小

    long    i_filebuffer_size; //文件缓存的大小
    char    sz_filebufstatic[M_MAX_BUFFER_SIZE]; //静态文件缓存
    char    *sz_filebuffer;     //文件缓存,超过静态缓存则动态分配
    int     b_bufferchanged;                      // if 1, save file when close

    char    sz_lastsection[M_MAX_SECTION_SIZE];   // previous section name
    int     b_sectionfound;
    long    i_sc_startpos;                        // start/end position of the
    long    i_sc_endpos;                          // following lines of section

    long    i_value_startpos;                     // start/end position of key
    long    i_value_endpos;                       // value
    unsigned char u_mode;                          //mode == 0 只读  mode > 0 读写
}IniFileData_TS;
//*************************  **end of -- type definitions  *********************

/*******************************************************************************
 *   desc: initialize member vars
 *------------------------------------------------------------------------------
 *  param: none
 *------------------------------------------------------------------------------
 * return: 0   -- ok
*******************************************************************************/
static int IniInit(IniFileData_TS *f_inifile)
{
    f_inifile->p_inifile       = NULL;
    f_inifile->i_filesize      = 0;

    f_inifile->i_filebuffer_size  = M_MAX_BUFFER_SIZE;
    f_inifile->sz_filebuffer = f_inifile->sz_filebufstatic;
    memset(f_inifile->sz_filebuffer, 0, f_inifile->i_filebuffer_size);
    f_inifile->b_bufferchanged = -1;

    memset(f_inifile->sz_lastsection, 0, sizeof(f_inifile->sz_lastsection));
    f_inifile->b_sectionfound  = -1;
    f_inifile->i_sc_startpos   = 0;
    f_inifile->i_sc_endpos     = 0;

    f_inifile->i_value_startpos  = 0;
    f_inifile->i_value_endpos    = 0;
    f_inifile->u_mode = 0;
    return  0;
}

//size new size
static int IniFileBufferModify(long size, IniFileData_TS *f_inifile)
{
    if(size <= f_inifile->i_filebuffer_size)
        return 0;
    long newlen = (2 + (size/1024)) * 1024;//每次增加1-2k
    char *p = malloc(newlen);
    if(p == NULL)
        return -1;
    memcpy(p, f_inifile->sz_filebuffer, f_inifile->i_filesize);
    memset(p + f_inifile->i_filesize, 0, newlen - f_inifile->i_filesize);
    if(f_inifile->i_filebuffer_size > M_MAX_BUFFER_SIZE)
    {
        free(f_inifile->sz_filebuffer);
    }
    f_inifile->i_filebuffer_size = newlen;
    f_inifile->sz_filebuffer = p;
    return 0;
}

/*******************************************************************************
 *   desc: open ini file
 *------------------------------------------------------------------------------
 *  param: char *psz_file       -- file to open
 *         unsigned char mode   -- mode == 0 只读  mode > 0 读写
 *------------------------------------------------------------------------------
 * return:  0   -- file succefully opened
 *         -1   -- fail to open ini file
 *         -2   -- fail to read file to buffer
*******************************************************************************/
void *IniOpenFile(const char *psz_file, const unsigned char mode)
{
    struct stat   statbuf;
    IniFileData_TS	*f_inifile = NULL;

    f_inifile = malloc(sizeof(IniFileData_TS));
    if(f_inifile == NULL)
        return NULL;
    IniInit(f_inifile);

    memset(&statbuf, 0, sizeof(struct stat));
    stat(psz_file, &statbuf);
    DBGPRINT("OpenFile -- ini file size = %ld\n", statbuf.st_size);

    // open file
    if(mode == 0)
    {
        f_inifile->p_inifile = fopen(psz_file, "r");
    }
    else
    {
        if(statbuf.st_size)
            f_inifile->p_inifile = fopen(psz_file, "r+");
        else
            f_inifile->p_inifile = fopen(psz_file, "w+");
    }
    if(f_inifile->p_inifile == NULL)
    {
        //perror(psz_file); // for test
        free(f_inifile);
        return NULL;
    }
    f_inifile->u_mode = mode;

    // read file to buffer
    if(statbuf.st_size)
    {
        if(IniFileBufferModify(statbuf.st_size, f_inifile) < 0)
        {
            fclose(f_inifile->p_inifile);
            free(f_inifile);
            return NULL;
        }
        if(fread(f_inifile->sz_filebuffer, statbuf.st_size, 1, f_inifile->p_inifile) != 1)
        {
            fclose(f_inifile->p_inifile);
            free(f_inifile);
            return NULL;
        }
    }
    rewind(f_inifile->p_inifile);
    f_inifile->i_filesize  = statbuf.st_size;              // save file size

    return  f_inifile;
}

//把文件清空
int IniCleanFile(void *fpini)
{
    if(fpini == NULL)
        return -1;
    IniFileData_TS *f_inifile = (IniFileData_TS *)fpini;
    // file not opened or read only
    if(f_inifile->p_inifile == NULL || f_inifile->u_mode == 0)
        return  0;
    
    ftruncate(fileno(f_inifile->p_inifile), 0);
    f_inifile->i_filesize      = 0;
    f_inifile->b_bufferchanged = -1;
    memset(f_inifile->sz_lastsection, 0, sizeof(f_inifile->sz_lastsection));
    f_inifile->b_sectionfound  = -1;
    f_inifile->i_sc_startpos   = 0;
    f_inifile->i_sc_endpos     = 0;

    f_inifile->i_value_startpos  = 0;
    f_inifile->i_value_endpos    = 0;
    return 0;
}

/*******************************************************************************
 *   desc: close ini file
 *------------------------------------------------------------------------------
 *  param: none
 *------------------------------------------------------------------------------
 * return:  0   -- file succefully closed
 *         -1   -- fail to close the opened file
*******************************************************************************/
int IniCloseFile(void *fpini)
{
    if(fpini == NULL)
        return -1;
    IniFileData_TS *f_inifile = (IniFileData_TS *)fpini;
    // file not opened
    if(f_inifile->p_inifile == NULL)
    {
        free(f_inifile);
        return  0;
    }

    // save file if buffer changed
    if(f_inifile->b_bufferchanged == 0)
    {
        DBGPRINT("\nCloseFile -- wrtie file, file size = %ld\n", f_inifile->i_filesize);
        //rewind(p_inifile);
        fwrite(f_inifile->sz_filebuffer, f_inifile->i_filesize, 1, f_inifile->p_inifile);
        f_inifile->b_bufferchanged = -1;
    }
    
    if(f_inifile->i_filebuffer_size > M_MAX_BUFFER_SIZE)
    {
        free(f_inifile->sz_filebuffer);
    }

    // close file
    if(fclose(f_inifile->p_inifile) != -1)
    {
        free(f_inifile);
        return  0;
    }
    free(f_inifile);
    return  -1;
}

//return the ini file size
int IniFileSizeGet(void *fpini)
{
    if(fpini == NULL)
        return 0;
    IniFileData_TS *f_inifile = (IniFileData_TS *)fpini;
    // file not opened
    if(f_inifile->p_inifile == NULL)
    {
        return  0;
    }
    return f_inifile->i_filesize;
}

/*******************************************************************************
 *   desc: search start position of section content
 *------------------------------------------------------------------------------
 *  param: const long i_position  -- next position to ']'<section name end mark>
 *------------------------------------------------------------------------------
 * return: 0     -- found
*******************************************************************************/
static int IniSearchContentStart(const long i_position, IniFileData_TS *f_inifile)
{
    long  i = 0;
    long  i_temp = 0;

    i = i_position;

    // we ignore the rest of sectio name line
    while(i < f_inifile->i_filesize && f_inifile->sz_filebuffer[i] != '\n')
        i++;
    if(f_inifile->sz_filebuffer[i] == '\n')
        i++;

    // if reach end of file, we append some \n
    if(i >= f_inifile->i_filesize)
    {
        f_inifile->i_sc_startpos = i;
        if(IniFileBufferModify(f_inifile->i_filesize + 2, f_inifile) < 0)
            return -1;
        for(i_temp = 0; i_temp < 2; i_temp++)
        {
            f_inifile->sz_filebuffer[i+i_temp]  = '\n';
        }
        f_inifile->i_sc_startpos = i + 1;
        f_inifile->i_filesize  += 2;
        f_inifile->b_bufferchanged = 0;
        return  0;
    }

    // not reach end of file
    f_inifile->i_sc_startpos = i;

    // if it's '['(which means no enough \n between setciotns), we insert some \n
    if(f_inifile->sz_filebuffer[i] == '[')
    {
        if(IniFileBufferModify(f_inifile->i_filesize + 3, f_inifile) < 0)
            return -1;
        for(i_temp = f_inifile->i_filesize; i_temp >= f_inifile->i_sc_startpos; i_temp--)
        {
            f_inifile->sz_filebuffer[i_temp+3] = f_inifile->sz_filebuffer[i_temp];
        }
        for(i_temp = 0; i_temp < 3; i_temp++)
        {
            f_inifile->sz_filebuffer[f_inifile->i_sc_startpos+i_temp] = '\n';
        }
        f_inifile->b_bufferchanged = 0;
        f_inifile->i_filesize  += 3;
    }

    return  0;
}



/*******************************************************************************
 *   desc: search end position of section content
 *------------------------------------------------------------------------------
 *  param: const long i_startpos      -- start position of section content
 *------------------------------------------------------------------------------
 * return: 0   -- found
 ******************************************************************************/
static int IniSearchContentEnd(const long i_startpos, IniFileData_TS *f_inifile)
{
    long  i = 0;
    long  i_temp = 0;

    i = i_startpos;

    // try to serach position of next '['
    while(i < f_inifile->i_filesize)
    {
        // skip space, tab and \n
        while(i < f_inifile->i_filesize && (f_inifile->sz_filebuffer[i] == ' ' || \
               f_inifile->sz_filebuffer[i] == '\t' || f_inifile->sz_filebuffer[i] == '\n'))
            i++;

        //==========================================================================
        // 1. found '[', we try to find a position before '['
        //==========================================================================
        if(f_inifile->sz_filebuffer[i] == '[')
        {
            //DBGPRINT(("SearchContentEnd -- position of next [ = %ld\n", i));
            // skip \n backword
            i_temp  = i;
            while(i > i_startpos + 1 && f_inifile->sz_filebuffer[i-1] == '\n')
                i--;
            f_inifile->i_sc_endpos = i;

            // if no enough \n between setciotns, we insert some \n
            if(f_inifile->i_sc_endpos == i_temp)
            {
                if(IniFileBufferModify(f_inifile->i_filesize + 2, f_inifile) < 0)
                    return -1;
                for(i = f_inifile->i_filesize; i >= i_temp; i--)
                {
                    f_inifile->sz_filebuffer[i+2] = f_inifile->sz_filebuffer[i];
                }
                for(i = 0; i < 2; i++)
                {
                    f_inifile->sz_filebuffer[i_temp+i] = '\n';
                }
                f_inifile->i_filesize  += 2;
                f_inifile->b_bufferchanged = 0;
            }
            return  0;
        }
        else
        {
            // ignore the line and forward
            while(i < f_inifile->i_filesize && f_inifile->sz_filebuffer[i] != '\n')
                i++;
            if(f_inifile->sz_filebuffer[i] == '\n')
                i++;
        }

        //==========================================================================
        // 2. if reach end of file
        //==========================================================================
        if(i == f_inifile->i_filesize)
        {
            // skip \n backword
            while(i > i_startpos + 1 && f_inifile->sz_filebuffer[i-1] == '\n')
                i--;
            f_inifile->i_sc_endpos = i;

            // we append some \n if not enough
            if(i >= f_inifile->i_filesize - 1)
            {
                if(IniFileBufferModify(f_inifile->i_filesize + 2, f_inifile) < 0)
                    return -1;
                i = f_inifile->i_filesize;
                for(i_temp = 0; i_temp < 2; i_temp++)
                {
                    f_inifile->sz_filebuffer[i_temp+i]  = '\n';
                }
                f_inifile->i_filesize  += 2;
                f_inifile->b_bufferchanged = 0;
            }
            return  0;
        }

    }

    return  0;
}



/*******************************************************************************
 *   desc: search a section
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *------------------------------------------------------------------------------
 * return: 0   -- section found
 *         -1  -- section not found
*******************************************************************************/
static int IniSearchSection(const char *psz_section, IniFileData_TS *f_inifile)
{
    long i = 0;
    int b_skip = -1;


    f_inifile->b_sectionfound  = -1;

    // file error
    if(f_inifile->p_inifile == NULL)
        return  -1;

    // section name can't be null
    if(strlen(psz_section) == 0)
        return	-1;

    // same section name to the previous one
    if(strncasecmp(f_inifile->sz_lastsection, psz_section, strlen(psz_section)) == 0)
    {
        f_inifile->b_sectionfound  = 0;
        return  0;
    }

    while(i < f_inifile->i_filesize)
    {
        // skip space, tab and \n
        while(i < f_inifile->i_filesize && (f_inifile->sz_filebuffer[i] == ' ' || \
                f_inifile->sz_filebuffer[i] == '\t' || f_inifile->sz_filebuffer[i] == '\n'))
            i++;
        // return if reach end of file
        if(i >= f_inifile->i_filesize)
            return  -1;

        b_skip = -1;
        switch(f_inifile->sz_filebuffer[i])
        {
        case  '#':            // comment
            b_skip = 0;
            break;

        case  '[':            // section begin mark -- [
            i++;
            while(i < f_inifile->i_filesize && (f_inifile->sz_filebuffer[i] == ' ' ||
                                               f_inifile->sz_filebuffer[i] == '\t'))
                i++;
            if(i >= f_inifile->i_filesize)
                return  -1;

            if(strncasecmp(f_inifile->sz_filebuffer + i, psz_section, strlen(psz_section)) == 0)
            {
                // found section name, we try to seek ']'
                i	+= strlen(psz_section);
                while(i < f_inifile->i_filesize && (f_inifile->sz_filebuffer[i] == ' ' ||
                                                   f_inifile->sz_filebuffer[i] == '\t'))
                    i++;
                if(i >= f_inifile->i_filesize)
                    return  -1;

                if(f_inifile->sz_filebuffer[i] == ']')
                {
                    // ']' found, so we:

                    // 1. save section name to sz_lastsection
                    strncpy(f_inifile->sz_lastsection, psz_section, sizeof(f_inifile->sz_lastsection));
                    f_inifile->sz_lastsection[sizeof(f_inifile->sz_lastsection) - 1] = 0;

                    // 2. get start and end position of section content
                    i++;
                    IniSearchContentStart(i, f_inifile);
                    IniSearchContentEnd(f_inifile->i_sc_startpos, f_inifile);
                    DBGPRINT("\nSearchSection -- section content start at %ld, end at %ld\n",
                              f_inifile->i_sc_startpos, f_inifile->i_sc_endpos);
                    f_inifile->b_sectionfound	= 0;
                    return  0;

                }
                else
                {
                    // no matching ']'
                    b_skip = 0;
                }
            }

            else
            {
                // section name not match
                b_skip = 0;
            }
            break;

        default:              // other
            b_skip = 0;
            break;
        }

        if (b_skip == 0)
        {
            // ignore the line and forward
            while(i < f_inifile->i_filesize && f_inifile->sz_filebuffer[i] != '\n')
                i++;
            if(i >= f_inifile->i_filesize)
                return  -1;
            i++;                          // Jump to the next line
        }

    }

    return  -1;
}


/*******************************************************************************
 *   desc: get a string value by key
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *         const char * psz_key         -- key name
 *         char *       psz_value       -- key value
 *         int          value_size      -- key value max size
 *------------------------------------------------------------------------------
 * return: 0   -- key value found
 *         -1  -- key value not found
*******************************************************************************/
int IniGetString(const char *psz_section, const char *psz_key, char *psz_value, int value_size, void *fpini)
{
    *psz_value = '\0';
    if(fpini == NULL)
        return -1;
    IniFileData_TS *f_inifile = (IniFileData_TS *)fpini;

    long i = 0;
    long j = 0;
    int b_skip = -1;

    // key name can't be null
    if(strlen(psz_key) == 0)
        return	-1;

    // return if section not found
    if(IniSearchSection(psz_section, f_inifile) == -1)
        return	-1;

    DBGPRINT("\nGetString -- to get value of %s\n", psz_key);
    i = f_inifile->i_sc_startpos;
    while(i < f_inifile->i_sc_endpos)
    {
        // skip space, tab and \n
        while(i < f_inifile->i_filesize && (f_inifile->sz_filebuffer[i] == ' ' || \
               f_inifile->sz_filebuffer[i] == '\t' || f_inifile->sz_filebuffer[i] == '\n' ))
            i++;
        // return if reach end of section
        if(i >= f_inifile->i_sc_endpos)
            return  -1;

        b_skip  = -1;
        switch(f_inifile->sz_filebuffer[i])
        {
        case  '#':        // a comment line
            b_skip = 0;
            break;
        default:
            if(strncasecmp(f_inifile->sz_filebuffer + i, psz_key, strlen(psz_key)) == 0)
            {
                //======================================================================
                // key matched, try to find value
                //======================================================================
                i += strlen(psz_key);
                // 1. skip space and tab
                while(i < f_inifile->i_sc_endpos &&
                        (f_inifile->sz_filebuffer[i] == ' ' || f_inifile->sz_filebuffer[i] == '\t'))
                    i++;
                if(i >= f_inifile->i_sc_endpos)
                    return  -1;

                // 2. try to find '='
                if(f_inifile->sz_filebuffer[i] == '=')
                {
                    //====================================================================
                    // '=' found, get kay value
                    //====================================================================
                    i++;
                    // skip space and tab
                    while(i < f_inifile->i_sc_endpos &&
                            (f_inifile->sz_filebuffer[i] == ' ' || f_inifile->sz_filebuffer[i] == '\t'))
                        i++;
                    if(i >= f_inifile->i_sc_endpos)
                    {//no value
                        f_inifile->i_value_startpos  = i;
                        f_inifile->i_value_endpos    = i-1;
                        return  0;
                    }

                    // search the end of the key value
                    j = i;
                    while(j < f_inifile->i_sc_endpos && f_inifile->sz_filebuffer[j] != '\n')
                        j++;
                    j--;
                    if(f_inifile->sz_filebuffer[j] == '\r')//假如是dos格式，则要考虑把\r\n去掉
                        j--;
                    while(f_inifile->sz_filebuffer[j] == ' ' || f_inifile->sz_filebuffer[j] == '\t')
                        j--;

                    if((j - i) < 0)
                    {//no value
                        f_inifile->i_value_startpos  = i;
                        f_inifile->i_value_endpos    = i-1;
                        return 0;
                    }
                    
                    // copy the key value
                    if(value_size > (j - i + 1))
                        value_size = (j - i + 1);
                    else
                        value_size--;
                    strncpy(psz_value, f_inifile->sz_filebuffer + i, value_size);
                    *(psz_value + value_size) = '\0';
                    f_inifile->i_value_startpos  = i;
                    f_inifile->i_value_endpos    = j;
                    DBGPRINT("GetString -- value of %s is: %s\n", psz_key, psz_value);

                    return  0;
                }
                else
                {
                    //====================================================================
                    // no matching '=', ignore the line
                    //====================================================================
                    b_skip  = 0;
                }
            }
            else
            {
                //======================================================================
                // key not matched, ignore the line and forward
                //======================================================================
                b_skip  = 0;
            }
            break;
        }

        if (b_skip == 0)
        {
            // ignore the line and forward
            while(i < f_inifile->i_filesize && f_inifile->sz_filebuffer[i] != '\n')
                i++;
            if(i >= f_inifile->i_filesize)
                return  -1;
            i++;                          // Jump to the next line
        }
    }

    return  -1;
}

/*******************************************************************************
 *   desc: set a string value by key
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *         const char * psz_key         -- key name
 *         const char * psz_value       -- key value
 *------------------------------------------------------------------------------
 * return: 0   -- key value writen to buffer
*******************************************************************************/
int IniSetString(const char *psz_section, const char *psz_key, const char *psz_value, void *fpini)
{
    char  sz_value[M_MAX_INTVAL_BUFFER_SIZE];
    int   i_oldvaluelen = 0;                    // lenght of old value
    int   i_newvaluelen = 0;                    // lenght of new value
    long  i = 0;
    long  i_temp = 0;
    
    if(fpini == NULL)
        return -1;
    IniFileData_TS *f_inifile = (IniFileData_TS *)fpini;
    
    if(f_inifile->u_mode == 0)
        return -1;

    //DBGPRINT("\nSetString -- to search section: %s\n", psz_section);
    if(IniSearchSection(psz_section, f_inifile) == -1)
    {
        //==========================================================================
        // section not found, we append the section and key value
        // at the end of buffer
        //==========================================================================
        //sprintf(sz_value, "[%s]\n%s = %s\n", psz_section, psz_key, psz_value);
        i_temp  += (strlen(psz_section) + strlen(psz_key) + strlen(psz_value) + 7);
        if(IniFileBufferModify(f_inifile->i_filesize + i_temp, f_inifile) < 0)
            return -1;
        i = f_inifile->i_filesize;
        strncpy(f_inifile->sz_filebuffer + i, "[", 1);
        i += 1;
        strncpy(f_inifile->sz_filebuffer + i, psz_section, strlen(psz_section));
        i += strlen(psz_section);
        strncpy(f_inifile->sz_filebuffer + i, "]\n", 2);
        i += 2;
        strncpy(f_inifile->sz_filebuffer + i, psz_key, strlen(psz_key));
        i += strlen(psz_key);
        strncpy(f_inifile->sz_filebuffer + i, " = ", 3);
        i += 3;
        strncpy(f_inifile->sz_filebuffer + i, psz_value, strlen(psz_value));
        i += strlen(psz_value);
        strncpy(f_inifile->sz_filebuffer + i, "\n", 1);
        f_inifile->i_filesize += i_temp;
        f_inifile->b_bufferchanged = 0;
        return  0;
    }

    if(IniGetString(psz_section, psz_key, sz_value, M_MAX_INTVAL_BUFFER_SIZE, f_inifile) == 0)
    {
        //==========================================================================
        // section and key found, replace value
        //==========================================================================
        i_oldvaluelen = f_inifile->i_value_endpos - f_inifile->i_value_startpos + 1;
        i_newvaluelen = strlen(psz_value);
        //DBGPRINT("SetString -- before update, file size = %d\n", i_filesize);
        if(i_newvaluelen == 0)
        {//delete this line \r-0x0D \n-0x0A  linux \n  dos \r\n
            long start;//行首
            long end;//行尾
            start = f_inifile->i_value_startpos;
            while(start > f_inifile->i_sc_startpos && f_inifile->sz_filebuffer[start] != '\n')
                start--;
            start++;
            end = f_inifile->i_value_endpos;
            while(end < f_inifile->i_sc_endpos && f_inifile->sz_filebuffer[end] != '\n')
                end++;
            end ++;
            i_temp  = end - start;
            for(i = start; i < (f_inifile->i_filesize - i_temp); i++)
            {
                f_inifile->sz_filebuffer[i] = f_inifile->sz_filebuffer[i+i_temp];
            }
            f_inifile->i_filesize  -= i_temp;
        }
        else if(i_newvaluelen > i_oldvaluelen)
        {
            // new value is longer than old value ************************************
            // 1. get more space by moving content backward
            i_temp  = i_newvaluelen - i_oldvaluelen;
            if(IniFileBufferModify(f_inifile->i_filesize + i_temp, f_inifile) < 0)
                return -1;
            for(i = f_inifile->i_filesize - 1; i >= f_inifile->i_value_endpos; i--)
            {
                f_inifile->sz_filebuffer[i+i_temp] = f_inifile->sz_filebuffer[i];
            }
            f_inifile->i_filesize  += i_temp;
            f_inifile->sz_filebuffer[f_inifile->i_filesize+1] = '\0';

            // 2. write new value to buffer
            strncpy(f_inifile->sz_filebuffer + f_inifile->i_value_startpos, psz_value, strlen(psz_value));

        }
        else if(i_newvaluelen < i_oldvaluelen)
        {
            // new value is shorter than old value ***********************************
            // 1. write new value to buffer
            strncpy(f_inifile->sz_filebuffer + f_inifile->i_value_startpos, psz_value, strlen(psz_value));
            // 2. replace the following chars with spaces
            i_temp  = i_oldvaluelen - i_newvaluelen;
            for(i = 0; i < i_temp; i++)
            {
                f_inifile->sz_filebuffer[f_inifile->i_value_endpos-i] = ' ';
            }
        }

        else
        {
            // same length ***********************************************************
            // just replace old value
            strncpy(f_inifile->sz_filebuffer + f_inifile->i_value_startpos, psz_value, strlen(psz_value));
        }
        //DBGPRINT("SetString -- after  update, file size = %d\n", f_inifile->i_filesize);
        f_inifile->b_bufferchanged = 0;

    }
    else
    {
        if(!strlen(psz_value))
        {//空值不写入
            return 0;
        }
        //==========================================================================
        // key not found, we add key value by inserting a new line
        //==========================================================================
        // 1. make new line
        //sprintf(sz_value, "\n%s = %s", psz_key, psz_value);
        i_temp	= strlen(psz_key) + strlen(psz_value) + 4;

        if(IniFileBufferModify(f_inifile->i_filesize + i_temp, f_inifile) < 0)
            return -1;
        // 2. move buffer for new line
        for(i = f_inifile->i_filesize; i >= f_inifile->i_sc_endpos; i--)
        {
            f_inifile->sz_filebuffer[i+i_temp] = f_inifile->sz_filebuffer[i];
        }

        // 3. copy new line to buffer
        i = f_inifile->i_sc_endpos;
        strncpy(f_inifile->sz_filebuffer + i, "\n", 1);
        i += 1;
        strncpy(f_inifile->sz_filebuffer + i, psz_key, strlen(psz_key));
        i += strlen(psz_key);
        strncpy(f_inifile->sz_filebuffer + i, " = ", 3);
        i += 3;
        strncpy(f_inifile->sz_filebuffer + i, psz_value, strlen(psz_value));
        f_inifile->i_filesize += i_temp;
        f_inifile->b_bufferchanged = 0;
    }

    // search end position of content again
    IniSearchContentEnd(f_inifile->i_sc_startpos, f_inifile);

    return  0;
}

/*******************************************************************************
 *   desc: get a interger value by key
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *         const char * psz_key         -- key name
 *         int          i_default       -- default value
 *------------------------------------------------------------------------------
 * return: key value or default value
*******************************************************************************/
int IniGetInteger(const char *psz_section, const char *psz_key, int i_default, void *fpini)
{
    char sz_buffer[M_MAX_INTVAL_BUFFER_SIZE];

    //memset(sz_buffer, 0, sizeof(sz_buffer));
    if(IniGetString(psz_section, psz_key, sz_buffer, M_MAX_INTVAL_BUFFER_SIZE, fpini) == 0)
    {
        DBGPRINT("GetInteger -- key value is: %s\n", sz_buffer);
        if(strlen(sz_buffer) > 2)
        {
            // maybe a hex value
            if(sz_buffer[0] == '0'  && (sz_buffer[1] == 'x' || sz_buffer[1] == 'X'))
            {
                return  (int)(strtol(sz_buffer, (char **)NULL, 16));
            }
        }
        return  atoi(sz_buffer);
    }

    return  i_default;
}

/*******************************************************************************
 *   desc: set a interger value
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *         const char * psz_key         -- key name
 *         const int    i_value         -- key value
 *------------------------------------------------------------------------------
 * return: 0
*******************************************************************************/
int IniSetInteger(const char *psz_section, const char *psz_key, const int i_value, void *fpini)
{
    char sz_buffer[M_MAX_INTVAL_BUFFER_SIZE];
    
    DBGPRINT("SetInteger -- key value is: %d\n", i_value);
    memset(sz_buffer, 0, sizeof(sz_buffer));
    sprintf(sz_buffer, "%d", i_value);
    DBGPRINT("SetInteger -- value buffer is: %s\n", sz_buffer);
    return IniSetString(psz_section, psz_key, sz_buffer, fpini);
}

/*******************************************************************************
 *   desc: get a long value by key
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *         const char * psz_key         -- key name
 *         long         i_default       -- default value
 *------------------------------------------------------------------------------
 * return: key value or default value
*******************************************************************************/
long IniGetLong(const char *psz_section, const char *psz_key, long i_default, void *fpini)
{
    char sz_buffer[M_MAX_INTVAL_BUFFER_SIZE];

    //memset(sz_buffer, 0, sizeof(sz_buffer));
    if(IniGetString(psz_section, psz_key, sz_buffer, M_MAX_INTVAL_BUFFER_SIZE, fpini) == 0)
    {
        if(strlen(sz_buffer) > 2)
        {
            // maybe a hex value
            if(sz_buffer[0] == '0'  && (sz_buffer[1] == 'x' || sz_buffer[1] == 'X'))
            {
                return  (strtol(sz_buffer, (char **)NULL, 16));
            }
        }
        return  atol(sz_buffer);
    }

    return  i_default;
}

/*******************************************************************************
 *   desc: set a long value
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *         const char * psz_key         -- key name
 *         const long   i_value         -- key value
 *------------------------------------------------------------------------------
 * return: 0
*******************************************************************************/
int IniSetLong(const char *psz_section, const char *psz_key, const long i_value, void *fpini)
{
    char sz_buffer[M_MAX_INTVAL_BUFFER_SIZE];
    
    memset(sz_buffer, 0, sizeof(sz_buffer));
    sprintf(sz_buffer, "%ld", i_value);
    return IniSetString(psz_section, psz_key, sz_buffer, fpini);
}

/*******************************************************************************
 *   desc: get a double value by key
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *         const char * psz_key         -- key name
 *         double       i_default       -- default value
 *------------------------------------------------------------------------------
 * return: key value or default value
*******************************************************************************/
double IniGetDouble(const char *psz_section, const char *psz_key, double i_default, void *fpini)
{
    char sz_buffer[M_MAX_INTVAL_BUFFER_SIZE];

    //memset(sz_buffer, 0, sizeof(sz_buffer));
    if(IniGetString(psz_section, psz_key, sz_buffer, M_MAX_INTVAL_BUFFER_SIZE, fpini) == 0)
    {
        return  atof(sz_buffer);
    }

    return  i_default;
}

/*******************************************************************************
 *   desc: set a double value
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *         const char * psz_key         -- key name
 *         const double i_value         -- key value
 *------------------------------------------------------------------------------
 * return: 0
*******************************************************************************/
int IniSetDouble(const char *psz_section, const char *psz_key, const double i_value, void *fpini)
{
    char sz_buffer[M_MAX_INTVAL_BUFFER_SIZE];
    
    memset(sz_buffer, 0, sizeof(sz_buffer));
    sprintf(sz_buffer, "%g", i_value);
    return IniSetString(psz_section, psz_key, sz_buffer, fpini);
}

/*******************************************************************************
 *   desc: get a bool value by key
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *         const char * psz_key         -- key name
 *         bool         b_default       -- default value
 *------------------------------------------------------------------------------
 * return: key value or default value
*******************************************************************************/
int IniGetBool(const char *psz_section, const char *psz_key, int b_default, void *fpini)
{
    char sz_buffer[M_MAX_INTVAL_BUFFER_SIZE];

    //memset(sz_buffer, 0, sizeof(sz_buffer));
    if(IniGetString(psz_section, psz_key, sz_buffer, M_MAX_INTVAL_BUFFER_SIZE, fpini) == 0)
    {
        DBGPRINT("GetBool -- key value is: %s\n", sz_buffer);
        if(strncasecmp(sz_buffer, "y", strlen("y")) == 0      ||
                strncasecmp(sz_buffer, "yes", strlen("yes")) == 0  ||
                strncasecmp(sz_buffer, "true", strlen("true")) == 0 )
            return  1;
        if(strncasecmp(sz_buffer, "n", strlen("n")) == 0      ||
                strncasecmp(sz_buffer, "no", strlen("no")) == 0    ||
                strncasecmp(sz_buffer, "false", strlen("false")) == 0 )
            return  0;
    }

    return  b_default;
}

/*******************************************************************************
 *   desc: set a bool value
 *------------------------------------------------------------------------------
 *  param: const char * psz_section     -- section name
 *         const char * psz_key         -- key name
 *         const bool   b_value         -- key value
 *------------------------------------------------------------------------------
 * return: 0
*******************************************************************************/
int IniSetBool(const char *psz_section, const char *psz_key, const int b_value, void *fpini)
{
    char sz_buffer[M_MAX_INTVAL_BUFFER_SIZE];
    
    memset(sz_buffer, 0, sizeof(sz_buffer));
    if(b_value)
        sprintf(sz_buffer, "%s", "true");
    else
        sprintf(sz_buffer, "%s", "false");
    return IniSetString(psz_section, psz_key, sz_buffer, fpini);
}

#if 0
//==============================================================================
// test code
//==============================================================================
int main(int argc, char **argv)
{
    char    sz_value[256];
    int     i_status = 0;
    long	  l_status = 0;
    double  d_status = 0;
    int    b_status;

    DBGPRINT("\nMain -- ------------------DEMO.C-------------------------\n");
    // 1. open ini file
    void *fpini = IniOpenFile("test.ini", 1);
    if(fpini == NULL)
        return  -1;


    // 如果不确定该Ini文件是否为Unix类型文件，调用转换函数进行转换
    //if (ToUnixStyle() != 0)
    DBGPRINT("Fail to convert to unix style file.\n");


#if 1
    // 3. get/set value of string type
    memset(sz_value, 0, sizeof(sz_value);
    b_status  = IniGetString("sectionA", "AID", sz_value, 256, fpini);
    DBGPRINT("Main -- sectionA--AID = %s\n", sz_value);

    DBGPRINT("\nMain -- to Set String");
    b_status  = IniSetString("sectionC", "PID", "ABCD1234561234567890", fpini);
    DBGPRINT("Main -- PID set to: %s\n", "ABCD1234561234567890", fpini);

    memset(sz_value, 0, sizeof(sz_value);
    b_status  = IniGetString("sectionC", "PID", sz_value, 256, fpini);
    DBGPRINT("Main -- sectionC--PID = %s\n", sz_value, fpini);

    memset(sz_value, 0, sizeof(sz_value);
    b_status  = IniGetString("sectionC", "AID", sz_value, 256, fpini);
    DBGPRINT("Main -- sectionC--AID = %s\n", sz_value, fpini);
#endif


#if 0
    // 4. get/set value of integer type
    i_status  = IniGetInteger("sectionA", "NNN", 99, fpini);
    DBGPRINT("Main -- sectionA--NNN = %d\n", i_status);

    i_status  = IniGetInteger("sectionA", "NUM", 99, fpini);
    DBGPRINT("Main -- sectionA--NUM = %d\n", i_status);

    DBGPRINT("\nMain -- to SetInteger\n");
             b_status  = IniSetInteger("sectionA", "NUM", 12345, fpini);
             DBGPRINT("Main -- NUM set to: %d\n", 12345);

             i_status  = IniGetInteger("sectionA", "NUM", 999, fpini);
             DBGPRINT("Main -- sectionA--NUM = %d\n", i_status);
#endif
#if 0
             // 5. get/set value of long type
             l_status  = IniGetLong("sectionA", "NNN", 99, fpini);
             DBGPRINT("Main -- sectionA--NNN = %ld\n", l_status);

             l_status  = IniGetLong("sectionA", "NUM", 99, fpini);
             DBGPRINT("Main -- sectionA--NUM = %ld\n", l_status);

             DBGPRINT("\nMain -- to SetInteger\n");
             b_status  = IniSetLong("sectionA", "NUM", 12345678, fpini);
             DBGPRINT("Main -- NUM set to: %ld\n", 12345678);

             l_status  = IniGetLong("sectionA", "NUM", 999, fpini);
             DBGPRINT("Main -- sectionA--NUM = %ld\n", l_status);
#endif


#if 0
             // 6. get/set value of double type
             d_status  = IniGetDouble("sectionA", "NNN", 99, fpini);
             DBGPRINT("Main -- sectionA--NNN = %g\n", d_status);

             d_status  = IniGetDouble("sectionA", "NUM", 99, fpini);
             DBGPRINT("Main -- sectionA--NUM = %g\n", d_status);

             DBGPRINT("\nMain -- to Set Number");
             b_status  = IniSetDouble("sectionA", "NUM", 12345678.123, fpini);
             DBGPRINT("Main -- NUM set to: %g\n", 12345678.123);

             d_status  = IniGetDouble("sectionA", "NUM", 999, fpini);
             DBGPRINT("Main -- sectionA--NUM = %g\n", d_status);
#endif

#if 1
             // 7. get/set value of int type
             b_status  = IniGetBool("sectionA", "BOOLA", -1, fpini);
             DBGPRINT("Main -- sectionA--BOOLA = %d\n", (int)b_status);

             b_status  = IniGetBool("sectionA", "BOOLB", -1, fpini);
             DBGPRINT("Main -- sectionA--BOOLB = %d\n", (int)b_status);

             DBGPRINT("\nMain -- to Set Bool");
             b_status  = IniSetBool("sectionA", "BOOLB", -1, fpini);
             DBGPRINT("Main -- BOOLB set to: %d\n", (int)-1);

             b_status  = IniGetBool("sectionA", "BOOLB", -1, fpini);
             DBGPRINT("Main -- sectionA--BOOLB = %d\n", (int)b_status);
#endif


             // 9. close file
             IniCloseFile(fpini);

             DBGPRINT("Main -- ---------------------E N D-------------------------\n\n");

             return  0;
         }
#endif




