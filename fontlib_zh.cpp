#include "fontlib_zh.h"
#include <iostream>
#include <string.h>

bool strMatch(const char* str1, const char* str2)
{
    bool retCode = false;
    do {
        if((strlen(str2) != strlen(str1))) {
            break;
        }
        if(strcmp(str1, str2) == 0) {
            retCode = true;
        }
    } while(0);
    return retCode;//两个字符串相等
}

int getMatchedPymb(unsigned char *strin, T9PY_IDX **matchlist)
{
    T9PY_IDX *bestmatch;//最佳匹配
    int mcnt = 0;
    bestmatch = (T9PY_IDX*)&t9PY_index[0];//默认为a 的匹配
    int pyindexLen=sizeof(t9PY_index) / sizeof(t9PY_index[0]);//得到py 索引表的大小.
    for(int i = 0; i < pyindexLen; i++)
    {
        if(strMatch((const char* )strin, t9PY_index[i].T9))
        {
            matchlist[mcnt++] = (T9PY_IDX*)&t9PY_index[i];
        }
    }
    return mcnt;//返回匹配的个数
}


bool getMatchedPyEn(int nIn, char *matchstr)
{
    int pyindexLen=sizeof(t9EN_index) / sizeof(t9EN_index[0]);//得到py 索引表的大小.
    for(int i = 0; i < pyindexLen; i++)
    {
        if(nIn == t9EN_index[i].key)
        {
            strcpy(matchstr, t9EN_index[i].Letter);
            return true;
        }
    }
    return false;//返回匹配的个数
}
