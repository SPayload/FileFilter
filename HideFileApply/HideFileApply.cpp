#include "common.h"
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <fltuser.h>
#include <FltUserStructures.h>

#pragma comment(lib, "FltLib.lib")

#define MINISPY_NAME      L"MiniSpy"


VOID MyAddProtectPath(LPCWCH path, LPCWCH fileType);

VOID MyDelProtectPath(LPCWCH path, LPCWCH fileType);


HANDLE port = INVALID_HANDLE_VALUE;


int wmain(int argc, wchar_t* argv[])
{

    /*for (int i = 0; i < argc; i++)
        printf("argv[%d]:%S\n", i, argv[i]);*/
    
    HRESULT hResult = S_OK;

    //printf("Connecting to filter's port...\n");

    // �򿪵����ļ�ϵͳ΢�͹�����������ͨ�ŷ������˿ڵ�������
    hResult = FilterConnectCommunicationPort(MINISPY_PORT_NAME,
        FLT_PORT_FLAG_SYNC_HANDLE,
        NULL,
        0,
        NULL,
        &port);

    if (IS_ERROR(hResult)) {

        printf("Could not connect to filter: 0x%08x\n", hResult);
        goto Main_Exit;
    }

    if (1 >= argc) {// �޲���
        return -1;
    }
    else {
        if (!wcscmp(argv[1], L"hide"))
        {
            wchar_t FilePath[MAX_PATH] = { 0 };
            wchar_t FileType[MAX_PATH] = { 0 };
            wcscpy(FilePath, argv[2]);
            wcscpy(FileType, argv[3]);
            //������裺���� FilePath Ŀ¼�µ���չ��Ϊ FileType �������ļ����������� FileType ���ļ�����������
            MyAddProtectPath(FilePath, FileType);//L"C:\\test*", L"*.txt*"
        }
        else if (!wcscmp(argv[1], L"unhide")) {
            wchar_t FilePath[MAX_PATH] = { 0 };
            wchar_t FileType[MAX_PATH] = { 0 };
            wcscpy(FilePath, argv[2]);
            wcscpy(FileType, argv[3]);
            //ȡ������ FilePath Ŀ¼�µ���չ��Ϊ FileType �������ļ����������� FileType ���ļ�����������
            MyDelProtectPath(FilePath, FileType);
        }
    }

Main_Exit:

    if (INVALID_HANDLE_VALUE != port)
    {
        CloseHandle(port);
    }

    return 0;
}



VOID MyAddProtectPath(LPCWCH path, LPCWCH fileType)
{
    DWORD result;
    INPUT_BUFFER input;

    ZeroMemory(&input, sizeof(INPUT_BUFFER));

    input.command = ADD_PROTECTED_PATH;
    wcscpy(input.protectedPath, path);
    wcscpy(input.fileType, fileType);


    //���� ADD_PROTECTED_PATH���׼�������������ͨ��
    FilterSendMessage(port,
        &input,
        sizeof(INPUT_BUFFER),
        NULL,
        0,
        &result);
}


VOID MyDelProtectPath(LPCWCH path, LPCWCH fileType)
{
    DWORD result;
    INPUT_BUFFER input;

    ZeroMemory(&input, sizeof(INPUT_BUFFER));

    input.command = DEL_PROTECTED_PATH;
    wcscpy(input.protectedPath, path);
    wcscpy(input.fileType, fileType);

    //���� DEL_PROTECTED_PATH���׼�������������ͨ��
    FilterSendMessage(port,
        &input,
        sizeof(INPUT_BUFFER),
        NULL,
        0,
        &result);
}

