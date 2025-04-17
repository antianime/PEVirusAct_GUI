#include"PETamper.h"

using namespace std;

unsigned char check[] = "CHECKCHECK";
/*
IMAGE_DOS_HEADER idh;           //DOS头（以"MZ"开头）
IMAGE_NT_HEADERS inh;			//NT头（PE签名、文件头、可选头）

WORD NumberOfSections;			// 节区数量

// 节区相关数据结构
std::vector<IMAGE_SECTION_HEADER> SectionHeaders;	// 存储所有节区头
//std::vector<BYTE[8]> SectionNames;
//std::vector<BYTE*> SectionNames;
std::vector<std::vector<BYTE>>SectionNames;			// 存储所有节区名称
std::vector<BYTE> stubbuffer;

*/

					// 存储DOS存根数据

unsigned char shellcode[];							// 存储shellcode代码




//const std::vector<std::vector<FieldInfo>>ALL_HEADERS_INFO = { IMAGE_DOS_HEADER_INFO,IMAGE_NT_HEADERS_INFO,IMAGE_SECTION_HEADERS_INFO };

PETamper::PETamper(HANDLE hFile)
{
	unsigned long NumberOfBytesRead;	// 读取字节数计数
	unsigned char MZSignal[2];			// 用于检查"MZ"签名的缓冲区

	// 打开PE文件，获取文件句柄
	
	this->hFile = hFile;
	

	// 检查PE文件开头的"MZ"签名（DOS头签名）
	SetFilePointer(hFile, 0, NULL, 0);		// 将文件指针移动到文件开头
	if (!ReadFile(hFile, MZSignal, sizeof(MZSignal), &NumberOfBytesRead, NULL))
	{
		cout << "ReadFile failed at MZSignal!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return;
	}
	if (MZSignal[0] != 'M' || MZSignal[1] != 'Z')		// 检查签名是否正确
	{
		cout << "This is not a PE file!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return;
	}

	// 读取DOS头（IMAGE_DOS_HEADER结构）
	SetFilePointer(hFile, 0, NULL, 0);			// 重置文件指针到开头
	if (!ReadFile(hFile, &idh, sizeof(IMAGE_DOS_HEADER), &NumberOfBytesRead, NULL))
	{
		cout << "ReadFile failed at IMAGE_DOS_HEADER!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return;
	}

	

	// 读取DOS存根,  其中e_lfanew是从DOS头到PE签名的偏移量
	//char* stubbuffer = new char[idh.e_lfanew - sizeof(IMAGE_DOS_HEADER)];
	stubbuffer.resize(idh.e_lfanew - sizeof(IMAGE_DOS_HEADER));

	SetFilePointer(hFile, sizeof(IMAGE_DOS_HEADER), NULL, 0);		// 移动到DOS头之后

	if (!ReadFile(hFile, stubbuffer.data(), stubbuffer.size(), &NumberOfBytesRead, NULL))
	{
		cout << "ReadFile failed at stubbuffer!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return;
	}


	// 读取NT头（PE签名和头信息),  其中e_lfanew是从DOS头到PE签名的偏移量
	SetFilePointer(hFile, idh.e_lfanew, NULL, 0);
	if (!ReadFile(hFile, &inh, sizeof(IMAGE_NT_HEADERS), &NumberOfBytesRead, NULL))
	{
		cout << "ReadFile failed at IMAGE_NT_HEADERS!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return;
	}

	// 从文件头获取节区数量（存储在文件头的NumberOfSections字段）
	NumberOfSections = inh.FileHeader.NumberOfSections;
	SectionHeaders.resize(NumberOfSections);		// 调整节区头向量大小
	SectionNames.resize(NumberOfSections);			// 调整节区名称向量大小
	Sections.resize(NumberOfSections);				// 调整节区数据向量大小

	// 计算节区头的位置：
	// DOS头 + PE签名(4字节) + 文件头(20字节) + 可选头大小
	SetFilePointer(hFile, idh.e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + inh.FileHeader.SizeOfOptionalHeader, NULL, 0);

	// 读取所有节区头信息
	for (WORD i = 0; i < NumberOfSections; i++)
	{
		if (!ReadFile(hFile, &SectionHeaders[i], sizeof(IMAGE_SECTION_HEADER), &NumberOfBytesRead, NULL))
		{
			cout << "ReadFile failed at SectionHeaders " << i << endl;
			cout << "GetLastError: " << GetLastError() << endl;
			return;
		}

		// 打印节区名称（8字节以null填充的字符串）
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
		SetFilePointer(hFile, SectionHeaders[i].PointerToRawData, NULL, 0);	// 移动到节区数据的起始位置
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



/* 组装PE文件函数 */




std::string PETamper::Assembly()
{
	std::string result;
	std::string temp;
	RawSizeNRawAddressAdjust();


	// 1. 写入DOS头

	// 3. 处理PE头对齐
	if (sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() > idh.e_lfanew)
	{
		// 计算4字节对齐的偏移量
		idh.e_lfanew += 4 * ((sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() + 3) % 4);//PE头按0x4对齐
	}

	

	temp.assign(reinterpret_cast<char*>(&idh), sizeof(idh));
	result += temp;

	temp.assign(reinterpret_cast<char*>(&stubbuffer), sizeof(stubbuffer));
	result += temp;

	// 4. 如果需要，填充空白区域
	if (sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() < idh.e_lfanew)//填充0x0
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


	// 1. 写入DOS头
	SetFilePointer(hFile, 0, NULL, 0);
	if (!WriteFile(hFile, &idh, sizeof(IMAGE_DOS_HEADER), &NumberOfBytesRead, NULL))
	{
		cout << "WriteFile failed at IMAGE_DOS_HEADER!" << endl;
		cout << "GetLastError: " << GetLastError() << endl;
		return false;
	}

	//cout << "NumberOfBytesRead" << NumberOfBytesRead << endl;
	//stubbuffer.data();

	// 2. 写入DOS存根
	SetFilePointer(hFile, sizeof(IMAGE_DOS_HEADER), NULL, 0);		// 移动到DOS头之后
	WriteFile(hFile, stubbuffer.data(), stubbuffer.size(), NULL, NULL);
	
	// 3. 处理PE头对齐
	if (sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() > idh.e_lfanew)
	{
		// 计算4字节对齐的偏移量
		idh.e_lfanew += 4 * ((sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() + 3) % 4);//PE头按0x4对齐
	}

	// 4. 如果需要，填充空白区域
	if (sizeof(IMAGE_DOS_HEADER) + stubbuffer.size() < idh.e_lfanew)//填充0x0
	{
		FillStr.resize(idh.e_lfanew - sizeof(IMAGE_DOS_HEADER) - stubbuffer.size());
		std::fill(FillStr.begin(), FillStr.end(), 0); // Fill the string with null characters
		WriteFile(hFile, FillStr.data(), FillStr.length(), NULL, NULL);
	}

	// 5. 写入NT头
	WriteFile(hFile, &inh, sizeof(IMAGE_NT_HEADERS), NULL, NULL);

	//文件对齐，空隙

	// 6. 写入所有节区头
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


// 初始化字段信息结构体向量
// 根据每个字段的大小计算其在结构体中的偏移量
bool HeaderInfoIni(vector<FieldInfo>HEADER_INFO)
{
	// 遍历所有字段信息
	for (int i = 0; i < HEADER_INFO.size(); i++)
	{
		if (i == 0)
			HEADER_INFO[i].offset = 0;		// 第一个字段偏移量为0
		else
			// 后续字段偏移量 = 前一个字段偏移量 + 前一个字段大小
			HEADER_INFO[i].offset = HEADER_INFO[i - 1].offset + HEADER_INFO[i - 1].size;
	}
	return true;

}

// 修改PE文件的入口点地址
bool PETamper::EntryPointCoverA(HANDLE hpFile, DWORD EntryPoint)
{
	unsigned long NumberOfBytesRead;

	// 将文件指针移动到PE头中的入口点地址位置
	SetFilePointer(hpFile, inh.OptionalHeader.AddressOfEntryPoint, NULL, 0);

	// 写入新的入口点地址
	WriteFile(hpFile, &EntryPoint, sizeof(EntryPoint), &NumberOfBytesRead, NULL);

	 // 检查操作是否成功
	if (GetLastError() != ERROR_SUCCESS)
	{
		std::cout << "ERROR:  " << GetLastError() << endl;
		return false;
	}
	else
		return true;
}


// 修改PE文件指定节区的数据
bool PETamper::SectionTamperA(HANDLE hpFile, LONG Point, unsigned char* buffer)
{
	unsigned long NumberOfBytesRead;

	// 将文件指针移动到指定位置
	SetFilePointer(hpFile, Point, NULL, 0);

	// 写入新的节区数据
	WriteFile(hpFile, buffer, sizeof(buffer), &NumberOfBytesRead, NULL);

	// 检查操作是否成功
	if (GetLastError() != ERROR_SUCCESS)
	{
		std::cout << "ERROR:  " << GetLastError() << endl;
		return false;
	}
	else
		return true;

}


// 修改DOS头中的特定字段
bool PETamper::DOSFieldTamper(PVOID object, LONG Point, string buffer)
{
	// 检查点位置是否有效
	if (Point > IMAGE_DOS_HEADER_INFO.size())
	{
		cout << "POINT ERROR! at DOSFieldTamper" << endl;
		return false;
	}

	// 检查缓冲区大小是否超过字段大小
	if (IMAGE_DOS_HEADER_INFO[Point].size < buffer.size())
	{
		cout << "BUFFER LENGTH ERROR! at DOSFieldTamper" << endl;
		return false;
	}

	// 用0填充缓冲区使其达到字段大小
	buffer.append(IMAGE_DOS_HEADER_INFO[Point].size - buffer.size(), 0); //填充0x0

	// 将缓冲区数据复制到DOS头的指定字段
	memmove(&idh + IMAGE_DOS_HEADER_INFO[Point].offset, buffer.c_str(), IMAGE_DOS_HEADER_INFO[Point].size);

	return true;
}

// 通用字段修改函数
bool PETamper::FieldTamper(PVOID object, LONG Point, char* buffer)
{
	
	// 将缓冲区数据复制到对象的指定位置
	memmove((PVOID)((LONGLONG)object + (LONGLONG)Point), buffer, sizeof(buffer));

	return true;
}

// 修改.text节区数据并更新入口点
bool PETamper::TextSectionTamperA(HANDLE hpFile, unsigned char* buffer, DWORD EntryPoint)
{
	unsigned long NumberOfBytesRead;
	DWORD PointerToRawPointer;

	// 遍历所有节区
	for (WORD i = 0; i < NumberOfSections; i++)
	{
		// 查找.text节区
		if (!strcmp((const char*)SectionNames[i].data(), ".text"))
		{
			// 修改.text节区数据
			SectionTamperA(hpFile, SectionHeaders[i].PointerToRawData, buffer);

			// 更新入口点地址
			EntryPointCoverA(hpFile, EntryPoint);
			return true;
		}
	}
	return false;
}


// 调整节区的原始大小和原始地址
bool PETamper::RawSizeNRawAddressAdjust()
{
	// 计算节区的SizeOfRawData和PointerToRawData
	for (WORD i = 0; i < NumberOfSections; i++)
	{

		// 设置节区原始大小为实际数据大小
		SectionHeaders[i].SizeOfRawData = Sections[i].size();
	}

	// 调整NT头位置
	if (idh.e_lfanew < sizeof(IMAGE_DOS_HEADER) + stubbuffer.size())
	{
		idh.e_lfanew = sizeof(IMAGE_DOS_HEADER) + stubbuffer.size();
	}

	string zerobuffer;
	int tempsize;

	// 调整每个节区的文件对齐和原始指针
	for (int i = 0; i < NumberOfSections; i++)
	{
		//节对齐

		// 计算当前节区大小与文件对齐的余数
		tempsize = Sections[i].size() % inh.OptionalHeader.FileAlignment;
		if (SectionHeaders[i].SizeOfRawData != 0)
		{
			// 如果需要对齐，填充0直到对齐
			if (tempsize != 0)
			{
				Sections[i].resize(Sections[i].size() + inh.OptionalHeader.FileAlignment - tempsize, '\0');
				SectionHeaders[i].SizeOfRawData = Sections[i].size();
			}
				
			// 设置节区在文件中的原始位置
			if (i == 0 || SectionHeaders[i - 1].SizeOfRawData == 0 || SectionHeaders[i - 1].PointerToRawData == 0)
			{
				// 计算第一个节区的对齐
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
				// 后续节区的位置 = 前一个节区位置 + 前一个节区大小
				SectionHeaders[i].PointerToRawData = SectionHeaders[i - 1].PointerToRawData + SectionHeaders[i - 1].SizeOfRawData;
			}
		}
			

			
	}

	return true;
}


// 读取整个文件内容到字符串
string PETamper::AllBin()
{
	// 获取文件大小
	DWORD Filesize = GetFileSize(hFile, NULL);
	//std::string FileBin.resize(Filesize);
	std::string FileBin;

	// 调整字符串大小以容纳整个文件
	FileBin.resize(Filesize);

	// 将文件指针移动到开头
	SetFilePointer(hFile, 0, 0, NULL);

	// 读取整个文件内容
	if (!ReadFile(hFile, FileBin.data(), Filesize, NULL, NULL))
	{
		qDebug() << "ReadFileError at SHOWSOURCEHEX" << GetLastError();
		return 0;
	}

	// 添加字符串结束符
	
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

