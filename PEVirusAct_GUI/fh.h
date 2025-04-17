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
#define DOS_T 1     // DOS头篡改
#define NT_T 2      // NT头篡改

//FileHeaderTamper : DOS - Decision
//#define 

//FileHeaderTamper : NT - Decision
#define e_magic_T 1     // 修改PE签名
#define e_lfanew_T 2    // 修改指向PE头的偏移量


// 字段信息结构体
typedef struct FieldInfo
{
    std::string field;  // 字段名称
    int size;           // 字段大小(字节)
    int offset;         // 字段偏移量
}FieldInfo;

//std::vector< FieldInfo>IMAGE_DOS_HEADER_INFO;

//std::vector<std::tuple<std::string, int, int>>IMAGE_DOS_HEADER_INFO[19];
//std::vector<std::tuple<std::string, int, int>>IMAGE_NT_HEADER_INFOS[];


// DOS存根结构体（64字节）
typedef struct IMAGE_DOS_STUB
{
    BYTE stub[64];      // DOS存根数据
}IMAGE_DOS_STUB;


//RICH_HEADER;
//extern char* stubbuffer;
//extern const std::vector<std::vector<FieldInfo>>ALL_HEADERS_INFO;
extern const std::vector< FieldInfo>IMAGE_DOS_HEADER_INFO;
//extern const std::vector< FieldInfo>IMAGE_NT_HEADERS_INFO;
typedef struct IMAGE_NT_HEADERS_INFO
{
	DWORD Signature;	// PE签名

}IMAGE_NT_HEADERS_INFO;
extern const std::vector< FieldInfo>IMAGE_SECTION_HEADERS_INFO;





extern unsigned char shellcode[];       // 外部定义的shellcode数组



QString StdStringToStructHexQString(std::string str);