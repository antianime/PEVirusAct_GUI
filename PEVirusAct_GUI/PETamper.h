#pragma once
#include"fh.h"


class PETamper
{
public:
	IMAGE_DOS_HEADER idh;           //DOSͷ����"MZ"��ͷ��
	IMAGE_NT_HEADERS inh;			//NTͷ��PEǩ�����ļ�ͷ����ѡͷ��
	std::vector<BYTE> stubbuffer;						// �洢DOS�������
	// ����������ݽṹ
	WORD NumberOfSections;
	std::vector<IMAGE_SECTION_HEADER> SectionHeaders;	// �洢���н���ͷ
	std::vector<std::vector<BYTE>>SectionNames;			// �洢���н�������
	std::vector<std::vector<BYTE>>Sections;			// �洢���н�������

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
