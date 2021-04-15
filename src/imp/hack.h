//int  IMP_Encoder_GetStream (int encChn, void *stream, int blockFlag);
int IMP_OSD_SetPoolSize(int newPoolSize);
int IMP_Encoder_SetPoolSize(int newPoolSize0);
int IMP_Encoder_SetMbRC ( int  encChn, int  bEnable);
int  IMP_Encoder_GetSuperFrameCfg (int encChn, void *pstSuperFrmParam);
int  IMP_Encoder_SetSuperFrameCfg (int encChn, const void *pstSuperFrmParam);

/*
int  IMP_Encoder_CreateGroup (int encGroup);

int  IMP_Encoder_DestroyGroup (int encGroup);

int  IMP_Encoder_CreateChn (int encChn, const IMPEncoderCHNAttr *attr);

int  IMP_Encoder_DestroyChn (int encChn);

int  IMP_Encoder_GetChnAttr (int encChn, IMPEncoderCHNAttr *const attr);

int  IMP_Encoder_RegisterChn (int encGroup, int encChn);

int  IMP_Encoder_UnRegisterChn (int encChn);

int  IMP_Encoder_StartRecvPic (int encChn);

int  IMP_Encoder_StopRecvPic (int encChn);

int  IMP_Encoder_Query (int encChn, IMPEncoderCHNStat *stat);


int  IMP_Encoder_ReleaseStream (int encChn, IMPEncoderStream *stream);

int  IMP_Encoder_PollingStream (int encChn, uint32_t timeoutMsec);

int  IMP_Encoder_SetMaxStreamCnt (int encChn, int nrMaxStream);

int  IMP_Encoder_GetMaxStreamCnt (int encChn, int *nrMaxStream);

int  IMP_Encoder_RequestIDR (int encChn);

int  IMP_Encoder_FlushStream (int encChn);

int  IMP_Encoder_SetChnColor2Grey (int encChn, const IMPEncoderColor2GreyCfg *pstColor2Grey);

int  IMP_Encoder_GetChnColor2Grey (int encChn, IMPEncoderColor2GreyCfg *pstColor2Grey);

int  IMP_Encoder_SetChnAttrRcMode (int encChn, const IMPEncoderAttrRcMode *pstRcModeCfg);

int  IMP_Encoder_GetChnAttrRcMode (int encChn, IMPEncoderAttrRcMode *pstRcModeCfg);

int  IMP_Encoder_SetChnFrmRate (int encChn, const IMPEncoderFrmRate *pstFps);

int  IMP_Encoder_GetChnFrmRate (int encChn, IMPEncoderFrmRate *pstFps);

int  IMP_Encoder_SetChnROI (int encChn, const IMPEncoderROICfg *pstVencRoiCfg);

int  IMP_Encoder_GetChnROI (int encChn, IMPEncoderROICfg *pstVencRoiCfg);

int  IMP_Encoder_GetGOPSize (int encChn, IMPEncoderGOPSizeCfg *pstGOPSizeCfg);

int  IMP_Encoder_SetGOPSize (int encChn, const IMPEncoderGOPSizeCfg *pstGOPSizeCfg);

int  IMP_Encoder_SetChnDemask (int encChn, const IMPEncoderAttrDemask *pdemaskAttr);

int  IMP_Encoder_GetChnDemask (int encChn, IMPEncoderAttrDemask *pdemaskAttr);

int  IMP_Encoder_SetChnFrmUsedMode (int encChn, const IMPEncoderAttrFrmUsed *pfrmUsedAttr);

int  IMP_Encoder_GetChnFrmUsedMode (int encChn, IMPEncoderAttrFrmUsed *pfrmUsedAttr);

int  IMP_Encoder_SetChnDenoise (int encChn, const IMPEncoderAttrDenoise *pdenoiseAttr);

int  IMP_Encoder_GetChnDenoise (int encChn, IMPEncoderAttrDenoise *pdenoiseAttr);

int  IMP_Encoder_SetChnHSkip (int encChn, const IMPEncoderAttrHSkip *phSkipAttr);

int  IMP_Encoder_GetChnHSkip (int encChn, IMPEncoderAttrHSkip *phSkipAttr);

int  IMP_Encoder_SetChnHSkipBlackEnhance (int encChn, const int bBlackEnhance);

int  IMP_Encoder_InsertUserData (int encChn, void *userData, uint32_t userDataLen);

int  IMP_Encoder_SetFisheyeEnableStatus (int encChn, int enable);

int  IMP_Encoder_GetFisheyeEnableStatus (int encChn, int *enable);

int  IMP_Encoder_SetChangeRef (int encChn, int bEnable);

int  IMP_Encoder_GetChangeRef (int encChn, int *bEnable);

int  IMP_Encoder_SetMbRC (int encChn, int bEnable);

int  IMP_Encoder_GetMbRC (int encChn, int *bEnable);



int  IMP_Encoder_SetH264TransCfg (int encChn, const IMPEncoderH264TransCfg *pstH264TransCfg);

int  IMP_Encoder_GetH264TransCfg (int encChn, IMPEncoderH264TransCfg *pstH264TransCfg);

int  IMP_Encoder_SetJpegeQl (int encChn, const IMPEncoderJpegeQl *pstJpegeQl);

int  IMP_Encoder_GetJpegeQl (int encChn, IMPEncoderJpegeQl *pstJpegeQl);
*/
