		form->GetSerialNumber(SerialNum);
		SerialNum.SetRecycling(form->GetRecycleSN());
		MKBLicenseCount.SetCount(form->GetMKBLicenseCount());

		_SerialNumberFilePath = form->GetIniFilePath();

		UpdateSerialNumAndMKBLicenseCount();
		SaveSerialNumAndMKBLicenseCount();

		CIniFile iniFile(_AppExePath + L"\\Sys\\Config\\MPTool.ini");
		iniFile.WriteString(L"Settings", L"Serial Number File", _SerialNumberFilePath);
		iniFile.UpdateFile();