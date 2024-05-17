# Project - Patcher

* 目的：以事先撰寫的描述檔(JSON格式)進行程式碼自動化修改、更新
* Source Code：https://github.com/zackjtl/CodePatcher.git
* 範本：已放置在release目錄中，其設定檔及更新檔以可套用至目前大部分MP Tool版本
* 組成：
    * NewVer資料夾：放置要替代、更新的程式新版內容
    * config.json：用來描述要執行的更新、插入、替代等動作；程式的行為主要由這個檔案來描述
    * Patcher.exe：主程式　
    * 其他：其餘為相依的library檔案
    <img src="https://hackmd.io/_uploads/S1gTSUVXR.png" width="75%">

## 操作

```bash
> patcher.exe -f [script file] -v [svn number]
 -f: 後面指定json檔案，預設為config.json
 -v: 後面輸入數字，為SVN版本；該數字會用在每組更新行為的條件判斷
```

## JSON描述檔

以json格式中括號[]內的陣列來描述要進行的程式碼更新行為，每個大括號{}內為一組行為描述，行為中若有"condition"，則會以condition中的條件式是否為true，true的話才會執行這組更新

有5種行為模式，說明如下：

### Mode 1: 以更新檔的內容取代目標檔案內指定段落
```json
  {
    "mode": 1,
    "path": "SourceCode\\MPTool",
    "condition": "svn <= 2056",
    "targetFile": "MainForm.cpp",
    "targetFunction": "bool TMainFrm::ScanReader",
    "startString": "if (readerVersion[0] !=       TestStationVersion.GetFirmwareVersionString()) {",
    "endString": "_ReaderFirmwareVersion = readerVersion[0].c_str();",
    "newContentFile": "NewVer\\MainForm_ScanReaderV1.cpp"
  },
```
* path: 搜尋target file的起始目錄，若沒設定，則以patcher執行檔所在目錄為起始目錄
* condition: 支援Ｃ語言不等式，以user輸入-v後面的數字為svn，符合條件才執行這項修改(可省略)
* targetFile: 要修改的目標檔按 (只需輸入檔名，程式會自動由上述的path開始搜尋，包含子目錄)
* targetFunction: 要修改的code的所在function (這是為了縮小搜尋範圍，可以省略，但要確保搜尋字串在該程式檔案內只出現一次)
* startString: 段落起始字串
* endString: 段落結束字串
* newContentFile: 要替換的新內容的檔案

註：以整行為單位，即使輸入的段落起始及結束字串只是該行的一部分，也會被整行取代掉

### Mode 2: 將更新檔中的內容插入目標檔案內指定的字串的前面或後面
```json
  {
    "mode": 2,
    "path": "SourceCode\\MPTool",
    "targetFile": "CardOpenThread.cpp",
    "targetFunction": "void CCardOpen::Run",
    "targetString": "_CardSerialNum = SerialNum.Get();",
    "newString": "    Save_40PortData_InSerialNum(_CardSerialNum);",
    "insertAfter": true
  },
```
* path: 搜尋target file的起始目錄，若沒設定，則以patcher執行檔所在目錄為起始目錄
* condition: 支援Ｃ語言不等式，以user輸入-v後面的數字為svn，符合條件才執行這項修改(可省略)
* targetFile: 要修改的目標檔按 (只需輸入檔名，程式會自動由上述的path開始搜尋，包含子目錄)
* targetFunction: 要修改的code的所在function (這是為了縮小搜尋範圍，可以省略，但要確保搜尋字串在該程式檔案內只出現一次)
* targetString: 目標字串 (該字串本身不會被取代)
* newString: 要插入在目標字串前面或後面的新字串
* insertAfter: true - newString插入在targetString後面；false - newString插入在targetString後面

### Mode 3: 取代指定行的字串內容
```json
  {
    "mode": 3,
    "path": "SourceCode\\MPTool",
    "targetFile": "MainForm.cpp",
    "targetFunction": "void TMainFrm::SaveSerialNumAndMKBLicenseCount",
    "targetString": "CIniFile  iniFile(_SerialNumberFilePath, FILE_CREATE_NEW);",
    "newString": "	CIniFile  iniFile(_SerialNumberFilePath, FILE_OPEN_READ_WRITE);"
  },
```
* path: 搜尋target file的起始目錄，若沒設定，則以patcher執行檔所在目錄為起始目錄
* condition: 支援Ｃ語言不等式，以user輸入-v後面的數字為svn，符合條件才執行這項修改(可省略)
* targetFile: 要修改的目標檔按 (只需輸入檔名，程式會自動由上述的path開始搜尋，包含子目錄)
* targetFunction: 要修改的code的所在function (這是為了縮小搜尋範圍，可以省略，但要確保搜尋字串在該程式檔案內只出現一次)
* targetString: 目標字串 (該字串本身不會被取代)
* newString: 要插取代目標字串的新字串

註：以整行為單位，即使輸入的目標字串只是該行的一部分，也會被整行取代掉

### Mode 4: 將更新檔內容插入在目標檔案的結尾
```json
{
    "mode": 4,
    "path": "SourceCode\\MPTool",
    "targetFile": "MainForm.cpp",
    "newContentFile": "NewVer\\MainForm_SNFunc.cpp"
},
```
* path: 搜尋target file的起始目錄，若沒設定，則以patcher執行檔所在目錄為起始目錄
* condition: 支援Ｃ語言不等式，以user輸入-v後面的數字為svn，符合條件才執行這項修改(可省略)
* targetFile: 要修改的目標檔按 (只需輸入檔名，程式會自動由上述的path開始搜尋，包含子目錄)
* newContentFile: 新內容的檔案

### Mode 5: 將目標檔案以新檔案整個取代
```json
  {
    "mode": 5,
    "path": "SourceCode\\MPTool",
    "targetFile": "SerialNum.cpp",
    "newContentFile": "NewVer\\SerialNum.cpp"
  },
  {
    "mode": 5,
    "path": "SourceCode\\MPTool",
    "targetFile": "SerialNum.h",
    "newContentFile": "NewVer\\SerialNum.h"
  }
```
* path: 搜尋target file的起始目錄，若沒設定，則以patcher執行檔所在目錄為起始目錄
* condition: 支援Ｃ語言不等式，以user輸入-v後面的數字為svn，符合條件才執行這項修改(可省略)
* targetFile: 要被取代的檔案 (只需輸入檔名，程式會自動由上述的path開始搜尋，包含子目錄)
* newContentFile: 新內容的檔案
