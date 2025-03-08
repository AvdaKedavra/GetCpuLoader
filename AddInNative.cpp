
#include "CPULoadMonitor.h"
#include <chrono>
#include <thread>

#include "stdafx.h"

#if defined( __linux__ ) || defined(__APPLE__)
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <iconv.h>
#endif

#include <wchar.h>
#include "AddInNative.h"
#include <string>

//My include
#include <unordered_set>
#include <locale>
#include <codecvt>
#include <thread>

static const wchar_t* g_MethodNames[] = {
    L"TestString",
    L"TestInt",
    L"TestBool",
    L"TestDate",
    L"GetCPULoad"
};

static const wchar_t* g_MethodNamesRu[] = {
    L"ТестСтрока",
    L"ТестЧисло",
    L"ТестБулево",
    L"ТестДата",
    L"ПолучитьПроцентЗагрузкиПроцессора"
};

static const WCHAR_T g_kClassNames[] = u"CAddInNative"; //|OtherClass1|OtherClass2";

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);
static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;
static std::u16string s_names(g_kClassNames);
//---------------------------------------------------------------------------//
long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        *pInterface= new CAddInNative();
        return (long)*pInterface;
    }
    return 0;
}
//---------------------------------------------------------------------------//
AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
    g_capabilities = capabilities;
    return eAppCapabilitiesLast;
}
//---------------------------------------------------------------------------//
AttachType GetAttachType()
{
    return eCanAttachAny;
}
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pIntf)
{
    if(!*pIntf)
        return -1;

    delete *pIntf;
    *pIntf = 0;
    return 0;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
    return s_names.c_str();
}
//---------------------------------------------------------------------------//
//CAddInNative
CAddInNative::CAddInNative()
{
}
//---------------------------------------------------------------------------//
CAddInNative::~CAddInNative()
{
}
//---------------------------------------------------------------------------//
bool CAddInNative::Init(void* pConnection)
{ 
    m_iConnect = (IAddInDefBase*)pConnection;
    return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetInfo()
{ 
    return 2000; 
}
//---------------------------------------------------------------------------//
void CAddInNative::Done()
{
}
/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase
//---------------------------------------------------------------------------//
bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{ 
    const wchar_t* wsExtension = L"FindsWordMutch";
    size_t iActualSize = ::wcslen(wsExtension) + 1;

    if (m_iMemory)
    {
        if (m_iMemory->AllocMemory((void**)wsExtensionName, (unsigned)iActualSize * sizeof(WCHAR_T))) {
            ::convToShortWchar(wsExtensionName, wsExtension, iActualSize); 
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNProps()
{ 
    return eLastProp;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{ 
    return -1;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias)
{ 
    return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::SetPropVal(const long lPropNum, tVariant *varPropVal)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropReadable(const long lPropNum)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropWritable(const long lPropNum)
{
    return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNMethods()
{ 
    return eLastMethod;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindMethod(const WCHAR_T* wsMethodName)
{ 
    long plMethodNum = -1;
    wchar_t* name = 0;

    ::convFromShortWchar(&name, wsMethodName);

    plMethodNum = findName(g_MethodNames, name, eLastMethod);

    if (plMethodNum == -1)
        plMethodNum = findName(g_MethodNamesRu, name, eLastMethod);

    delete[] name;

    return plMethodNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, const long lMethodAlias)
{ 
    if (lMethodNum >= eLastMethod)
        return NULL;

    wchar_t* wsCurrentName = NULL;
    WCHAR_T* wsMethodName = NULL;
    size_t iActualSize = 0;

    switch (lMethodAlias)
    {
    case 0: // First language
        wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
        break;
    case 1: // Second language
        wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
        break;
    default:
        return 0;
    }

    iActualSize = wcslen(wsCurrentName) + 1;

    if (m_iMemory && wsCurrentName)
    {
        if (m_iMemory->AllocMemory((void**)&wsMethodName, (unsigned)iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
    }

    return wsMethodName;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNParams(const long lMethodNum)
{ 
    switch (lMethodNum)
    {
    case eFindWordMutch:
        return 2;
    case eTestString:
        return 1;
    case eTestInt:
        return 1;
    case eTestBool:
        return 1;
    case eTestDate:
        return 1;
    default:
        return 0;
    }
    return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                        tVariant *pvarParamDefValue)
{ 
    return false;
} 
//---------------------------------------------------------------------------//
bool CAddInNative::HasRetVal(const long lMethodNum)
{ 
    switch (lMethodNum)
    {
    case eFindWordMutch:
        return true;
    case eTestString:
        return true;
    case eTestInt:
        return true;
    case eTestBool:
        return true;
    case eTestDate:
        return true;
    case eGetCPULoad:
        return true;
    default:
        return false;
    }

    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
    switch (lMethodNum)
    {
    case eTestString: {

        wchar_t* wsCurrentName = NULL;
        size_t iActualSize = 0;

        ::convFromShortWchar(&wsCurrentName, TV_WSTR(&paParams[0]));

        iActualSize = ::wcslen(wsCurrentName) + 1;
        TV_VT(pvarRetValue) = VTYPE_PWSTR;

        if (m_iMemory->AllocMemory((void**)&pvarRetValue->pwstrVal, iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(&pvarRetValue->pwstrVal, wsCurrentName, iActualSize);
        pvarRetValue->wstrLen = iActualSize;
        return true;
        break;
    }
    case eTestInt: {

        int ui = TV_INT_PTR(&paParams[0]);
        TV_VT(pvarRetValue) = VTYPE_I4;
        pvarRetValue->lVal = ui;

        return true;
        break;
    }
    case eTestBool: {

        bool ui = TV_BOOL(&paParams[0]);
        TV_VT(pvarRetValue) = VTYPE_BOOL;
        pvarRetValue->bVal = ui;
        return true;
        break;
    }
    case eTestDate: {

        DATE ui = TV_DATE(&paParams[0]);
        TV_VT(pvarRetValue) = VTYPE_DATE;
        pvarRetValue->date = ui;

        return true;
        break;
    }
    case eGetCPULoad: {
        
        CPULoadMonitor cpuMonitor;
        try {

            float cpuLoad = cpuMonitor.GetCPULoad();

            TV_VT(pvarRetValue) = VTYPE_BOOL;
            pvarRetValue->fltVal = cpuLoad * 100;
            return true;
            break;
        }
        catch (const std::exception& e) {
            
            // дописать вывод ошибок
            return false;
        }
    }
                 
    default:
        return false;
    }
}
//---------------------------------------------------------------------------//
void CAddInNative::SetLocale(const WCHAR_T* loc)
{
}
//---------------------------------------------------------------------------//
void ADDIN_API CAddInNative::SetUserInterfaceLanguageCode(const WCHAR_T * lang)
{
}
//---------------------------------------------------------------------------//
bool CAddInNative::setMemManager(void* mem)
{
    m_iMemory = (IMemoryManager*)mem;
    return m_iMemory != 0;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, size_t len)
{
    if (!len)
        len = ::wcslen(Source) + 1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*) Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len * sizeof(WCHAR_T));

#if defined( __linux__ ) || defined(__APPLE__)
    size_t succeed = (size_t)-1;
    size_t f = len * sizeof(wchar_t), t = len * sizeof(WCHAR_T);
    const char* fromCode = sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32";
    iconv_t cd = iconv_open("UTF-16LE", fromCode);
    if (cd != (iconv_t)-1)
    {
        succeed = iconv(cd, (char**)&tmpWChar, &f, (char**)&tmpShort, &t);
        iconv_close(cd);
        if(succeed != (size_t)-1)
            return (uint32_t)succeed;
    }
#endif 
    for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
    {
        *tmpShort = (WCHAR_T)*tmpWChar;
    }

    return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source) + 1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len * sizeof(wchar_t));
#if defined( __linux__ ) || defined(__APPLE__)
    size_t succeed = (size_t)-1;
    const char* fromCode = sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32";
    size_t f = len * sizeof(WCHAR_T), t = len * sizeof(wchar_t);
    iconv_t cd = iconv_open("UTF-32LE", fromCode);
    if (cd != (iconv_t)-1)
    {
        succeed = iconv(cd, (char**)&tmpShort, &f, (char**)&tmpWChar, &t);
        iconv_close(cd);
        if(succeed != (size_t)-1)
            return (uint32_t)succeed;
    }
#endif 
    for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
    {
        *tmpWChar = (wchar_t)*tmpShort;
    }

    return res;
}
//---------------------------------------------------------------------------//
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T *tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}
//---------------------------------------------------------------------------//
long CAddInNative::findName(const wchar_t* names[], const wchar_t* name,
    const uint32_t size) const
{
    long ret = -1;
    for (uint32_t i = 0; i < size; i++)
    {
        if (!wcscmp(names[i], name))
        {
            ret = i;
            break;
        }
    }
    return ret;
}


CPULoadMonitor::CPULoadMonitor() : previousTotalTicks(0), previousIdleTicks(0) {}

float CPULoadMonitor::GetCPULoad() {
    FILETIME idleTime, kernelTime, userTime;

    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        throw std::runtime_error("Failed to get system times.");
    }

    const auto idleTicks = FileTimeToUInt64(idleTime);
    const auto totalTicks = FileTimeToUInt64(kernelTime) + FileTimeToUInt64(userTime);

    return CalculateCPULoad(idleTicks, totalTicks);
}

unsigned long long CPULoadMonitor::FileTimeToUInt64(const FILETIME& ft) {
    return (static_cast<unsigned long long>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

float CPULoadMonitor::CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks) {
    const auto totalTicksSinceLastTime = totalTicks - previousTotalTicks;
    const auto idleTicksSinceLastTime = idleTicks - previousIdleTicks;

    const float cpuLoad = 1.0f - ((totalTicksSinceLastTime > 0)
        ? static_cast<float>(idleTicksSinceLastTime) / totalTicksSinceLastTime
        : 0.0f);

    previousTotalTicks = totalTicks;
    previousIdleTicks = idleTicks;

    return cpuLoad;
}


