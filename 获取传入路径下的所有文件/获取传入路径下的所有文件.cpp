#include <windows.h>
#include <strsafe.h>


#pragma warning(disable, 4996)

// ���ַ�ת��Ϊ���ַ�(Unicode --> ASCII)
#define  WCHAR_TO_CHAR(lpW_Char, lpChar) \
    WideCharToMultiByte(CP_ACP, NULL, lpW_Char, -1, \
lpChar, _countof(lpChar), NULL, FALSE);

// ���ַ�ת��Ϊ���ַ�(ASCII --> Unicode)
#define  CHAR_TO_WCHAR(lpChar, lpW_Char) \
    MultiByteToWideChar(CP_ACP, NULL, lpChar, -1, \
lpW_Char, _countof(lpW_Char));

//���ݴ����·������
LARGE_INTEGER VirusKilling(WCHAR* lpFoldPath);

int wmain(int argc, wchar_t* argv[])
{
    if (1 >= argc) {// �޲���
        return -1;
    }
    else {
        wchar_t DirectoryPath[MAX_PATH] = { 0 };
        wcscpy(DirectoryPath, argv[1]);
        printf("%-35s%-100s \n", "�ļ���", "�ļ�·��");
        VirusKilling(DirectoryPath);        
    }
    return 0;
}





//���ݴ����·������
LARGE_INTEGER VirusKilling(WCHAR* lpFoldPath)
{
    //��ʼ��·���ַ���
    WCHAR szFoldPath[MAX_PATH] = { 0 }; //��ȡ��·��
    StringCchCat(szFoldPath, MAX_PATH, lpFoldPath);
    StringCchCat(szFoldPath, MAX_PATH, L"\\*");

    //��ȡ��һ���ļ�����Ϣ
    LARGE_INTEGER qwFileTotalSize = { 0 };//�ļ��ܴ�С
    WIN32_FIND_DATA w32FindData = { 0 };	//�ļ���Ϣ�ṹ��
    HANDLE hFindFile = FindFirstFile(szFoldPath, &w32FindData);//��ȡ���ļ��ľ��,���ļ���Ϣ�ṹ��

    

    //ѭ��������ȡ��ǰĿ¼�е��ļ���Ϣ
    do {
        //ȥ����������Ŀ¼
        if ((!wcscmp(w32FindData.cFileName, L".")) || (!wcscmp(w32FindData.cFileName, L"..")))
            continue;
        if (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)// �ļ���
        {
            //�ļ���
            WCHAR szSubfoldPath[MAX_PATH] = { 0 };
            //StringCchPrintf��Ҫ��ͷ�ļ�#include <strsafe.h>
            //StringCchPrintf(szSubfoldPath, MAX_PATH, L"%S\\", lpFoldPath);//׷�ӷ�б��
            StringCchCat(szSubfoldPath, MAX_PATH, lpFoldPath);
            StringCchCat(szSubfoldPath, MAX_PATH, L"\\");
            StringCchCat(szSubfoldPath, MAX_PATH, w32FindData.cFileName); //��Ŀ¼��
            qwFileTotalSize = VirusKilling(szSubfoldPath);
        }
        else
        {
            WCHAR szFullPath[MAX_PATH] = { 0 };//���浱ǰȫ��·��
            StringCchCat(szFullPath, MAX_PATH, lpFoldPath);
            StringCchCat(szFullPath, MAX_PATH, L"\\");
            StringCchCat(szFullPath, MAX_PATH, w32FindData.cFileName); //��Ŀ¼��
            CHAR cFullPath[MAX_PATH] = { 0 };
            CHAR cFileName[MAX_PATH] = { 0 };
            WCHAR_TO_CHAR(w32FindData.cFileName, cFileName);
            WCHAR_TO_CHAR(szFullPath, cFullPath);
            printf("%-35s%-100s \n", cFileName, cFullPath);
        }
    } while (FindNextFile(hFindFile, &w32FindData));
    return qwFileTotalSize;
}