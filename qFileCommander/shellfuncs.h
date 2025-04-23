#ifndef SHELLFUNCS_H
#define SHELLFUNCS_H

#include <vector>
#include <Windows.h>
#include <ShlObj.h>
#include <cstdlib>
#include <algorithm>
#include <shellapi.h>
#include <QList>


class CComInterfaceReleaser {
public:
    explicit CComInterfaceReleaser(IUnknown* i) : _i(i) {}
    ~CComInterfaceReleaser() { if (_i) _i->Release(); }
private:
    IUnknown* _i;
};

class CItemIdArrayReleaser {
public:
    explicit CItemIdArrayReleaser(const std::vector<ITEMIDLIST*>& idArray) : _array(idArray) {}
    ~CItemIdArrayReleaser() {
        for (ITEMIDLIST* item : _array)
            CoTaskMemFree(item);
    }

    CItemIdArrayReleaser& operator=(const CItemIdArrayReleaser&) = delete;
private:
    const std::vector<ITEMIDLIST*>& _array;
};

struct ComInitializer {
    ComInitializer() {
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    }

    ~ComInitializer() {
        CoUninitialize();
    }
};

static bool prepareContextMenuForObjects(QStringList& objects, void* parentWindow, HMENU& hmenu, IContextMenu*& imenu)
{
    ComInitializer comInitializer;

    if (objects.empty())
        return false;

    std::vector<ITEMIDLIST*> ids;
    std::vector<LPCITEMIDLIST> relativeIds;
    IShellFolder* ifolder = 0;
    for (size_t i = 0; i < objects.size(); ++i)
    {
        std::replace(objects[i].begin(), objects[i].end(), '/', '\\');
        ids.push_back(0);
        HRESULT result = SHParseDisplayName(objects[i].toStdWString().c_str(), 0, &ids.back(), 0, 0);
        if (!SUCCEEDED(result) || !ids.back())
        {
            ids.pop_back();
            continue;
        }

        relativeIds.push_back(0);
        result = SHBindToParent(ids.back(), IID_IShellFolder, (void**)&ifolder, &relativeIds.back());
        if (!SUCCEEDED(result) || !relativeIds.back())
            relativeIds.pop_back();
        else if (i < objects.size() - 1 && ifolder)
        {
            ifolder->Release();
            ifolder = nullptr;
        }
    }

    CItemIdArrayReleaser arrayReleaser(ids);

    imenu = 0;

    if (!ifolder)
        return false;

    HRESULT result = ifolder->GetUIObjectOf((HWND)parentWindow, (UINT)relativeIds.size(), (const ITEMIDLIST**)relativeIds.data(), IID_IContextMenu, 0, (void**)&imenu);
    if (!SUCCEEDED(result) || !imenu)
        return false;

    hmenu = CreatePopupMenu();
    if (!hmenu)
        return false;
    return (SUCCEEDED(imenu->QueryContextMenu(hmenu, 0, 1, 0x7FFF, CMF_NORMAL)));
}


static bool openShellContextMenuForObjects(QStringList& objects, int xPos, int yPos, void* parentWindow)
{
    ComInitializer comInitializer;

    IContextMenu* imenu = 0;
    HMENU hMenu = NULL;
    if (!prepareContextMenuForObjects(objects, parentWindow, hMenu, imenu) || !hMenu || !imenu)
        return false;

    CComInterfaceReleaser menuReleaser(imenu);

    const int iCmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD, xPos, yPos, (HWND)parentWindow, NULL);
    if (iCmd > 0)
    {
        CMINVOKECOMMANDINFO info = { 0 };
        info.cbSize = sizeof(info);
        info.hwnd = (HWND)parentWindow;
        info.lpVerb = MAKEINTRESOURCEA(iCmd - 1);
        info.nShow = SW_SHOWNORMAL;
        imenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);
    }

    DestroyMenu(hMenu);

    return true;
}


static bool pasteFromClipboard(QString destFolder, void * parentWindow)
{
    ComInitializer comInitializer;

    IContextMenu * imenu = 0;
    HMENU hMenu = NULL;
    QStringList t(1, destFolder);
    if (!prepareContextMenuForObjects(t, parentWindow, hMenu, imenu) || !hMenu || !imenu)
        return false;

    CComInterfaceReleaser menuReleaser(imenu);

    CMINVOKECOMMANDINFO info = { 0 };
    info.cbSize = sizeof(info);
    info.hwnd = (HWND) parentWindow;
    info.lpVerb = "Paste";
    info.nShow = SW_SHOWNORMAL;
    const auto result = imenu->InvokeCommand((LPCMINVOKECOMMANDINFO) &info);

    DestroyMenu(hMenu);

    return SUCCEEDED(result);
}


static bool deleteItems(QStringList& items, bool moveToTrash, void * parentWindow)
{
    ComInitializer comInitializer;

    std::vector<ITEMIDLIST*> idLists;
    for (auto& path: items)
    {
        __unaligned ITEMIDLIST* idl = ILCreateFromPathW(path.toStdWString().c_str());
        if (!idl)
        {
            for (auto& pid : idLists)
                ILFree(pid);
            return false;
        }
        idLists.push_back(idl);
    }

    IShellItemArray * iArray = 0;
    HRESULT result = SHCreateShellItemArrayFromIDLists((UINT)idLists.size(), (LPCITEMIDLIST*)idLists.data(), &iArray);

    // Freeing memory
    for (auto& pid: idLists)
        ILFree(pid);
    idLists.clear();

    if (!SUCCEEDED(result) || !iArray)
    {
        return false;
    }

    IFileOperation * iOperation = 0;
    result = CoCreateInstance(CLSID_FileOperation, 0, CLSCTX_ALL, IID_IFileOperation, (void**)&iOperation);
    if (!SUCCEEDED(result) || !iOperation)
    {
        return false;
    }

    result = iOperation->DeleteItems(iArray);
    if (SUCCEEDED(result))
    {
        if (moveToTrash)
        {
            iOperation->SetOperationFlags(FOF_ALLOWUNDO);
        }
        else
            iOperation->SetOperationFlags(FOF_WANTNUKEWARNING);

        iOperation->SetOwnerWindow((HWND) parentWindow);

        result = iOperation->PerformOperations();
        if (SUCCEEDED(result) || result == COPYENGINE_E_USER_CANCELLED)
        {
            result = S_OK;
        }
    }

    iOperation->Release();
    iArray->Release();
    return SUCCEEDED(result);
}


static void showProperties(QStringList selected_files)
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
                    //hr=SHMultiFileProperties(pdata,0);
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

#endif // SHELLFUNCS_H
