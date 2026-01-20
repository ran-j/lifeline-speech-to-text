#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include "lhsp01.h"

static void PrintError(const char *label, int err)
{
    const char *errStr = _lhs_ErrorGetStringA(err);
    std::cerr << label << " (err " << err << ": " << (errStr ? errStr : "unknown") << ")." << std::endl;
}

static bool InitializeLibrary(void **context)
{
    std::cout << "Initializing library..." << std::endl;
    _lhs_Initialize(0, context);
    if (*context == nullptr)
    {
        std::cerr << "Failed to initialize library." << std::endl;
        return false;
    }
    std::cout << "Library initialized." << std::endl;
    return true;
}

static bool OpenContextFromGrammar(void *context, const char *grammarFile, void **ctxHandle)
{
    const char *grammarFiles[] = {grammarFile};
    int err = _lhs_asrCtxOpenFromGrammarFilesA(context, grammarFiles, nullptr, 1, nullptr, ctxHandle);
    if (err != 0)
    {
        PrintError("Error while opening the context", err);
        return false;
    }
    std::cout << "Context opened from grammar file." << std::endl;
    return true;
}

static bool GetContextEngineName(void *ctxHandle, char *engineName, size_t engineNameSize)
{
    size_t size = engineNameSize;
    int err = _lhs_asrCtxGetEngineNameA(ctxHandle, engineName, &size);
    if (err != 0)
    {
        PrintError("Error while getting the engine name", err);
        return false;
    }
    std::cout << "Context engine name: " << engineName << std::endl;
    return true;
}

static bool LoadEngineList(void *context, std::vector<uint8_t> *engineList, uint32_t *engineCount)
{
    uint32_t count = 0;
    int err = _lhs_GetEngineListA(context, 0, nullptr, &count);
    if (err != 0)
    {
        PrintError("Error while querying for the engine list", err);
        return false;
    }
    if (count == 0)
    {
        std::cerr << "No recognition engines installed." << std::endl;
        return false;
    }
    std::cout << "Engine count: " << count << std::endl;

    engineList->assign(count * 0x184, 0);
    err = _lhs_GetEngineListA(context, 0, engineList->data(), &count);
    if (err != 0)
    {
        PrintError("Error while querying for engine list data", err);
        return false;
    }
    std::cout << "Engine list retrieved." << std::endl;
    *engineCount = count;
    return true;
}

static void *FindEngineEntry(const std::vector<uint8_t> &engineList, uint32_t engineCount, const char *engineName)
{
    for (uint32_t i = 0; i < engineCount; ++i)
    {
        const char *entry = reinterpret_cast<const char *>(engineList.data() + (i * 0x184));
        if (std::strcmp(entry, engineName) == 0)
        {
            return const_cast<char *>(entry);
        }
    }
    return nullptr;
}

static bool QueryEngineInfo(void *context, void *engineEntry, uint32_t *engineInfo)
{
    int err = _lhs_asrSysGetEngineInfoA(context, engineEntry, engineInfo);
    if (err != 0)
    {
        PrintError("Error while querying for engine info", err);
        return false;
    }
    std::cout << "Engine info retrieved." << std::endl;
    return true;
}

static bool OpenRecognizer(void *context, const char *engineName, uint32_t engineInfo, void *engineEntry,
                           void **recHandle)
{
    int err = _lhs_asrRecOpenA(context, engineName, engineInfo, engineEntry, recHandle);
    if (err != 0)
    {
        PrintError("Error while opening the engine", err);
        return false;
    }
    std::cout << "Recognizer opened." << std::endl;
    return true;
}

static bool LoadContextIntoRecognizer(void *recHandle, void *ctxHandle)
{
    int err = _lhs_asrRecLoadContext(recHandle, ctxHandle, 1);
    if (err != 0)
    {
        PrintError("Error while loading the context", err);
        return false;
    }
    std::cout << "Context loaded into recognizer." << std::endl;
    return true;
}

static void ConfigureRecognizer(void *recHandle)
{
    uint32_t recParams[] = {7, 0, 6, 0, 0x10, 0, 10, 0};
    _lhs_asrRecSetParamList(recHandle, recParams, 4);
    std::cout << "Recognizer params set." << std::endl;
}

static void Cleanup(void *recHandle, void *ctxHandle, void *context)
{
    if (recHandle != nullptr)
    {
        _lhs_asrRecUnloadContext(recHandle);
        _lhs_asrRecClose(recHandle);
    }
    if (ctxHandle != nullptr)
    {
        _lhs_asrCtxClose(ctxHandle);
    }
    if (context != nullptr)
    {
        _lhs_UnInitialize(context);
    }
}

int main()
{
    const char *grammarFile = "llus.grm";

    void *context = nullptr;
    if (!InitializeLibrary(&context))
    {
        return 1;
    }

    void *ctxHandle = nullptr;
    if (!OpenContextFromGrammar(context, grammarFile, &ctxHandle))
    {
        Cleanup(nullptr, nullptr, context);
        return 1;
    }

    char engineName[128] = {};
    if (!GetContextEngineName(ctxHandle, engineName, sizeof(engineName)))
    {
        Cleanup(nullptr, ctxHandle, context);
        return 1;
    }

    std::vector<uint8_t> engineList;
    uint32_t engineCount = 0;
    if (!LoadEngineList(context, &engineList, &engineCount))
    {
        Cleanup(nullptr, ctxHandle, context);
        return 1;
    }

    void *engineEntry = FindEngineEntry(engineList, engineCount, engineName);
    if (engineEntry == nullptr)
    {
        std::cerr << "Recognizing engine not found." << std::endl;
        Cleanup(nullptr, ctxHandle, context);
        return 1;
    }
    std::cout << "Engine entry matched: " << reinterpret_cast<char *>(engineEntry) << std::endl;

    uint32_t engineInfo = 0;
    if (!QueryEngineInfo(context, engineEntry, &engineInfo))
    {
        Cleanup(nullptr, ctxHandle, context);
        return 1;
    }

    void *recHandle = nullptr;
    if (!OpenRecognizer(context, engineName, engineInfo, engineEntry, &recHandle))
    {
        Cleanup(nullptr, ctxHandle, context);
        return 1;
    }

    if (!LoadContextIntoRecognizer(recHandle, ctxHandle))
    {
        Cleanup(recHandle, ctxHandle, context);
        return 1;
    }

    ConfigureRecognizer(recHandle);
    std::cout << "Context loaded and recognizer opened successfully." << std::endl;

    Cleanup(recHandle, ctxHandle, context);
    return 0;
}
