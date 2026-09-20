#include "recomp.h"

#ifdef __cplusplus
extern "C" {
#endif

void __ll_div_recomp(uint8_t* rdram, recomp_context* ctx);
void osViSetMode_recomp(uint8_t* rdram, recomp_context* ctx);
void osSpTaskYield_recomp(uint8_t* rdram, recomp_context* ctx);
void osViSetEvent_recomp(uint8_t* rdram, recomp_context* ctx);
void osCartRomInit_recomp(uint8_t* rdram, recomp_context* ctx);
void osSetThreadPri_recomp(uint8_t* rdram, recomp_context* ctx);
void osSetIntMask_recomp(uint8_t* rdram, recomp_context* ctx);
void osWritebackDCache_recomp(uint8_t* rdram, recomp_context* ctx);
void __osInitialize_common_recomp(uint8_t* rdram, recomp_context* ctx);
void osMotorStop_recomp(uint8_t* rdram, recomp_context* ctx);
void osWritebackDCacheAll_recomp(uint8_t* rdram, recomp_context* ctx);
void osPfsDeleteFile_recomp(uint8_t* rdram, recomp_context* ctx);
void __ull_div_recomp(uint8_t* rdram, recomp_context* ctx);
void osAiSetNextBuffer_recomp(uint8_t* rdram, recomp_context* ctx);
void osPfsInitPak_recomp(uint8_t* rdram, recomp_context* ctx);
void osSpTaskLoad_recomp(uint8_t* rdram, recomp_context* ctx);
void osViSetSpecialFeatures_recomp(uint8_t* rdram, recomp_context* ctx);
void osInvalDCache_recomp(uint8_t* rdram, recomp_context* ctx);
void osJamMesg_recomp(uint8_t* rdram, recomp_context* ctx);
void osContGetReadData_recomp(uint8_t* rdram, recomp_context* ctx);
void osViBlack_recomp(uint8_t* rdram, recomp_context* ctx);
void osInvalICache_recomp(uint8_t* rdram, recomp_context* ctx);
void osStartThread_recomp(uint8_t* rdram, recomp_context* ctx);
void osPfsReadWriteFile_recomp(uint8_t* rdram, recomp_context* ctx);
void osViGetCurrentFramebuffer_recomp(uint8_t* rdram, recomp_context* ctx);
void osCreateMesgQueue_recomp(uint8_t* rdram, recomp_context* ctx);
void osSendMesg_recomp(uint8_t* rdram, recomp_context* ctx);
void osPiStartDma_recomp(uint8_t* rdram, recomp_context* ctx);
void osContStartReadData_recomp(uint8_t* rdram, recomp_context* ctx);
void osContGetQuery_recomp(uint8_t* rdram, recomp_context* ctx);
void osSetTimer_recomp(uint8_t* rdram, recomp_context* ctx);
void osPfsAllocateFile_recomp(uint8_t* rdram, recomp_context* ctx);
void osPfsFileState_recomp(uint8_t* rdram, recomp_context* ctx);
void osSpTaskYielded_recomp(uint8_t* rdram, recomp_context* ctx);
void osAiGetLength_recomp(uint8_t* rdram, recomp_context* ctx);
void osAiSetFrequency_recomp(uint8_t* rdram, recomp_context* ctx);
void osSetEventMesg_recomp(uint8_t* rdram, recomp_context* ctx);
void osGetCount_recomp(uint8_t* rdram, recomp_context* ctx);
void __ll_mul_recomp(uint8_t* rdram, recomp_context* ctx);
void osContStartQuery_recomp(uint8_t* rdram, recomp_context* ctx);
void osStopThread_recomp(uint8_t* rdram, recomp_context* ctx);
void osContInit_recomp(uint8_t* rdram, recomp_context* ctx);
void osContSetCh_recomp(uint8_t* rdram, recomp_context* ctx);
void osPfsFreeBlocks_recomp(uint8_t* rdram, recomp_context* ctx);
void __ull_rem_recomp(uint8_t* rdram, recomp_context* ctx);
void osViSwapBuffer_recomp(uint8_t* rdram, recomp_context* ctx);
void osCreateViManager_recomp(uint8_t* rdram, recomp_context* ctx);
void __osRestoreInt_recomp(uint8_t* rdram, recomp_context* ctx);
void osVirtualToPhysical_recomp(uint8_t* rdram, recomp_context* ctx);
void osCreateThread_recomp(uint8_t* rdram, recomp_context* ctx);
void __osDisableInt_recomp(uint8_t* rdram, recomp_context* ctx);
void __osSetFpcCsr_recomp(uint8_t* rdram, recomp_context* ctx);
void __osSpSetPc_recomp(uint8_t* rdram, recomp_context* ctx);
void osRecvMesg_recomp(uint8_t* rdram, recomp_context* ctx);
void osGetThreadPri_recomp(uint8_t* rdram, recomp_context* ctx);
void osSpTaskStartGo_recomp(uint8_t* rdram, recomp_context* ctx);
void osPfsFindFile_recomp(uint8_t* rdram, recomp_context* ctx);
void osMotorInit_recomp(uint8_t* rdram, recomp_context* ctx);
void osMotorStart_recomp(uint8_t* rdram, recomp_context* ctx);
void osCreatePiManager_recomp(uint8_t* rdram, recomp_context* ctx);
void osGetTime_recomp(uint8_t* rdram, recomp_context* ctx);
void hm_func_80410C7C(uint8_t* rdram, recomp_context* ctx);
void hm_func_80413F7C(uint8_t* rdram, recomp_context* ctx);
void hm_func_80413F88(uint8_t* rdram, recomp_context* ctx);
void hm_func_80413F94(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041401C(uint8_t* rdram, recomp_context* ctx);
void hm_func_80414110(uint8_t* rdram, recomp_context* ctx);
void hm_func_804141F4(uint8_t* rdram, recomp_context* ctx);
void hm_func_80414248(uint8_t* rdram, recomp_context* ctx);
void hm_func_804142D8(uint8_t* rdram, recomp_context* ctx);
void hm_func_80414420(uint8_t* rdram, recomp_context* ctx);
void hm_func_804146D0(uint8_t* rdram, recomp_context* ctx);
void hm_func_80414764(uint8_t* rdram, recomp_context* ctx);
void hm_func_80414794(uint8_t* rdram, recomp_context* ctx);
void hm_func_804147C4(uint8_t* rdram, recomp_context* ctx);
void hm_func_804147F4(uint8_t* rdram, recomp_context* ctx);
void hm_func_80414824(uint8_t* rdram, recomp_context* ctx);
void hm_func_80414854(uint8_t* rdram, recomp_context* ctx);
void hm_func_80414884(uint8_t* rdram, recomp_context* ctx);
void hm_func_804148B4(uint8_t* rdram, recomp_context* ctx);
void hm_func_80414940(uint8_t* rdram, recomp_context* ctx);
void hm_func_80415028(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041505C(uint8_t* rdram, recomp_context* ctx);
void hm_func_804151EC(uint8_t* rdram, recomp_context* ctx);
void hm_func_804152D4(uint8_t* rdram, recomp_context* ctx);
void hm_func_80415710(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041A190(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041A204(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041A5C0(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041AC98(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041B3D4(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041B938(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041C0A8(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041C0F8(uint8_t* rdram, recomp_context* ctx);
void hm_func_8041C340(uint8_t* rdram, recomp_context* ctx);

#ifdef __cplusplus
}
#endif
