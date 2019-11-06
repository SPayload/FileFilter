#include <windows.h>
#include <strsafe.h>


#pragma warning(disable, 4996)

// 宽字符转换为多字符(Unicode --> ASCII)
#define  WCHAR_TO_CHAR(lpW_Char, lpChar) \
    WideCharToMultiByte(CP_ACP, NULL, lpW_Char, -1, \
lpChar, _countof(lpChar), NULL, FALSE);

// 多字符转换为宽字符(ASCII --> Unicode)
#define  CHAR_TO_WCHAR(lpChar, lpW_Char) \
    MultiByteToWideChar(CP_ACP, NULL, lpChar, -1, \
lpW_Char, _countof(lpW_Char));

//根据传入的路径遍历
LARGE_INTEGER VirusKilling(WCHAR* lpFoldPath);

int wmain(int argc, wchar_t* argv[])
{
    if (1 >= argc) {// 无参数
        return -1;
    }
    else {
        wchar_t DirectoryPath[MAX_PATH] = { 0 };
        wcscpy(DirectoryPath, argv[1]);
        printf("%-35s%-100s \n", "文件名", "文件路径");
        VirusKilling(DirectoryPath);        
    }
    return 0;
}





//根据传入的路径遍历
LARGE_INTEGER VirusKilling(WCHAR* lpFoldPath)
{
    //初始化路径字符串
    WCHAR szFoldPath[MAX_PATH] = { 0 }; //获取子路径
    StringCchCat(szFoldPath, MAX_PATH, lpFoldPath);
    StringCchCat(szFoldPath, MAX_PATH, L"\\*");

    //获取第一个文件的信息
    LARGE_INTEGER qwFileTotalSize = { 0 };//文件总大小
    WIN32_FIND_DATA w32FindData = { 0 };	//文件信息结构体
    HANDLE hFindFile = FindFirstFile(szFoldPath, &w32FindData);//获取此文件的句柄,和文件信息结构体

    

    //循环遍历获取当前目录中的文件信息
    do {
        //去掉两个特殊目录
        if ((!wcscmp(w32FindData.cFileName, L".")) || (!wcscmp(w32FindData.cFileName, L"..")))
            continue;
        if (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)// 文件夹
        {
            //文件夹
            WCHAR szSubfoldPath[MAX_PATH] = { 0 };
            //StringCchPrintf需要包头文件#include <strsafe.h>
            //StringCchPrintf(szSubfoldPath, MAX_PATH, L"%S\\", lpFoldPath);//追加反斜杠
            StringCchCat(szSubfoldPath, MAX_PATH, lpFoldPath);
            StringCchCat(szSubfoldPath, MAX_PATH, L"\\");
            StringCchCat(szSubfoldPath, MAX_PATH, w32FindData.cFileName); //子目录名
            qwFileTotalSize = VirusKilling(szSubfoldPath);
        }
        else
        {
            WCHAR szFullPath[MAX_PATH] = { 0 };//保存当前全部路径
            StringCchCat(szFullPath, MAX_PATH, lpFoldPath);
            StringCchCat(szFullPath, MAX_PATH, L"\\");
            StringCchCat(szFullPath, MAX_PATH, w32FindData.cFileName); //子目录名
            CHAR cFullPath[MAX_PATH] = { 0 };
            CHAR cFileName[MAX_PATH] = { 0 };
            WCHAR_TO_CHAR(w32FindData.cFileName, cFileName);
            WCHAR_TO_CHAR(szFullPath, cFullPath);
            printf("%-35s%-100s \n", cFileName, cFullPath);
        }
    } while (FindNextFile(hFindFile, &w32FindData));
    return qwFileTotalSize;
}