#include <iostream>

#include <Windows.h>

#include <MinHook.h>

#include "TranslateService.hpp"
//#include "Logger.hpp"

kiddiontr::TranslateService* tr_service = nullptr;

typedef int(WINAPI* DRAWTEXTW)(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format);

DRAWTEXTW fpDrawTextW = nullptr;

int WINAPI MyDrawTextW(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format)
{
    if (cchText == 1)
        return fpDrawTextW(hdc, lpchText, cchText, lprc, format);
    if(lpchText[0] == L'◀' && lpchText[cchText - 1] == L'▶')
        return fpDrawTextW(hdc, lpchText, cchText, lprc, format);
    std::wstring wstr(lpchText);
    auto index = wstr.find(L'|');
    
    std::wstring tr;
    if (index != std::wstring_view::npos)
        tr = tr_service->tr(wstr.substr(0, index));
    else
        tr = tr_service->tr(wstr);

    //auto tr = tr_service->tr(wstr);50250222222222222222222228585
    return fpDrawTextW(hdc, tr.data(), tr.size(), lprc, format);
}

bool hook() {
    if (MH_Initialize() != MH_OK)
        return false;

    if (MH_CreateHook(&DrawTextW, &MyDrawTextW, (LPVOID*)&fpDrawTextW) != MH_OK)
        return false;

    if (MH_EnableHook(&DrawTextW) != MH_OK)
        return false;
    return true;
}

bool unhook() {
    if (MH_DisableHook(&DrawTextW) != MH_OK)
        return false;
    if (MH_Uninitialize() != MH_OK)
        return false;
    return true;
}

void init()
{
#ifdef _DEBUG
    AllocConsole();
#endif // _DEBUG
    if (!hook())
    {
        MessageBox(nullptr, TEXT("Failed to hook DrawTextW"), TEXT("KiddionTR"), MB_OK);
        //(FATAL) << "Translate Service Init";
        return;
    }


    auto filepath = std::filesystem::current_path();
    filepath += "\\tr.txt";
    tr_service = new kiddiontr::TranslateService(filepath.wstring().c_str());
    //LOG(INFO) << "Translate Service Init";

    //LOG(INFO) << "Kiddion loaded.";
    return;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        new std::thread(&init);
        break;
    case DLL_PROCESS_DETACH:
        if (!unhook())
            MessageBox(nullptr, TEXT("Failed to unhook DrawTextW"), TEXT("KiddionTR"), MB_OK);

        if (tr_service)
            delete tr_service;

        //LOG(INFO) << "Kiddion unloaded.";
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

