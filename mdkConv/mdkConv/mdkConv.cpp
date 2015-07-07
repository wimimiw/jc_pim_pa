// mdkConv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "windows.h"
#include "string"
#include "iostream"
#include "fstream"
#include "vector"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	//生成文件名
	int secSize = 200;
	wchar_t pathBuf[MAX_PATH];

	GetModuleFileName(NULL, pathBuf,MAX_PATH);

	wstring ifile(pathBuf);
	wstring ofile(pathBuf);
	wstring oBinfile(pathBuf);

	ifile = ifile.substr(0, ifile.rfind(L"\\"));
	ofile = ofile.substr(0, ofile.rfind(L"\\"));
	oBinfile = oBinfile.substr(0, oBinfile.rfind(L"\\"));

	SYSTEMTIME st;
	GetLocalTime(&st);
	wchar_t timeBuf[256];

	swprintf_s(timeBuf,L".-.[%04d-%02d-%02d-%02d-%02d-%02d]",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond);
		
	wstring strTmp;

	ifile.append(argv[1]);
	strTmp.assign(argv[2]);strTmp.append(timeBuf);
	//ifile.append(L"\\rf_module.hex");
	//strTmp.assign(ofile); strTmp.append(timeBuf);

	ofile.assign(strTmp);
	oBinfile.assign(strTmp);	

	//导入源文件 
	wifstream wifile(ifile);	

	wchar_t buf[516];
	wstring strBegin,strEnd = L":00000001FF";
	vector<UCHAR> vcFileStream;

	while (!wifile.eof())
	{
		UCHAR len = 0, type = 0, data = 0, chkBak, chk;
		USHORT addr = 0;
		UINT32 rdTemp;
		wstring temp;
		vector<char> vcBuf;

		wifile.getline(buf, 516);
		temp.assign(buf); 

		swscanf_s(temp.data() + 0, L"%01X", &rdTemp); //start
		swscanf_s(temp.data() + 1, L"%02X", &rdTemp); len = rdTemp;//len
		swscanf_s(temp.data() + 3, L"%04X", &rdTemp); addr = rdTemp;//addr
		swscanf_s(temp.data() + 7, L"%02X", &rdTemp); type = rdTemp;//type

		if (type == 0x04)
		{
			strBegin.assign(buf);
		}

		chk = len + (addr&0xff) + (addr>>8) + type;

		for (size_t i = 0; i < len+1; i++)
		{
			swscanf_s(temp.data() + 9 + i * 2, L"%02X", &rdTemp); data = rdTemp;
			vcBuf.push_back(data);
			if (i != len)
			{
				chk += data;
			}
		}

		chkBak = vcBuf[len];

		if (type == 0x00 && len > 0 && chkBak == (UCHAR)(256-chk))
		{
			vcBuf.pop_back();
			vcFileStream.insert(vcFileStream.end(), vcBuf.begin(), vcBuf.end());
		}
	}

	wifile.close();

	//计算校验和
	wstring strCheckSum;

	[&](){
		USHORT checkSum = 0;
		for (size_t i = 0; i < 0x7000; i++)
		{
			checkSum += vcFileStream[i];
		}

		wchar_t tBuf[20];
		swprintf_s(tBuf, L"%04X", checkSum);
		strCheckSum.assign(tBuf);
	}();

	//导出HEX文件
	ofile.append(L".-.[" + strCheckSum+L"].hex");
	wofstream wofile(ofile);
	wofile << strBegin << endl;
	
	for (size_t i = 0; i < vcFileStream.size(); i += secSize)
	{
		UCHAR chk;
		wchar_t tBuf[10];
		wstring wstrTemp;
				 
		wstrTemp.append(L":");
		swprintf_s(tBuf, L"%02X", secSize);
		wstrTemp.append(tBuf);				chk = secSize;
		swprintf_s(tBuf, L"%04X", i);		chk += (i & 0x00ff) + (i >> 8);
		wstrTemp.append(tBuf);			
		wstrTemp.append(L"00");

		for (size_t j = 0; j < secSize; j++)
		{
			if (i + j < vcFileStream.size())
			{
				swprintf_s(tBuf, L"%02X", vcFileStream[i + j]);
				chk += vcFileStream[i + j];
				wstrTemp.append(tBuf);
			}
		}		

		swprintf_s(tBuf, L"%02X", (UCHAR)(256-chk));

		wstrTemp.append(tBuf);

		wofile << wstrTemp << endl;
	}

	wofile << strEnd << endl;
	
	wofile.close();

	//转换成BIN文件
	//wofstream  不支持 binary
	oBinfile.append(L".-.[" + strCheckSum + L"].bin");
	ofstream oBinFileHandle(oBinfile, ios::binary);
	oBinFileHandle.write((char*)vcFileStream.data(), vcFileStream.size());
	oBinFileHandle.close();

	wcout << L"==>Source HEX : " << ifile << endl;
	wcout << L"==>Output HEX : " << ofile << endl;
	wcout << L"==>Output BIN : " << oBinfile << endl;

	return 0;
}

