			UnicodeString  text;
			if (!TestStationVersion.CheckReaderFWVersionSupport(readerVersion[0])) {
				text.sprintf(L"Current reader firmware verison is %s, not %s\r\n"
										 L"Please update reader firmware.",
										 readerVersion[0].c_str(),
										 TestStationVersion.GetFirmwareVersionString().c_str());
				AddLog(clRed, text);
				EnableUuserInterfaceAfterScanReaderPass(false);
				return false;
			}
			_ReaderFirmwareVersion = readerVersion[0].c_str();

			TestStationVersion.SetSelectedReaderFirmwareVersion(_ReaderFirmwareVersion);
