#ifndef TestStationVersionH
#define TestStationVersionH

////#include <vcl.h>
#include <string>
#include <vector>
#include "BaseTypes.h"
#include "IniFile.h"

using namespace std;
//---------------------------------------------------------------------------
class CTestStationVersion
{
public :
					CTestStationVersion()
						: _HaveTag(false)
					{
					}
  void    Create(CIniFile& IniFile, uint CustomerID);
	void    GetApplicatonVersion(word* OutBuf, uint Length) { memcpy(OutBuf, _ApplicationVersion, Length * 2); }
	void    SetSelectedReaderFirmwareVersion(const wstring& Version);
  void    GetReaderFirmwareVersion(byte* OutBuf, uint Length) { memcpy(OutBuf, _ReaderFirmwareVersion, Length); }
  word    GetApplicatonVersion(uint Index) { return _ApplicationVersion[Index]; }
	wstring GetApplicatonVersionCaption() const;
  wstring GetApplicatonVersionString() const;
  wstring GetFirmwareVersionString() const;
	wstring GetDriverVersion() const { return _DriverVersion; }

  bool    HaveMinorVersion() const { return _HaveMinor; }
  wstring GetMinorVersion() const { return _Minor; }

	bool 		CheckReaderFWVersionSupport(const wstring& Ver);

private :
  word     _ApplicationVersion[4];
  byte     _ReaderFirmwareVersion[3];
	wstring  _DriverVersion;
	wstring  _Tag;
	bool		 _HaveTag;
  wstring  _Minor;
  bool     _HaveMinor;

	vector<wstring>	 _SupportReaderFWVer;
	map<wstring, wstring> _FWVerDict;
};
//---------------------------------------------------------------------------

#endif
