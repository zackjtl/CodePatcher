void CCardOpen::Save_40PortData_InSerialNum(dword CardSerialNum)
{
	wstring appExePath  = ExtractFileDir(Application->ExeName).c_str();
	CIniFile iniFile(appExePath + L"\\Sys\\Config\\MPTool.ini");
	wstring serialNumberFilePath = iniFile.ReadString(L"Settings", L"Serial Number File");

	CIniFile  serialNumIniFile;

	if (serialNumberFilePath == L"") {
		serialNumberFilePath = _AppExePath + L"\\Sys\\Config\\SerialNum.ini";
	}

	if (!FileExists(serialNumberFilePath)) {
		serialNumIniFile.Open(serialNumberFilePath, FILE_CREATE_NEW);
	}
	else {
		serialNumIniFile.Open(serialNumberFilePath, FILE_OPEN_READ_WRITE);
	}
	TCardStatus cardStatus;
	memset(&cardStatus, 0, sizeof(cardStatus));
	cardStatus.SerialNum[_DeviceNo] = CardSerialNum;
	cardStatus.PortUsed[_DeviceNo]  = true;

	SerialNum.SavePortDetailInfo(serialNumIniFile, cardStatus, _DeviceNo);
}
//-----------------------------------------------------------------------------