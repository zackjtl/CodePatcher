//---------------------------------------------------------------------------
#ifndef SerialNumH
#define SerialNumH
//-----------------------------------------------------------------------------
#include "BaseTypes.h"
#include "BaseError.h"
#include "Lock.h"
#include "IniFile.h"
#include <list>

#define MAX_PORT  40

using namespace std;
//-----------------------------------------------------------------------------
enum {
	FIXED_SERIAL_NUM = 0, ORDER_SERIAL_NUM
};

typedef struct {
	dword  SerialNum[MAX_PORT];
	bool   PortUsed[MAX_PORT];         // Not Used = 0             ; Used = 1
	bool   OpenCardFinish[MAX_PORT];   // Not Open Card Finish = 0 ; Open Card Finish = 1
	bool   PassOrFail[MAX_PORT];       // Fail = 0                 ; Pass = 1
} TCardStatus;


class CSerialNum : public ObjectLevelLockable<CSerialNum>
{
public :
       CSerialNum() {}
       CSerialNum(dword Start, dword End) : _Start(Start), _End(End)
       {
       }
       CSerialNum(const CSerialNum& Source);
  CSerialNum& operator=(const CSerialNum& Source);
  bool operator!=(const CSerialNum& Source);
	void SaveToIniFile(CIniFile& IniFile);
	void SavePortDetailInfo(CIniFile& IniFile, TCardStatus& CardStatus, uint& ReaderSerialNum);
  void LoadFromIniFile(CIniFile& IniFile);

	void SetType(uint Value)
	{
    Lock  lock(*this);
    _Type = Value;
  }
	void SetStart(dword Value)
  {
    Lock  lock(*this);
    _Start = Value;
  }
  void SetEnd(dword Value)
  {
    Lock  lock(*this);
    _End = Value;
  }
  void SetRecycling(const list<dword>& Value)
  {
    Lock  lock(*this);
    _Recycling.assign(Value.begin(), Value.end());
	}
	void SetEnableSystemDate(bool Value)
	{
		_EnableSystemDate = Value;
  }
  void ClearRecycling()
  {
    Lock  lock(*this);
    _Recycling.clear();
  }
  void Recycle(dword SerialNum);

  uint GetType() const
  {
    Lock  lock(*this);
    return _Type;
  }
  dword GetStart() const
  {
    Lock  lock(*this);
    return  _Start;
  }
  dword GetEnd()const
  {
    Lock  lock(*this);
    return _End;
	}
	bool GetSystemDateEnabled() const
	{
		return _EnableSystemDate;
  }
  void GetRecycling(list<dword>& OutBuf) const { OutBuf = _Recycling; }
  dword Get() throw (CError);

	static int GetDateNow();
	void ValidateSystemDate();

private:
	uint UpdateSerialNumberDatePrefix(uint Original, bool ResetIfDateChanged);

private :
  uint        _Type;
	uint64      _Start;
  dword       _End;
	byte				_Date;
	bool				_EnableSystemDate;
	list<dword> _Recycling;
	CIniFile    _IniFile;
};
//-----------------------------------------------------------------------------
#endif


