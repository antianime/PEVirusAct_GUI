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




std::string PETamper::Assembly()
{
	std::string result;
	std::string temp;
	RawSizeNRawAddressAdjust();


	// 1. д��DOSͷ

	// 3. ����PEͷ����
	if (sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() > idh.e_lfanew)
	{
		// ����4�ֽڶ����ƫ����
		idh.e_lfanew += 4 * ((sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() + 3) % 4);//PEͷ��0x4����
	}

	

	temp.assign(reinterpret_cast<char*>(&idh), sizeof(idh));
	result += temp;

	temp.assign(reinterpret_cast<char*>(&stubbuffer), sizeof(stubbuffer));
	result += temp;

	// 4. �����Ҫ�����հ�����
	if (sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() < idh.e_lfanew)//���0x0
	{
		temp.resize(idh.e_lfanew - sizeof(IMAGE_DOS_HEADER) - stubbuffer.size());
		std::fill(temp.begin(), temp.end(), 0); // Fill the string with null characters
		result += temp;
		//WriteFile(hFile, temp.data(), temp.length(), NULL, NULL);
	}

	temp.assign(reinterpret_cast<char*>(&inh), sizeof(inh));
	result += temp;
	

	for (int i = 0; i < NumberOfSections; i++)
	{
		temp.assign(reinterpret_cast<char*>(&SectionHeaders[i]), sizeof(SectionHeaders[i]));
		result += temp;
	}


	for (WORD i = 0; i < NumberOfSections; i++)
	{
		if (!strcmp((const char*)SectionNames[i].data(), ".text"))
		{
			if (SectionHeaders[i].PointerToRawData > idh.e_lfanew + sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER) * NumberOfSections)
			{
				temp.resize(SectionHeaders[i].PointerToRawData - idh.e_lfanew - sizeof(IMAGE_NT_HEADERS) - sizeof(IMAGE_SECTION_HEADER) * NumberOfSections);
				std::fill(temp.begin(), temp.end(), 0);
				result += temp;
			}

		}
	}
	//Write Sections
	for (WORD i = 0; i < NumberOfSections; i++)
	{
		temp.assign(reinterpret_cast<char*>(Sections[i].data()), Sections[i].size());
		result += temp;
		//WriteFile(hFile, Sections[i].data(), Sections[i].size(), NULL, NULL);
	}

	return result;
}





bool PETamper::AssemblyA(HANDLE hFile)
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
	}

	// ����NTͷλ��
	if (idh.e_lfanew < sizeof(IMAGE_DOS_HEADER) + stubbuffer.size())
	{
		idh.e_lfanew = sizeof(IMAGE_DOS_HEADER) + stubbuffer.size();
	}

	string zerobuffer;
	int tempsize;

	// ����ÿ���������ļ������ԭʼָ��
	for (int i = 0; i < NumberOfSections; i++)
	{
		//�ڶ���

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


bool PETamper::ShellcodeInjection(char* buffer)
{
	for (int i = 0; i < NumberOfSections; i++)
	{
		if (strcmp((const char*)SectionNames[i].data(), ".text"))
		{
			Sections[i].resize(sizeof(buffer));
			memmove(Sections[i].data(), buffer, sizeof(buffer));
			return 0;
		}
	}
	return 1;
}

