#ifndef LHSP01_H
#define LHSP01_H

#include <windows.h>

extern "C"
{
#if defined(_MSC_VER)
#define LHS_CALL __stdcall
#else
#define LHS_CALL __attribute__((stdcall))
#endif

    extern void LHS_CALL _lhs_Initialize(uint32_t param_1, void **param_2);
    extern int LHS_CALL _lhs_UnInitialize(void *context);
    extern const char *LHS_CALL _lhs_ErrorGetStringA(int errorCode);

    extern int LHS_CALL _lhs_asrCtxOpenFromGrammarFilesA(void *context, const char **grammarFiles, void *param_3, uint32_t fileCount, void *param_5, void **outCtx);
    extern int LHS_CALL _lhs_asrCtxGetEngineNameA(void *ctx, char *buffer, size_t *bufferSize);
    extern int LHS_CALL _lhs_asrCtxClose(void *ctx);

    extern int LHS_CALL _lhs_GetEngineListA(void *context, uint32_t type, void *buffer, uint32_t *count);
    extern int LHS_CALL _lhs_asrSysGetEngineInfoA(void *context, void *engineEntry, uint32_t *outInfo);

    extern int LHS_CALL _lhs_asrRecOpenA(void *context, const char *engineName, uint32_t engineInfo, void *engineEntry, void **outRec);
    extern int LHS_CALL _lhs_asrRecLoadContext(void *rec, void *ctx, uint32_t options);
    extern int LHS_CALL _lhs_asrRecSetParamList(void *rec, uint32_t *paramList, uint32_t count);
    extern int LHS_CALL _lhs_asrRecUnloadContext(void *rec);
    extern int LHS_CALL _lhs_asrRecClose(void *rec);
}

#endif // LHSP01_H