#include "Pch.h"
#pragma hdrstop

#include "SerialNum.h"
#include "IniFile.h"
#include "UserOpError.h"
//---------------------------------------------------------------------------
CSerialNum::CSerialNum(const CSerialNum& Source)
{
  *this = Source;
}
//---------------------------------------------------------------------------
CSerialNum& CSerialNum::operator=(const CSerialNum& Source)
{
  Lock  lock(*this);

	_Type = Source.GetType();
	_EnableSystemDate = Source.GetSystemDateEnabled();
  _Start = Source.GetStart();
  _End = Source.GetEnd();
  Source.GetRecycling(_Recycling);

  return *this;
}
//---------------------------------------------------------------------------
bool CSerialNum::operator!=(const CSerialNum& Source)
{
  Lock  lock(*this);

  if (_Type != Source._Type) {
    return true;
	}
	if (_EnableSystemDate != Source._EnableSystemDate) {
    return true;
  }
	if (_Start != Source._Start) {
		return true;
	}
	if (_End != Source._End) {
		return true;
	}

	if (_Recycling != Source._Recycling) {
		return true;
	}
  return false;
}
//---------------------------------------------------------------------------
void CSerialNum::SaveToIniFile(CIniFile& IniFile)
{
	Lock           lock(*this);
	vector<dword>  items(_Recycling.begin(), _Recycling.end());

	if (_EnableSystemDate) {
		ValidateSystemDate();
	}

	IniFile.WriteIntegers(L"Serial Num", L"Recycling", items);
	IniFile.WriteInteger(L"Serial Num", L"Type", _Type);
	IniFile.WriteInteger(L"Serial Num", L"Enable System Date", _EnableSystemDate);
	IniFile.WriteInteger(L"Serial Num", L"End", _End);
	IniFile.WriteInteger(L"Serial Num", L"Start", (uint)_Start);
}
//---------------------------------------------------------------------------
void CSerialNum::SavePortDetailInfo(CIniFile& IniFile, TCardStatus& CardStatus, uint& ReaderSerialNum)
{
	Lock lock(*this);

	wstring port = L"Port" + IntegerToWideString(ReaderSerialNum);
	IniFile.WriteInteger(port, L"SerialNum", CardStatus.SerialNum[ReaderSerialNum]);
	IniFile.WriteInteger(port, L"PortUsed", CardStatus.PortUsed[ReaderSerialNum]);
	IniFile.UpdateFile();
}
//---------------------------------------------------------------------------
void CSerialNum::LoadFromIniFile(CIniFile& IniFile)
{
  Lock           lock(*this);
  vector<dword>  items;

  IniFile.ReadIntegers(L"Serial Num", L"Recycling", items);
  _Recycling.assign(items.begin(), items.end());

	_Type = IniFile.ReadInteger(L"Serial Num", L"Type");
	_EnableSystemDate = IniFile.ReadInteger(L"Serial Num", L"Enable System Date");
	_End = IniFile.ReadInteger(L"Serial Num", L"End");
	_Start = (uint) IniFile.ReadInteger(L"Serial Num", L"Start");

  if (_EnableSystemDate) {
  	ValidateSystemDate();
	}

	_IniFile = IniFile;
}
//---------------------------------------------------------------------------
void CSerialNum::ValidateSystemDate()
{
	_Start = UpdateSerialNumberDatePrefix(_Start, true);
	_End = UpdateSerialNumberDatePrefix(_End, false);
}
//---------------------------------------------------------------------------
uint CSerialNum::UpdateSerialNumberDatePrefix(uint Original, bool ResetIfDateChanged)
{
	wstring str = IntegerToHexWideString(Original, 8);
	wstring prefix = str.substr(0, 2);
  wstring fixed = str.substr(2, 2);
	wstring body = str.substr(4, 4);

	int dateNumber = HexStringToInteger(prefix);
	int dateNow = GetDateNow();

	if (dateNumber != dateNow) {
		prefix = IntegerToHexWideString(dateNow, 2);
		if (ResetIfDateChanged) {
			body = L"0000";
		}
		return HexStringToInteger(prefix + fixed + body);
	}
	return Original;
}
//---------------------------------------------------------------------------
int CSerialNum::GetDateNow()
{
  SYSTEMTIME      systemTime;
  wostringstream  text;
  GetLocalTime(&systemTime);

  return systemTime.wDay;
}
//---------------------------------------------------------------------------
void CSerialNum::Recycle(dword SerialNum)
{
  Lock  lock(*this);

  if (_Type == ORDER_SERIAL_NUM) {
    _Recycling.push_back(SerialNum);
  }
}
//---------------------------------------------------------------------------
dword CSerialNum::Get() throw (CError)
{
	Lock   lock(*this);
	dword  value;
	bool   fromRecycle = false;

	if (_Type == ORDER_SERIAL_NUM) {
		if (_EnableSystemDate) {
			ValidateSystemDate();
		}

		while (!_Recycling.empty()) {
			value = _Recycling.front();
			_Recycling.pop_front();

			if (_EnableSystemDate) {
				wstring strVal = IntegerToHexWideString(value, 8);
				wstring prefix = strVal.substr(0, 2);
				int dateNumber = HexStringToInteger(prefix);
				int dateNow = GetDateNow();

				if (dateNow == dateNumber) {
					fromRecycle = true;
					break;
				}
			}
			else {
				fromRecycle = true;
      	break;
			}
		}

		if (!fromRecycle) {
			if (_Start <= _End) {
						value = _Start++;
			}
			else {
				 throw CError(ERROR_END_SERIAL_NUM,
											L"[Error] : Out of end serial number.");
			}
		}
	}
	else {
		value = _Start;
	}
  return value;
}
//---------------------------------------------------------------------------


