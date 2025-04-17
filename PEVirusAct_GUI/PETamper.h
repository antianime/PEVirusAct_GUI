#pragma once
#include"fh.h"


class PETamper
{
public:
	IMAGE_DOS_HEADER idh;           //DOS头（以"MZ"开头）
	IMAGE_NT_HEADERS inh;			//NT头（PE签名、文件头、可选头）
	std::vector<BYTE> stubbuffer;						// 存储DOS存根数据
	// 节区相关数据结构
	WORD NumberOfSections;
	std::vector<IMAGE_SECTION_HEADER> SectionHeaders;	// 存储所有节区头
	std::vector<std::vector<BYTE>>SectionNames;			// 存储所有节区名称
	std::vector<std::vector<BYTE>>Sections;			// 存储所有节区数据

	HANDLE hFile;




	PETamper(HANDLE hFile);
	~PETamper();
	//bool FileHeaderTamper(HANDLE hFile, unsigned char* buffer, int size, int ObjectFlag, int DecisionFlag);
	bool SectionTamperA(HANDLE hFile, LONG Point, unsigned char* buffer);
	bool TextSectionTamperA(HANDLE hFile, unsigned char* buffer, DWORD EntryPoint);
	bool EntryPointCoverA(HANDLE hFile, DWORD EntryPoint);
	//bool HeaderInfoIni(std::vector<FieldInfo> HEADER_INFO);
	bool FieldTamper(PVOID object, LONG Point, char* buffer);
	bool DOSFieldTamper(PVOID object, LONG Point, std::string buffer);
	//bool AtomTamper(PVOID Struct, LONG Point, unsigned char* buffer, int size);  //obsolete

	bool RawSizeNRawAddressAdjust();
	bool SectionBin();
	string AllBin();

	bool Assembly(HANDLE hpFile);

};
