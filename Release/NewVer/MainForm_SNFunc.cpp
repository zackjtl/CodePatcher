void TMainFrm::ProcessSerialNumBeforeCreateCard()
{
	CIniFile  serialNumIniFile(_SerialNumberFilePath, FILE_OPEN_READ_WRITE);
	memset(&_CardStatus, 0, sizeof(_CardStatus));

	for (uint PortNum = 0; PortNum < MAX_PORT; ++PortNum) {
		wstring port = L"Port" + IntegerToWideString(PortNum);
		_CardStatus.SerialNum[PortNum]      = serialNumIniFile.ReadInteger(port, L"SerialNum", 0);
		_CardStatus.PortUsed[PortNum]       = serialNumIniFile.ReadInteger(port, L"PortUsed", 0);
		_CardStatus.OpenCardFinish[PortNum] = serialNumIniFile.ReadInteger(port, L"OpenCardFinish", 0);
		_CardStatus.PassOrFail[PortNum]     = serialNumIniFile.ReadInteger(port, L"PassOrFail", 0);
	}

	bool AP_Crash = false;
	for (uint PortNum = 0; PortNum < MAX_PORT; ++PortNum) {
		if ((_CardStatus.PortUsed[PortNum] == 1) && (_CardStatus.OpenCardFinish[PortNum] == 0)) {
			AP_Crash = true;
			break;
		}
	}

	if (AP_Crash == true) {
		vector<dword> totalItems;//40個Port的(當機 + Fail)值 + Recycling的值
		vector<dword> newItems;  //40個Port的(當機 + Fail)值
		vector<dword> oldItems;  //Recycling的值
		serialNumIniFile.ReadIntegers(L"Serial Num", L"Recycling", oldItems);

		for (uint PortNum = 0; PortNum < MAX_PORT; ++PortNum) {
			if ((_CardStatus.OpenCardFinish[PortNum] == 1) && (_CardStatus.PassOrFail[PortNum] == 1)) {

				vector<dword>::iterator it;
				for (it = oldItems.begin(); it != oldItems.end(); ++it) {
					if (_CardStatus.SerialNum[PortNum] == *it) {
						oldItems.erase(it);
						break;
					}
				}
			}
			if ( ((_CardStatus.OpenCardFinish[PortNum] == 0) && (_CardStatus.PortUsed[PortNum] == 1) && (_CardStatus.PassOrFail[PortNum] == 0)) ||
					 ((_CardStatus.OpenCardFinish[PortNum] == 1) && (_CardStatus.PortUsed[PortNum] == 1) && (_CardStatus.PassOrFail[PortNum] == 0)) ) {
				newItems.push_back(_CardStatus.SerialNum[PortNum]);
			}
		}

		sort(newItems.begin(), newItems.end());
		sort(oldItems.begin(), oldItems.end());
		set_union(newItems.begin(), newItems.end(), oldItems.begin(), oldItems.end(), std::inserter(totalItems, totalItems.begin()));
		serialNumIniFile.WriteIntegers(L"Serial Num", L"Recycling", totalItems);

		dword maxSerialNum = 0;
		for (uint PortNum = 0; PortNum < MAX_PORT; ++PortNum) {
			if (_CardStatus.PortUsed[PortNum] == true) {
				if (maxSerialNum < _CardStatus.SerialNum[PortNum]) {
					maxSerialNum = _CardStatus.SerialNum[PortNum];
				}
			}
		}
		if (maxSerialNum != 0) {
			serialNumIniFile.WriteInteger(L"Serial Num", L"Start", maxSerialNum + 1);
		}

		serialNumIniFile.UpdateFile();
	}

	Clear_40PortData_InSerialNum();
}
//---------------------------------------------------------------------------
void TMainFrm::Update_40PortData_InSerialNum(CCardTestThread* CardTestThread)
{
	CIniFile  serialNumIniFile(_SerialNumberFilePath, FILE_OPEN_READ_WRITE);
	uint readerSerialNum = CardTestThread->GetReaderSerialNum();

	wstring port = L"Port" + IntegerToWideString(readerSerialNum);
	serialNumIniFile.WriteInteger(port, L"OpenCardFinish", true);
	serialNumIniFile.WriteInteger(port, L"PassOrFail", CardTestThread->GetError().GetCode() ? false : true);
	serialNumIniFile.UpdateFile();
}
//---------------------------------------------------------------------------
void TMainFrm::Clear_40PortData_InSerialNum()
{
	CIniFile  serialNumIniFile(_SerialNumberFilePath, FILE_OPEN_READ_WRITE);
	memset(&_CardStatus, 0, sizeof(_CardStatus));

	for (uint PortNum = 0; PortNum < MAX_PORT; ++PortNum) {
		wstring port = L"Port" + IntegerToWideString(PortNum);
		serialNumIniFile.WriteInteger(port, L"SerialNum",      _CardStatus.SerialNum[PortNum]);
		serialNumIniFile.WriteInteger(port, L"PortUsed",       _CardStatus.PortUsed[PortNum]);
		serialNumIniFile.WriteInteger(port, L"OpenCardFinish", _CardStatus.OpenCardFinish[PortNum]);
		serialNumIniFile.WriteInteger(port, L"PassOrFail",     _CardStatus.PassOrFail[PortNum]);
	}
	serialNumIniFile.UpdateFile();
}
//---------------------------------------------------------------------------
void TMainFrm::Clear_RecyclingValue_InSerialNum()
{
	list<dword> recycling;
	SerialNum.GetRecycling(recycling);

	vector<dword>  items(recycling.begin(), recycling.end());

	CIniFile  serialNumIniFile(_SerialNumberFilePath, FILE_OPEN_READ_WRITE);
	serialNumIniFile.WriteIntegers(L"Serial Num", L"Recycling", items);
	serialNumIniFile.UpdateFile();
}
//---------------------------------------------------------------------------
void TMainFrm::Update_Start_InSerialNum()
{
	CIniFile  serialNumIniFile(_SerialNumberFilePath, FILE_OPEN_READ_WRITE);
	memset(&_CardStatus, 0, sizeof(_CardStatus));

	for (uint PortNum = 0; PortNum < MAX_PORT; ++PortNum) {
		wstring port = L"Port" + IntegerToWideString(PortNum);
		_CardStatus.SerialNum[PortNum]      = serialNumIniFile.ReadInteger(port, L"SerialNum", 0);
		_CardStatus.PortUsed[PortNum]       = serialNumIniFile.ReadInteger(port, L"PortUsed", 0);
		_CardStatus.OpenCardFinish[PortNum] = serialNumIniFile.ReadInteger(port, L"OpenCardFinish", 0);

	}

	dword maxSerialNum = 0;
	for (uint PortNum = 0; PortNum < MAX_PORT; ++PortNum) {
		if ( ((_CardStatus.OpenCardFinish[PortNum] == 1) && (_CardStatus.PortUsed[PortNum] == 1) && (_CardStatus.PassOrFail[PortNum] == 1)) ||
				 ((_CardStatus.OpenCardFinish[PortNum] == 1) && (_CardStatus.PortUsed[PortNum] == 1) && (_CardStatus.PassOrFail[PortNum] == 0)) ) {
			if (maxSerialNum < _CardStatus.SerialNum[PortNum]) {
				maxSerialNum = _CardStatus.SerialNum[PortNum];
			}
		}
	}

	serialNumIniFile.WriteInteger(L"Serial Num", L"Start", maxSerialNum + 1);
	serialNumIniFile.UpdateFile();
}
//---------------------------------------------------------------------------