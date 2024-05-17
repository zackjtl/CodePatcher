    CBW.SetCbd(0x02, QuickInitByCmd62);      // force cmd62
#if defined(_ET1288)
		CBW.SetCbd(0x03, 0x88);       // no quick init
#elif defined (_ET1289)
		CBW.SetCbd(0x03, 0x89);       // no quick init
#endif

    _Ut33xDevice.CmdLaunch(CBW, NULL, 0, 10, true); //