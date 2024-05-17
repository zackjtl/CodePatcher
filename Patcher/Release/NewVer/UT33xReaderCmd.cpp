	CBW.SetOpCode(READER_ClASS_OP_CODE, 0x00); // Op / sub-op
#if defined(_ET1288)
	CBW.SetCbd(0x02, 0x88);       // no quick init
#elif defined (_ET1289)
	CBW.SetCbd(0x02, 0x89);       // no quick init
#endif
  CBW.SetCbd( 0x08 , WorkMode);  // cdb[4] = 32; allocaton length