#include"PETamper.h"

using namespace std;

unsigned char check[] = "CHECKCHECK";
/*
IMAGE_DOS_HEADER idh;           //DOSͷ����"MZ"��ͷ��
IMAGE_NT_HEADERS inh;			//NTͷ��PEǩ�����ļ�ͷ����ѡͷ��

WORD NumberOfSections;			// ��������

// ����������ݽṹ
std::vector<IMAGE_SECTION_HEADER> SectionHeaders;	// �洢���н���ͷ
//std::vector<BYTE[8]> SectionNames;
//std::vector<BYTE*> SectionNames;
std::vector<std::vector<BYTE>>SectionNames;			// �洢���н�������
std::vector<BYTE> stubbuffer;

*/

					// �洢DOS�������

unsigned char shellcode[];							// �洢shellcode����


// DOSͷ�ֶ���Ϣ�����ơ���С��ƫ����)
const std::vector< FieldInfo>IMAGE_DOS_HEADER_INFO = {
	{"e_magic",2, 0x0},
	{"e_cblp",2,0x2} ,                 // Bytes on last page of file
	{ "e_cp",2, 0x4},                       // Pages in file
	{ "e_crlc", 2,0x6},                      // Relocations
	{ "e_cparhdr", 2,0x8},                  // Size of header in paragraphs
	{ "e_minalloc",2, 0xA},                  // Minimum extra paragraphs needed
	{ "e_maxalloc",2,0xC },                  // Maximum extra paragraphs needed
	{ "e_ss",2, 0xE},                       // Initial (relative) SS value
	{ "e_sp",2, 0x10},                        // Initial SP value
	{ "e_csum",2, 0x12},                     // Checksum
	{ "e_ip",2,0x14 },                      // Initial IP value
	{ "e_cs",2, 0x16},                     // Initial (relative) CS value
	{ "e_lfarlc",2,0x18 },                  // File address of relocation table
	{ "e_ovno",2,0x1A },                    // Overlay number
	{ "e_res", 8,0x1C},                // Reserved words
	{ "e_oemid",2,0x24 },                 // OEM identifier (for e_oeminfo)
	{ "e_oeminfo",2, 0x26},                // OEM information; e_oemid specific
	{ "e_res2",20,0x28 },               // Reserved words
	{ "e_lfanew",4,0x3C }
};

// NTͷ�ֶ���Ϣ
const std::vector<FieldInfo>Signature;
const std::vector<FieldInfo>IMAGE_FILE_HEADER_INFO;
const std::vector<FieldInfo>IMAGE_OPTIONAL_HEADER_INFO;

const std::vector<std::vector< FieldInfo>>IMAGE_NT_HEADERS_INFO = { Signature,IMAGE_FILE_HEADER_INFO,IMAGE_OPTIONAL_HEADER_INFO };


const std::vector< FieldInfo> IMAGE_SECTION_HEADERS_INFO = {
	{"Name",8,},
	{""},
};

//const std::vector<std::vector<FieldInfo>>ALL_HEADERS_INFO = { IMAGE_DOS_HEADER_INFO,IMAGE_NT_HEADERS_INFO,IMAGE_SECTION_HEADERS_INFO };

PETamper::PETamper(HANDLE hFile)
{
	unsigned long NumberOfBytesRead;	// ��ȡ�ֽ�������
	unsigned char MZSignal[2];			// ���ڼ��"MZ"ǩ���Ļ�����

	// ��PE�ļ�����ȡ�ļ����
	
	this->hFile = hFile;
	

	// ���PE�ļ���ͷ��"MZ"ǩ����DOSͷǩ����
	SetFilePointer(hFile, 0, NULL, 0);		// ���ļ�ָ���ƶ����ļ���ͷ
	if (!ReadFile(hFile, MZSignal, sizeof(MZSignal), &NumberOfBytesRead, NULL))
	{
		cout << "ReadFile failed at MZSignal!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return;
	}
	if (MZSignal[0] != 'M' || MZSignal[1] != 'Z')		// ���ǩ���Ƿ���ȷ
	{
		cout << "This is not a PE file!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return;
	}

	// ��ȡDOSͷ��IMAGE_DOS_HEADER�ṹ��
	SetFilePointer(hFile, 0, NULL, 0);			// �����ļ�ָ�뵽��ͷ
	if (!ReadFile(hFile, &idh, sizeof(IMAGE_DOS_HEADER), &NumberOfBytesRead, NULL))
	{
		cout << "ReadFile failed at IMAGE_DOS_HEADER!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return;
	}

	

	// ��ȡDOS���,  ����e_lfanew�Ǵ�DOSͷ��PEǩ����ƫ����
	//char* stubbuffer = new char[idh.e_lfanew - sizeof(IMAGE_DOS_HEADER)];
	stubbuffer.resize(idh.e_lfanew - sizeof(IMAGE_DOS_HEADER));

	SetFilePointer(hFile, sizeof(IMAGE_DOS_HEADER), NULL, 0);		// �ƶ���DOSͷ֮��

	if (!ReadFile(hFile, stubbuffer.data(), stubbuffer.size(), &NumberOfBytesRead, NULL))
	{
		cout << "ReadFile failed at stubbuffer!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return;
	}


	// ��ȡNTͷ��PEǩ����ͷ��Ϣ),  ����e_lfanew�Ǵ�DOSͷ��PEǩ����ƫ����
	SetFilePointer(hFile, idh.e_lfanew, NULL, 0);
	if (!ReadFile(hFile, &inh, sizeof(IMAGE_NT_HEADERS), &NumberOfBytesRead, NULL))
	{
		cout << "ReadFile failed at IMAGE_NT_HEADERS!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return;
	}

	// ���ļ�ͷ��ȡ�����������洢���ļ�ͷ��NumberOfSections�ֶΣ�
	NumberOfSections = inh.FileHeader.NumberOfSections;
	SectionHeaders.resize(NumberOfSections);		// ��������ͷ������С
	SectionNames.resize(NumberOfSections);			// ������������������С
	Sections.resize(NumberOfSections);				// ������������������С

	// �������ͷ��λ�ã�
	// DOSͷ + PEǩ��(4�ֽ�) + �ļ�ͷ(20�ֽ�) + ��ѡͷ��С
	SetFilePointer(hFile, idh.e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + inh.FileHeader.SizeOfOptionalHeader, NULL, 0);

	// ��ȡ���н���ͷ��Ϣ
	for (WORD i = 0; i < NumberOfSections; i++)
	{
		if (!ReadFile(hFile, &SectionHeaders[i], sizeof(IMAGE_SECTION_HEADER), &NumberOfBytesRead, NULL))
		{
			cout << "ReadFile failed at SectionHeaders " << i << endl;
			cout << "GetLastError: " << GetLastError() << endl;
			return;
		}

		// ��ӡ�������ƣ�8�ֽ���null�����ַ�����
		//cout << "Section Name: ";
		for (int j = 0; j < 8; j++)
		{
			SectionNames[i].resize(NumberOfSections);
			SectionNames[i][j] = SectionHeaders[i].Name[j];
			
			//cout << SectionNames[i][j];
		}
		
		//cout << endl;

	}
	for (WORD i = 0; i < NumberOfSections; i++)
	{
		Sections[i].resize(SectionHeaders[i].SizeOfRawData);
		SetFilePointer(hFile, SectionHeaders[i].PointerToRawData, NULL, 0);	// �ƶ����������ݵ���ʼλ��
		if (!ReadFile(hFile, Sections[i].data(), SectionHeaders[i].SizeOfRawData, &NumberOfBytesRead, NULL))
		{
			cout << "ReadFile failed at Sections " << i << " : " << SectionNames[i].data() << endl;
			cout << "GetLastError: " << GetLastError() << endl;
			return;
		}
	}

	

}

PETamper::~PETamper()
{
	CloseHandle(hFile);
}



/* ��װPE�ļ����� */
bool PETamper::Assembly(HANDLE hFile)
{
	unsigned long NumberOfBytesRead;
	std::string FillStr;

	//cout << idh.e_magic << endl;

	//adjust
	RawSizeNRawAddressAdjust();


	// 1. д��DOSͷ
	SetFilePointer(hFile, 0, NULL, 0);
	if (!WriteFile(hFile, &idh, sizeof(IMAGE_DOS_HEADER), &NumberOfBytesRead, NULL))
	{
		cout << "WriteFile failed at IMAGE_DOS_HEADER!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return false;
	}

	//cout << "NumberOfBytesRead" << NumberOfBytesRead << endl;
	//stubbuffer.data();

	// 2. д��DOS���
	SetFilePointer(hFile, sizeof(IMAGE_DOS_HEADER), NULL, 0);		// �ƶ���DOSͷ֮��
	WriteFile(hFile, stubbuffer.data(), stubbuffer.size(), NULL, NULL);
	
	// 3. ����PEͷ����
	if (sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() > idh.e_lfanew)
	{
		// ����4�ֽڶ����ƫ����
		idh.e_lfanew += 4 * ((sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() + 3) % 4);//PEͷ��0x4����
	}

	// 4. �����Ҫ�����հ�����
	if (sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() < idh.e_lfanew)//���0x0
	{
		FillStr.resize(idh.e_lfanew - sizeof(IMAGE_DOS_HEADER) - stubbuffer.size());
		std::fill(FillStr.begin(), FillStr.end(), 0); // Fill the string with null characters
		WriteFile(hFile, FillStr.data(), FillStr.length(), NULL, NULL);
	}

	// 5. д��NTͷ
	WriteFile(hFile, &inh, sizeof(IMAGE_NT_HEADERS), NULL, NULL);

	//�ļ����룬��϶

	// 6. д�����н���ͷ
	for (int i = 0; i < NumberOfSections; i++)
		WriteFile(hFile, &SectionHeaders[i], sizeof(IMAGE_SECTION_HEADER), NULL, NULL);
	//WriteFile(hFile, check, sizeof(check), NULL, NULL);

	
	for (WORD i = 0; i < NumberOfSections; i++)
	{




		//cout << SectionNames[i] << endl;
		if (!strcmp((const char*)SectionNames[i].data(), ".text"))
		{
			if (SectionHeaders[i].PointerToRawData > idh.e_lfanew + sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * NumberOfSections)
			{
				FillStr.resize(SectionHeaders[i].PointerToRawData - idh.e_lfanew - sizeof(IMAGE_NT_HEADERS) - sizeof(IMAGE_SECTION_HEADER) * NumberOfSections);
				std::fill(FillStr.begin(), FillStr.end(), 0); // Fill the string with null characters
				WriteFile(hFile, FillStr.data(), FillStr.size(), NULL, NULL);
			}
			
		}
	}
	//Write Sections
	for (WORD i = 0; i < NumberOfSections; i++)
	{
		WriteFile(hFile, Sections[i].data(), Sections[i].size(), NULL, NULL);
	}

	return true;
}


// ��ʼ���ֶ���Ϣ�ṹ������
// ����ÿ���ֶεĴ�С�������ڽṹ���е�ƫ����
bool HeaderInfoIni(vector<FieldInfo>HEADER_INFO)
{
	// ���������ֶ���Ϣ
	for (int i = 0; i < HEADER_INFO.size(); i++)
	{
		if (i == 0)
			HEADER_INFO[i].offset = 0;		// ��һ���ֶ�ƫ����Ϊ0
		else
			// �����ֶ�ƫ���� = ǰһ���ֶ�ƫ���� + ǰһ���ֶδ�С
			HEADER_INFO[i].offset = HEADER_INFO[i - 1].offset + HEADER_INFO[i - 1].size;
	}
	return true;

}

// �޸�PE�ļ�����ڵ��ַ
bool PETamper::EntryPointCoverA(HANDLE hpFile, DWORD EntryPoint)
{
	unsigned long NumberOfBytesRead;

	// ���ļ�ָ���ƶ���PEͷ�е���ڵ��ַλ��
	SetFilePointer(hpFile, inh.OptionalHeader.AddressOfEntryPoint, NULL, 0);

	// д���µ���ڵ��ַ
	WriteFile(hpFile, &EntryPoint, sizeof(EntryPoint), &NumberOfBytesRead, NULL);

	 // �������Ƿ�ɹ�
	if (GetLastError() != ERROR_SUCCESS)
	{
		std::cout << "ERROR:  " << GetLastError() << endl;
		return false;
	}
	else
		return true;
}


// �޸�PE�ļ�ָ������������
bool PETamper::SectionTamperA(HANDLE hpFile, LONG Point, unsigned char* buffer)
{
	unsigned long NumberOfBytesRead;

	// ���ļ�ָ���ƶ���ָ��λ��
	SetFilePointer(hpFile, Point, NULL, 0);

	// д���µĽ�������
	WriteFile(hpFile, buffer, sizeof(buffer), &NumberOfBytesRead, NULL);

	// �������Ƿ�ɹ�
	if (GetLastError() != ERROR_SUCCESS)
	{
		std::cout << "ERROR:  " << GetLastError() << endl;
		return false;
	}
	else
		return true;

}


// �޸�DOSͷ�е��ض��ֶ�
bool PETamper::DOSFieldTamper(PVOID object, LONG Point, string buffer)
{
	// ����λ���Ƿ���Ч
	if (Point > IMAGE_DOS_HEADER_INFO.size())
	{
		cout << "POINT ERROR! at DOSFieldTamper" << endl;
		return false;
	}

	// ��黺������С�Ƿ񳬹��ֶδ�С
	if (IMAGE_DOS_HEADER_INFO[Point].size < buffer.size())
	{
		cout << "BUFFER LENGTH ERROR! at DOSFieldTamper" << endl;
		return false;
	}

	// ��0��仺����ʹ��ﵽ�ֶδ�С
	buffer.append(IMAGE_DOS_HEADER_INFO[Point].size - buffer.size(), 0); //���0x0

	// �����������ݸ��Ƶ�DOSͷ��ָ���ֶ�
	memmove(&idh + IMAGE_DOS_HEADER_INFO[Point].offset, buffer.c_str(), IMAGE_DOS_HEADER_INFO[Point].size);

	return true;
}

// ͨ���ֶ��޸ĺ���
bool PETamper::FieldTamper(PVOID object, LONG Point, char* buffer)
{
	
	// �����������ݸ��Ƶ������ָ��λ��
	memmove((PVOID)((LONGLONG)object + (LONGLONG)Point), buffer, sizeof(buffer));

	return true;
}

// �޸�.text�������ݲ�������ڵ�
bool PETamper::TextSectionTamperA(HANDLE hpFile, unsigned char* buffer, DWORD EntryPoint)
{
	unsigned long NumberOfBytesRead;
	DWORD PointerToRawPointer;

	// �������н���
	for (WORD i = 0; i < NumberOfSections; i++)
	{
		// ����.text����
		if (!strcmp((const char*)SectionNames[i].data(), ".text"))
		{
			// �޸�.text��������
			SectionTamperA(hpFile, SectionHeaders[i].PointerToRawData, buffer);

			// ������ڵ��ַ
			EntryPointCoverA(hpFile, EntryPoint);
			return true;
		}
	}
	return false;
}


// ����������ԭʼ��С��ԭʼ��ַ
bool PETamper::RawSizeNRawAddressAdjust()
{
	// ���������SizeOfRawData��PointerToRawData
	for (WORD i = 0; i < NumberOfSections; i++)
	{

		// ���ý���ԭʼ��СΪʵ�����ݴ�С
		SectionHeaders[i].SizeOfRawData = Sections[i].size();
		//SectionHeaders[i].PointerToRawData = idh.e_lfanew + sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * NumberOfSections + stubbuffer.size() + sizeof(IMAGE_DOS_HEADER);
	}

	//Adjust PointerToRawData

	//NT Headers
	// ����NTͷλ��
	if (idh.e_lfanew < sizeof(IMAGE_DOS_HEADER) + stubbuffer.size())
	{
		idh.e_lfanew = sizeof(IMAGE_DOS_HEADER) + stubbuffer.size();
	}

	//Section Headers
	//if()

	//Sections
	string zerobuffer;
	int tempsize;

	// ����ÿ���������ļ������ԭʼָ��
	for (int i = 0; i < NumberOfSections; i++)
	{
		//�ڶ���
		//Sections[i].resize((SectionHeaders[i].SizeOfRawData + 199) / 200 * 200, '\0');

		// ���㵱ǰ������С���ļ����������
		tempsize = Sections[i].size() % inh.OptionalHeader.FileAlignment;
		if (SectionHeaders[i].SizeOfRawData != 0)
		{
			// �����Ҫ���룬���0ֱ������
			if (tempsize != 0)
			{
				Sections[i].resize(Sections[i].size() + inh.OptionalHeader.FileAlignment - tempsize, '\0');
				SectionHeaders[i].SizeOfRawData = Sections[i].size();
			}
				
			// ���ý������ļ��е�ԭʼλ��
			if (i == 0 || SectionHeaders[i - 1].SizeOfRawData == 0 || SectionHeaders[i - 1].PointerToRawData == 0)
			{
				// �����һ�������Ķ���
				tempsize = (idh.e_lfanew + sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER)) % inh.OptionalHeader.FileAlignment;
				if (tempsize != 0)
				{
					SectionHeaders[i].PointerToRawData = inh.OptionalHeader.FileAlignment + (idh.e_lfanew + sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER)) - tempsize;
					//Sections[i].insert(Sections[i].begin(), zerobuffer.begin(), zerobuffer.end());
					//SectionHeaders[i].PointerToRawData = idh.e_lfanew + sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * NumberOfSections + stubbuffer.size() + sizeof(IMAGE_DOS_HEADER);
				}
			}
			else
			{
				// ����������λ�� = ǰһ������λ�� + ǰһ��������С
				SectionHeaders[i].PointerToRawData = SectionHeaders[i - 1].PointerToRawData + SectionHeaders[i - 1].SizeOfRawData;
			}
		}
			

			
	}
		
		
	


	return true;
}


// ��ȡ�����ļ����ݵ��ַ���
string PETamper::AllBin()
{
	// ��ȡ�ļ���С
	DWORD Filesize = GetFileSize(hFile, NULL);
	//std::string FileBin.resize(Filesize);
	std::string FileBin;

	// �����ַ�����С�����������ļ�
	FileBin.resize(Filesize);

	// ���ļ�ָ���ƶ�����ͷ
	SetFilePointer(hFile, 0, 0, NULL);

	// ��ȡ�����ļ�����
	if (!ReadFile(hFile, FileBin.data(), Filesize, NULL, NULL))
	{
		qDebug() << "ReadFileError at SHOWSOURCEHEX" << GetLastError();
		return 0;
	}

	// ����ַ���������
	
	FileBin += '\0';
	return FileBin;
}

/*
bool AtomTamper(PVOID Struct, LONG Point, unsigned char* buffer, int size)  //obsolete
{
	memmove((PVOID)((LONGLONG)Struct + (LONGLONG)Point), buffer, size);
	if (GetLastError() != ERROR_SUCCESS)
	{
		cout << "ERROR at AtomTamper: " << GetLastError() << endl;
		return false;
	}
	else
		return true;

}

*/


QString StdStringToStructHexQString(std::string str)
{
	QByteArray byteArray(str.data(), str.size());


	QString hexString = byteArray.toHex(' ').toUpper();

	QString StructHexString;
	for (int i = 0; i < hexString.size(); i++)
	{
		StructHexString += hexString[i];
		if ((i + 1) % 48 == 0)
			StructHexString += '\n';

	}
	return StructHexString;
}
