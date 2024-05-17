#include "Pch.h"
#pragma hdrstop


#include "TestStationVersion.h"
#include "TypeConv.h"
//---------------------------------------------------------------------------
void CTestStationVersion::Create(CIniFile& IniFile, uint CustomerID)
{
  //wstring          text = IniFile.ReadString(L"Version", IntegerToWideString(CustomerID));
	wstring          text = IniFile.ReadString(L"Version", L"0");
	vector<wstring>  items;

	SplitString(text, L".", items);

  for (uint idx = 0; idx < 4; ++idx) {
		_ApplicationVersion[idx] = StringToInteger(items[idx]);
	}
	if (items.size() >= 5) {
		_Tag = items[4];
		_HaveTag = true;
  }

	vector<wstring> fwVerText;

	IniFile.ReadStrings(L"System", L"Firmware Version", fwVerText);

	vector<wstring>::const_iterator it = fwVerText.begin();
	vector<wstring>::const_iterator end = fwVerText.end();

	int pos = 0;


	while (it != end) {
		pos = (*it).find_first_of(L":");

		if (pos != wstring::npos) {
			wstring left = (*it).substr(0, pos);
			wstring right = (*it).substr(pos+1, (*it).length()-pos);

			_FWVerDict.insert(make_pair(left, right));
		}
		else {
			_FWVerDict.insert(make_pair((*it), L"-"));
		}
		++it;
	}


	/*
	text = IniFile.ReadString(L"System", L"Firmware Version");
	SplitString(text, L".", items);

	_ReaderFirmwareVersion[0] = StringToInteger(items[0]);

	uint value = StringToInteger(items[1]);

	_ReaderFirmwareVersion[1] = value / 10;
	_ReaderFirmwareVersion[2] = value % 10;*/

	_DriverVersion = IniFile.ReadString(L"System", L"Driver Version");

  _Minor = IniFile.ReadString(L"Version", L"0.Minor", L"");

  if (_Minor != L"")
    _HaveMinor = true;
  else
    _HaveMinor = false;
}
//---------------------------------------------------------------------------
void CTestStationVersion::SetSelectedReaderFirmwareVersion(const wstring& Version)
{
	vector<wstring>  items;

	SplitString(Version, L".", items);

	_ReaderFirmwareVersion[0] = StringToInteger(items[0]);

	uint value = StringToInteger(items[1]);

	_ReaderFirmwareVersion[1] = value / 10;
	_ReaderFirmwareVersion[2] = value % 10;
}
//---------------------------------------------------------------------------
wstring CTestStationVersion::GetApplicatonVersionCaption() const
{
  ////uint testVersion = _ApplicationVersion[3];

  return IntegerToWideString(_ApplicationVersion[0]) + L'.' +
         IntegerToWideString(_ApplicationVersion[1]) + L'.' +
         IntegerToWideString(_ApplicationVersion[2]) + L'.' +
				 IntegerToWideString(_ApplicationVersion[3]) +
				 (_HaveTag ? L"." + _Tag : (wstring)L"") +
         (wstring)(_HaveMinor ? ((wstring)L" (" + _Minor + L")") : (wstring)L"");
}
//---------------------------------------------------------------------------
wstring CTestStationVersion::GetApplicatonVersionString() const
{
  ////uint testVersion = _ApplicationVersion[3];

  return IntegerToWideString(_ApplicationVersion[0]) + L'.' +
         IntegerToWideString(_ApplicationVersion[1]) + L'.' +
         IntegerToWideString(_ApplicationVersion[2]) + L'.' +
				 IntegerToWideString(_ApplicationVersion[3]);
}
//---------------------------------------------------------------------------
wstring CTestStationVersion::GetFirmwareVersionString() const
{
	wstring verList;

	map<wstring, wstring>::const_iterator it = _FWVerDict.begin();
	map<wstring, wstring>::const_iterator first = _FWVerDict.begin();
	map<wstring, wstring>::const_iterator end = _FWVerDict.end();

	while (it != end) {
		if (it != first) {
			verList += L", ";
		}

		verList += it->first;

		if (it->second == L"-") {
			;
		}
		else if (it->second == L"") {
			verList += L" and above";
		}
		else {
			verList += L" to " + it->second;
		}
		++it;
	}
	return verList;
}
//---------------------------------------------------------------------------
bool CTestStationVersion::CheckReaderFWVersionSupport(const wstring& Ver)
{
	wstring verList;

	map<wstring, wstring>::const_iterator it = _FWVerDict.begin();
	map<wstring, wstring>::const_iterator end = _FWVerDict.end();

	while (it != end) {

		if (it->second == L"-") {
			if (Ver == it->first) {
				return true;
			}
		}
		else if (it->second == L"") {
			if (Ver >= it->first) {
				return true;
			}
		}
		else {
			if ((Ver >= it->first) && (Ver <= it->second)) {
				return true;
			}
		}
		++it;
	}
	return false;
}
//---------------------------------------------------------------------------