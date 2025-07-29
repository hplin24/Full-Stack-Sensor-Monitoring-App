#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>
#include "WMI.h"
#include "Logger.h"

#pragma comment(lib, "wbemuuid.lib")

bool QueryWMI(const std::wstring& wsQuery, const std::wstring& wsProperty, std::wstring& result)
{
    Logger logger(L"wmi");
    logger.StartLogging();

    logger.LogWstr(L"[QueryWMI] CALLED");
    logger.LogWstr(L"[QueryWMI] Given query=" + wsQuery);

    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        logger.LogStr("[QueryWMI] Failed to initialize COM library. Error code = " + std::to_string(hres));
        return false;
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------

    hres = CoInitializeSecurity(
        NULL,
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
    );

    if (FAILED(hres))
    {
        logger.LogStr("[QueryWMI] Failed to initialize security. Error code = " + std::to_string(hres));
        CoUninitialize();
        return false;
    }

    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator* pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator, (LPVOID*)&pLoc);

    if (FAILED(hres))
    {
        logger.LogStr("[QueryWMI] Failed to create IWbemLocator object. Error code = " + std::to_string(hres));
        CoUninitialize();
        return false;
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices* pSvc = NULL;

    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
        NULL,                    // User name. NULL = current user
        NULL,                    // User password. NULL = current
        0,                       // Locale. NULL indicates current
        NULL,                    // Security flags.
        0,                       // Authority (for example, Kerberos)
        0,                       // Context object 
        &pSvc                    // pointer to IWbemServices proxy
    );

    if (FAILED(hres))
    {
        logger.LogStr("[QueryWMI] Could not connect. Error code = " + std::to_string(hres));
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    logger.LogStr("[QueryWMI] Connected to ROOT\\CIMV2 WMI namespace");

    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
        pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
        NULL,                        // Server principal name 
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        logger.LogStr("[QueryWMI] Could not set proxy blanket. Error code = " + std::to_string(hres));
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t(L"WQL"),
        bstr_t(std::wstring(L"SELECT * FROM " + wsQuery).c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        logger.LogStr("[QueryWMI] Query for operating system name failed. Error code = " + std::to_string(hres));
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return false;
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------

    IWbemClassObject* pclsObj = NULL;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
            &pclsObj, &uReturn);

        if (0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        VariantInit(&vtProp);
        // Get the value of the Name property
        hr = pclsObj->Get(wsProperty.c_str(), 0, &vtProp, 0, 0);
        logger.LogWstr(wsProperty + L" : " + vtProp.bstrVal);
        result = vtProp.bstrVal;
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    // Cleanup
    // ========

    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();

    logger.LogStr("[QueryWMI] Run successfully");

    return true;
}


//#include <iostream>
//int main()
//{
//    std::wstring res;
//    std::cout << QueryWMI(L"Win32_SerialPort WHERE Caption LIKE '%STLink Virtual COM Port%'", L"DeviceID", res) << std::endl;
//    std::wcout << L"res: " << res << L"___" << std::endl;
//    system("pause");
//    return 0;
//}