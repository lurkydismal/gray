#pragma once
#include <fswatcher/filewatch.hpp>

#include "ThmProperties.h"

class CContentView:
    public IEditorWnd
{
    friend class CLevelPreferences;

private:

    struct DragDropData 
    {
        xr_string FileName;
    };

    struct IconData 
    {
        ref_texture Icon;
        bool UseButtonColor = false;
    };

    struct HintItem
    {
        xr_string Name;
        ImVec2 Pos;
        bool Active = false;
    };

    struct FileOptData
    {
        xr_path File;
        bool IsDir = false;
        shared_str ISESect;
    };

    struct RenameObjectData
    {
        xr_string RenameBuf;
        xr_path Path{};
        bool Active{ false };
        bool Focus{ false };
        bool SetText{ false };
    };

    enum class EViewMode
    {
        Tile,
        List
    };

public:
    CContentView();
    virtual void Draw() override;
    void DrawHeader();
    void FindFile();
    void DrawISEDir(size_t& HorBtnIter, const size_t IterCount);
    void DrawRootDir(size_t& HorBtnIter, const size_t& IterCount, xr_string& NextDir);
    void DrawOtherDir(size_t& HorBtnIter, const size_t IterCount, xr_string& NextDir);

    void ClearFileList();

    void RescanISEDirectory(const xr_string& path);
    void RescanDirectory();

    virtual void Init();
    virtual void Destroy();
    virtual void ResetBegin();
    virtual void ResetEnd();

    void RemoveCustomIcon(const xr_string&);
    void LoadCustomIcons();
private:
    bool DrawItem(const FileOptData& FilePath, size_t& HorBtnIter, const size_t IterCount);
    bool DrawItemByList(const FileOptData& FilePath, size_t& HorBtnIter, const size_t IterCount);
    bool DrawItemHelper(xr_path& FilePath, xr_string& FileName, const CContentView::FileOptData& InitFileName, CContentView::IconData* IconPtr);
    bool DrawItemByTile(const FileOptData& FilePath, size_t& HorBtnIter, const size_t IterCount);
    bool DrawFormContext();
    bool DrawContext(const xr_path& Path);
    bool Contains();
    IconData& GetTexture(const xr_string& IconPath);

    xr_map<xr_string, FileOptData> ScanConfigs(const xr_string& StartPath);
    void ScanConfigsRecursive(xr_map<xr_string, CContentView::FileOptData>& TempPath, const xr_string& ParseStr);
    void CheckFileNameRecursive(xr_path&FilePath, const xr_string&) const;

    bool CheckFile(const xr_path& File) const;

    void CopyAction(const xr_path&) const;
    void PasteAction(const xr_string&) /*const*/;
    void DeleteAction(const xr_path&) /*const*/;
    void CutAction(const xr_path&) const;
    void CreateAction() /*const*/;
    void RenameAction(const xr_path& FilePath,const xr_string NewName);

    void RenameActionActivate(const xr_path& Path);
    void RenameActionEnd();

    bool ShouldTheFileHaveTHM(const xr_path&) const;

    void AcceptDragDropAction(xr_path&);
    bool BeginDragDropAction(xr_path&, xr_string&, const CContentView::FileOptData&, CContentView::IconData*);

private:
    EViewMode ViewMode = EViewMode::Tile;

    HintItem CurrentItemHint;

    DragDropData Data;
    xr_vector<FileOptData> Files;
    filewatch::FileWatch<std::string>* WatcherPtr;

    ref_texture MenuIcon;

    RenameObjectData RenameObject;
    mutable xr_path CopyObjectPath;
    mutable bool IsCutting;

    xr_string CurrentDir;
    xr_string RootDir;
    xr_string LogsDir;
    ImVec2 BtnSize{};

    xr_hash_map<xr_string, IconData> Icons;

    bool IsDelWatcher = false;
    bool IsSpawnElement = false;
    bool IsFindResult = false;

    bool IsThmMode = false;

    bool IsWndDestroyed = false;
    xr_string ISEPath;
    string32 FindStr = {};

    volatile xr_atomic_bool NeedRescan = false;

    float TextHeight = 0.f;

    CUIThmProperties ThmPropWnd;
};

extern CContentView* GContentView;