#pragma once

#include<iostream>
#include<windows.h>
#include<fileapi.h>
#include<vector>

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QStandardItem>



using namespace std;

//FileHeaderTamper : Object
#define DOS_T 1     // DOSͷ�۸�
#define NT_T 2      // NTͷ�۸�

//FileHeaderTamper : DOS - Decision
//#define 

//FileHeaderTamper : NT - Decision
#define e_magic_T 1     // �޸�PEǩ��
#define e_lfanew_T 2    // �޸�ָ��PEͷ��ƫ����


// �ֶ���Ϣ�ṹ��
typedef struct FieldInfo
{
    std::string field;  // �ֶ�����
    int size;           // �ֶδ�С(�ֽ�)
    int offset;         // �ֶ�ƫ����
}FieldInfo;

//std::vector< FieldInfo>IMAGE_DOS_HEADER_INFO;

//std::vector<std::tuple<std::string, int, int>>IMAGE_DOS_HEADER_INFO[19];
//std::vector<std::tuple<std::string, int, int>>IMAGE_NT_HEADER_INFOS[];


// DOS����ṹ�壨64�ֽڣ�
typedef struct IMAGE_DOS_STUB
{
    BYTE stub[64];      // DOS�������
}IMAGE_DOS_STUB;


//RICH_HEADER;
//extern char* stubbuffer;
//extern const std::vector<std::vector<FieldInfo>>ALL_HEADERS_INFO;
extern const std::vector< FieldInfo>IMAGE_DOS_HEADER_INFO;
//extern const std::vector< FieldInfo>IMAGE_NT_HEADERS_INFO;
typedef struct IMAGE_NT_HEADERS_INFO
{
	DWORD Signature;	// PEǩ��

}IMAGE_NT_HEADERS_INFO;
extern const std::vector< FieldInfo>IMAGE_SECTION_HEADERS_INFO;





extern unsigned char shellcode[];       // �ⲿ�����shellcode����



QString StdStringToStructHexQString(std::string str);