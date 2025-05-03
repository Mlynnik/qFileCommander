#include "shellfuncs.h"
#include <vector>
#include <Windows.h>
#include <ShlObj.h>
#include <shellapi.h>


class Array_Releaser {
private:
    const std::vector<ITEMIDLIST*>& v;
public:
    Array_Releaser(const std::vector<ITEMIDLIST*>& _v) : v(_v) {}
    ~Array_Releaser() {
        for (ITEMIDLIST* i : v)
            CoTaskMemFree(i);
    }
};

bool create_menu(QStringList& files, void* parentWind, HMENU& hmenu, IContextMenu*& imenu)
{
    if (files.empty())
        return false;

    std::vector<ITEMIDLIST*> ids;
    std::vector<LPCITEMIDLIST> rIds;
    IShellFolder* ifolder = nullptr;
    HRESULT res;
    for (int i = 0; i < files.size(); ++i)
    {
        files[i].replace('/', '\\');
        ids.push_back(0);
        res = SHParseDisplayName(files[i].toStdWString().c_str(), 0, &ids.back(), 0, 0);
        if (!SUCCEEDED(res) || !ids.back())
        {
            ids.pop_back();
            continue;
        }

        rIds.push_back(0);
        res = SHBindToParent(ids.back(), IID_IShellFolder, (void**)&ifolder, &rIds.back());
        if (!SUCCEEDED(res) || !rIds.back())
            rIds.pop_back();
        else if (i < files.size() - 1 && ifolder)
        {
            ifolder->Release();
            ifolder = nullptr;
        }
    }

    Array_Releaser arrReleaser(ids);

    if (!ifolder)
        return false;

    imenu = 0;


    res = ifolder->GetUIObjectOf((HWND)parentWind, (UINT)rIds.size(), (const ITEMIDLIST**)rIds.data(), IID_IContextMenu, 0, (void**)&imenu);
    if (!SUCCEEDED(res) || !imenu)
        return false;

    hmenu = CreatePopupMenu();


    if (!hmenu)
        return false;
    return (SUCCEEDED(imenu->QueryContextMenu(hmenu, 0, 1, 0x7FFF, CMF_NORMAL)));
}


bool shellfuncs::get_context_menu(QStringList& files, int x, int y, void* parentWind)
{
    IContextMenu* imenu = 0;
    HMENU hMenu = NULL;
    if (!create_menu(files, parentWind, hMenu, imenu) || !hMenu || !imenu)
        return false;

    int iCmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD, x, y, (HWND)parentWind, NULL);
    if (iCmd > 0)
    {
        CMINVOKECOMMANDINFO info = { 0 };
        info.cbSize = sizeof(info);
        info.hwnd = (HWND)parentWind;
        info.lpVerb = MAKEINTRESOURCEA(iCmd - 1);
        info.nShow = SW_SHOWNORMAL;
        imenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);
    }

    if (imenu) imenu->Release();
    DestroyMenu(hMenu);

    return true;
}

bool shellfuncs::copy_api(QStringList& files, void * parentWind)
{
    IContextMenu * imenu = 0;
    HMENU hMenu = NULL;
    if (!create_menu(files, parentWind, hMenu, imenu) || !hMenu || !imenu)
        return false;

    CMINVOKECOMMANDINFO info = { 0 };
    info.cbSize = sizeof(info);
    info.hwnd = (HWND)parentWind;
    info.lpVerb = "Copy";
    info.nShow = SW_SHOWNORMAL;
    HRESULT res = imenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);

    if (imenu) imenu->Release();
    DestroyMenu(hMenu);

    return SUCCEEDED(res);
}

bool shellfuncs::cut_api(QStringList& files, void * parentWind)
{
    IContextMenu * imenu = 0;
    HMENU hMenu = NULL;
    if (!create_menu(files, parentWind, hMenu, imenu) || !hMenu || !imenu)
        return false;

    CMINVOKECOMMANDINFO info = { 0 };
    info.cbSize = sizeof(info);
    info.hwnd = (HWND) parentWind;
    info.lpVerb = "Cut";
    info.nShow = SW_SHOWNORMAL;
    HRESULT res = imenu->InvokeCommand((LPCMINVOKECOMMANDINFO) &info);

    if (imenu) imenu->Release();
    DestroyMenu(hMenu);

    return SUCCEEDED(res);
}

bool shellfuncs::paste_api(QString& dir_to, void * parentWind)
{
    IContextMenu * imenu = 0;
    HMENU hMenu = NULL;
    QStringList t(1, dir_to);
    if (!create_menu(t, parentWind, hMenu, imenu) || !hMenu || !imenu)
        return false;

    CMINVOKECOMMANDINFO info = { 0 };
    info.cbSize = sizeof(info);
    info.hwnd = (HWND) parentWind;
    info.lpVerb = "Paste";
    info.nShow = SW_SHOWNORMAL;
    HRESULT res = imenu->InvokeCommand((LPCMINVOKECOMMANDINFO) &info);

    if (imenu) imenu->Release();
    DestroyMenu(hMenu);

    return SUCCEEDED(res);
}


bool shellfuncs::delete_files_api(QString fname, bool moveToTrash, void * parentWind)
{
    __unaligned ITEMIDLIST* idl = ILCreateFromPathW(fname.replace('/', '\\').toStdWString().c_str());
    if (!idl)
    {
        ILFree(idl);
        return false;
    }
    IShellItem *psi;
    HRESULT res = SHCreateShellItem(NULL, NULL, idl, &psi);

    if (!SUCCEEDED(res) || !psi)
    {
        return false;
    }

    IFileOperation * iOperation = nullptr;
    res = CoCreateInstance(CLSID_FileOperation, 0, CLSCTX_ALL, IID_IFileOperation, (void**)&iOperation);
    if (!SUCCEEDED(res) || !iOperation)
    {
        return false;
    }

    res = iOperation->DeleteItem(psi, NULL);
    if (SUCCEEDED(res))
    {
        if (moveToTrash)
            iOperation->SetOperationFlags(FOF_ALLOWUNDO);
        else
            iOperation->SetOperationFlags(FOF_WANTNUKEWARNING);

        iOperation->SetOwnerWindow((HWND) parentWind);

        res = iOperation->PerformOperations();
        if (SUCCEEDED(res) || res == COPYENGINE_E_USER_CANCELLED)
            res = S_OK;
    }

    iOperation->Release();
    psi->Release();
    return SUCCEEDED(res);
}


void shellfuncs::showProperties(QStringList selected_files)
{
    for (int i = 0; i < selected_files.length(); ++i) {
        selected_files[i].replace("/", "\\");
    }

    if (selected_files.length() == 1) {
        SHELLEXECUTEINFO info = {0};
        info.cbSize = sizeof info;
        info.lpFile = (const wchar_t*) selected_files.first().utf16();
        info.nShow = SW_SHOW;
        info.fMask = SEE_MASK_INVOKEIDLIST;
        info.lpVerb = L"properties";
        ShellExecuteEx(&info);
    } else {
        int nrFiles = selected_files.length();
        LPITEMIDLIST *pidlDrives = (LPITEMIDLIST *)malloc(sizeof(LPITEMIDLIST)*nrFiles);
        IShellFolder* psfDesktop;
        IDataObject* pdata;
        HRESULT hr;
        ULONG chEaten=0, dwAttributes=0;
        int i=0;
        hr = SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, pidlDrives);
        if (SUCCEEDED(hr))
        {
            hr = SHGetDesktopFolder(&psfDesktop);
            for (int i = 0; i < nrFiles; i ++)
                psfDesktop->ParseDisplayName(NULL, NULL, (wchar_t*)selected_files[i].utf16(), &chEaten, (LPITEMIDLIST*)&pidlDrives[i], &dwAttributes);
            if (SUCCEEDED(hr))
            {
                hr = psfDesktop->GetUIObjectOf(NULL, nrFiles, (LPCITEMIDLIST*)pidlDrives, IID_IDataObject, NULL, (void**)&pdata);
                if (SUCCEEDED(hr))
                {
                    CoInitialize(NULL);
                    SHMultiFileProperties(pdata,0);
                    pdata->Release();
                    CoUninitialize();
                }
                psfDesktop->Release();
            }
            for(i=0; i < nrFiles; i++)
                ILFree(pidlDrives[i]);
        }
        free(pidlDrives);
    }
}
