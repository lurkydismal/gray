#include "stdafx.h"
#include "ContentView.h"

#include "../../utils/xrDXT/xrDXT.h"

CContentView* GContentView = nullptr;

CContentView::CContentView():
    WatcherPtr(nullptr)
{
    string_path Dir = {};
    FS.update_path(Dir, "$fs_root$", "");

    RootDir = xr_path(Dir).xstring();
    CurrentDir = RootDir;
    CopyObjectPath.clear();
    IsCutting = false;

    FS.update_path(Dir, "$logs$", "");
    LogsDir = Dir;
}

void CContentView::Draw()
{
    if (IsWndDestroyed)
        return;

    if (ImGui::Begin("Content Browser"))
    {
        DrawHeader();

        if ((NeedRescan || Files.empty()) && !IsFindResult && !IsSpawnElement)
        {
            RescanDirectory();
            NeedRescan = false;
        }

        if (ImGui::BeginChild("##contentbrowserscroll"))
        {
            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && ImGui::IsMouseReleased(1) && !ImGui::IsItemHovered())
            {
                if (!xr_path(CurrentDir).has_root_path() && !IsSpawnElement)
                ImGui::OpenPopup("##contentbrowsercontext");
            }
            else if (!RenameObject.Focus && ImGui::IsMouseClicked(0) /*ImGui::IsMouseReleased(0)*/)
            {
                RenameObject.Active = false;
            }

            DrawFormContext();

            const size_t IterCount = (ImGui::GetWindowSize().x / (BtnSize.x + 15)) - 1;
            size_t HorBtnIter = 0;
            xr_string NextDir = CurrentDir;

            if ((!RootDir.Contains(CurrentDir) && !IsSpawnElement) || IsFindResult)
            {
                DrawOtherDir(HorBtnIter, IterCount, NextDir);
            }
            else if (IsSpawnElement)
            {
                DrawISEDir(HorBtnIter, IterCount);
            }
            else
            {
                DrawRootDir(HorBtnIter, IterCount, NextDir);
            }

            CurrentDir = NextDir;
            xr_strlwr(CurrentDir);
        }

        if (CurrentItemHint.Active)
        {
            ImGui::SetCursorPos(CurrentItemHint.Pos);
            ImGui::Button(CurrentItemHint.Name.c_str());
            CurrentItemHint.Active = false;
        }

        ImGui::EndChild();
    }

    ImGui::End();

    ThmPropWnd.Draw();
}

void CContentView::DrawHeader()
{
    BtnSize = (ViewMode == EViewMode::Tile) ? ImVec2(64.f, 64.f) : ImVec2(32.f, 32.f);
    if (ImGui::Button("root"))
    {
        CurrentDir = RootDir;
        IsSpawnElement = false;
        IsFindResult = false;
        std::memset(FindStr, 0, sizeof(FindStr));
        ISEPath.clear();
    }

    TextHeight = ImGui::CalcTextSize("1").y;

    ImGui::SameLine();
    ImGui::Text("/");

    auto DrawByPathLambda = [&](const xr_string& ViewDir)
    {
        auto Pathes = ViewDir.Split('\\');

        for (const xr_string& Path : Pathes)
        {
            ImGui::SameLine();
            if (ImGui::Button(Platform::ANSI_TO_UTF8(Path).data()))
            {
                xr_string NewPath = "";
                for (const xr_string& LocPath : Pathes)
                {
                    NewPath += LocPath;

                    if (LocPath == Path)
                        break;

                    NewPath += "\\";
                }

                if (IsSpawnElement)
                {
                    ISEPath = NewPath;
                    RescanISEDirectory(ISEPath);
                }
                else
                {
                    CurrentDir = NewPath;
                    RescanDirectory();
                }
            }

            ImGui::SameLine();
            ImGui::Text("/");
        }
    };

    if (IsSpawnElement)
    {
        ImGui::SameLine();
        if (ImGui::Button("Spawn Element"))
        {
            ISEPath.clear();
            RescanISEDirectory(ISEPath);
        }
        ImGui::SameLine();
        ImGui::Text("/");

        if (!ISEPath.empty())
        {
            DrawByPathLambda(ISEPath);
        }
    }
    else if (CurrentDir != RootDir)
    {
        DrawByPathLambda(CurrentDir);
    }


    int FindStartPosX = (int)ImGui::GetWindowSize().x;

    float w = 0;

    if (FindStartPosX > 400)
    {
        ImGui::SameLine();
        int FindSizeX = FindStartPosX / 3.5f;
        FindStartPosX -= FindSizeX;

        ImGui::SetCursorPosX(FindStartPosX);
        
        w = FindSizeX - 35;
    }
    else
    {
        w = FindStartPosX - 45;
    }

    IconData* IconPtr = &GetTexture("search");
    ImVec2 IconSize{ 0,0 };

    //Varian 1
    /*if (IconPtr->Icon)
    {
        IconSize={ 16,16 };
        ImGui::Image(IconPtr->Icon->pSurface, IconSize);
        ImGui::SameLine();
    }*/

    ImGui::SetNextItemWidth(w - IconSize.x*1.5f);

    if (ImGui::InputTextWithHint("##Search", "Search", FindStr, sizeof(FindStr)))
    {
        FindFile();
    }

    //Varian 2
    if (IconPtr->Icon)
    {
        IconSize = { 12,12 };

        ImGui::SameLine();
        ImVec2 cursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursorPos.x - IconSize.x-10.f, cursorPos.y+(IconSize.y/4)));

        ImGui::Image(IconPtr->Icon->pSurface, IconSize);
    }

    ImGui::SameLine();

    if (ImGui::BeginPopupContextItem("MenuCBPpp"))
    {
        if (ImGui::Checkbox("Show THM", &IsThmMode) && !IsSpawnElement)
        {
            RescanDirectory();
        }
        ImGui::Separator();

        if (ImGui::BeginMenu("View mode"))
        {
            if (ImGui::MenuItem("Tile"))
            {
                ViewMode = EViewMode::Tile;
            }
            if (ImGui::MenuItem("List"))
            {
                ViewMode = EViewMode::List;
            }

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }

    if (ImGui::ImageButton("##MenuCB", MenuIcon->pSurface, { 15, 15 }))
    {
        ImGui::OpenPopup("MenuCBPpp");
    }

    ImGui::Separator();
}

void CContentView::FindFile()
{
    xr_string ParseStr = IsUTF8(FindStr) ? Platform::UTF8_to_CP1251(FindStr) : FindStr;
    size_t Len = ParseStr.length();
    if (Len > 2)
    {
        IsFindResult = true;
        if (CurrentDir == RootDir && !IsSpawnElement)
        {
            IsDelWatcher = true;
            xr_delete(WatcherPtr);

            ClearFileList();
            for (const auto& file : xr_dir_recursive_iter { CurrentDir.data() })
            {
                if (file.is_directory())
                    continue;

                const xr_string& FName = file.path().filename().string().data();
                if (FName.Contains(ParseStr) && CheckFile(FName))
                {
                    Files.push_back({ file, false });
                }
            }

            auto TempPath = ScanConfigs("");
            ScanConfigsRecursive(TempPath, ParseStr);
        }
        else if (IsSpawnElement)
        {
            ClearFileList();

            auto TempPath = ScanConfigs("");
            ScanConfigsRecursive(TempPath, ParseStr);
        }
        else
        {
            IsDelWatcher = true;
            xr_delete(WatcherPtr);

            ClearFileList();
            for (const xr_dir_entry& file : xr_dir_recursive_iter { CurrentDir.data() })
            {
                if (file.is_directory())
                    continue;

                const xr_string& FName = xr_path(file.path()).xfilename();
                if (FName.Contains(ParseStr) && CheckFile(FName))
                {
                    Files.push_back({ file, false });
                }
            }
        }
    }
    else if (Len == 0)
    {
        IsFindResult = false;

        if (IsSpawnElement)
        {
            RescanISEDirectory(ISEPath);
        }
        else
        {
            RescanDirectory();
        }
    }
}

void CContentView::ScanConfigsRecursive(xr_map<xr_string, CContentView::FileOptData>& TempPath, const xr_string& ParseStr)
{
    for (auto& [Name, DirOpt] : TempPath)
    {
        if (DirOpt.IsDir)
        {
            auto RecFiles = ScanConfigs(Name);
            ScanConfigsRecursive(RecFiles, ParseStr);
        }

        if (Name.Contains(ParseStr) && !DirOpt.IsDir)
        {
            Files.push_back(DirOpt);
        }
    }
}

void CContentView::DrawISEDir(size_t& HorBtnIter, const size_t IterCount)
{
    if (DrawItem({ "..", true }, HorBtnIter, IterCount))
    {
        if (ISEPath.empty())
        {
            IsSpawnElement = false;
            ISEPath = "";
            ClearFileList();
        }
        else
        {
            xr_string Validate = ISEPath;
            if (Validate.ends_with('\\'))
            {
                Validate = Validate.erase(Validate.length() - 1);
            }
            xr_path ISEFS = Validate;

            if (ISEFS.has_parent_path())
            {
                RescanISEDirectory(ISEFS.parent_path().string().data());
                ISEPath = ISEFS.parent_path().string().data();
            }
            else
            {
                ISEPath = "";
                RescanISEDirectory("");
            }
        }
    }

    for (const FileOptData& Data : Files)
    {
        if (DrawItem(Data, HorBtnIter, IterCount))
        {
            if (Data.IsDir)
            {
                const xr_string& CopyFileName = Data.File.xstring();
                RescanISEDirectory(CopyFileName);
            }

            break;
        }
    }
}

void CContentView::DrawRootDir(size_t& HorBtnIter, const size_t& IterCount, xr_string& NextDir)
{
    string_path FSEntry = {};
    auto PathClickLambda = [&FSEntry, &HorBtnIter, &IterCount, &NextDir, this]()
    {
        xr_string Validate = FSEntry;
        if (Validate.ends_with('\\'))
        {
            Validate = Validate.erase(Validate.length() - 1);
        }

        ImGui::BeginDisabled(!FS.TryLoad(FSEntry));

        if (DrawItem({ Validate.c_str(), true }, HorBtnIter, IterCount))
        {
            NextDir = FSEntry;
            if (NextDir.ends_with('\\'))
            {
                NextDir = NextDir.erase(NextDir.length() - 1);
            }
            ClearFileList();
        }

        ImGui::EndDisabled();
    };

    IsSpawnElement = false;

    FS.update_path(FSEntry, "$server_data_root$", "");
    PathClickLambda();

    FS.update_path(FSEntry, "$import$", "");
    PathClickLambda();

    FS.update_path(FSEntry, "$game_data$", "");
    PathClickLambda();

    if (DrawItem({ "Spawn Elements", true }, HorBtnIter, IterCount))
    {
        RescanISEDirectory("");
    }
}

void CContentView::RescanISEDirectory(const xr_string& StartPath)
{
    ClearFileList();

    if (!StartPath.empty() && StartPath != ISEPath)
    {
        if (!ISEPath.empty() && !ISEPath.ends_with('\\'))
            ISEPath += "\\";

        ISEPath += StartPath + '\\';
    }

    auto TempPath = ScanConfigs(StartPath);

    for (auto& [Name, DirOpt] : TempPath)
    {
        if (DirOpt.IsDir)
        {
            Files.push_back(DirOpt);
        }
    }

    for (auto& [Name, DirOpt] : TempPath)
    {
        if (!DirOpt.IsDir)
        {
            Files.push_back(DirOpt);
        }
    }

    if (ISEPath.empty())
    {
        Files.push_back({ xr_string(xr_string(ENVMOD_CHOOSE_NAME) + ".ise") , false, ENVMOD_CHOOSE_NAME });
        Files.push_back({ xr_string(xr_string(RPOINT_CHOOSE_NAME) + ".ise") , false, RPOINT_CHOOSE_NAME });
    }

    IsSpawnElement = true;
}

void CContentView::DrawOtherDir(size_t& HorBtnIter, const size_t IterCount, xr_string& NextDir)
{
    xr_path FilePath = CurrentDir;
    if (!IsFindResult && DrawItem({ "..", true }, HorBtnIter, IterCount))
    {
        NextDir = FilePath.parent_path().string().data();
        if (FilePath.parent_path().is_absolute() && !NextDir.Contains(RootDir) || NextDir.empty())
        {
            NextDir = RootDir;
        }
        ClearFileList();
    }

    for (FileOptData FilePath : Files)
    {
        if (FilePath.IsDir)
        {
            if (DrawItem(FilePath, HorBtnIter, IterCount))
            {
                NextDir = FilePath.File.xstring();
                if (NextDir.ends_with('\\'))
                {
                    NextDir = NextDir.erase(NextDir.length() - 1);
                }
                ClearFileList();
                break;
            }
        }
        else
        {
            if (DrawItem(FilePath, HorBtnIter, IterCount))
            {
                if (FilePath.File.extension() == ".thm")
                {
                    ThmPropWnd.Load(FilePath.File);
                    ThmPropWnd.Show();
                }
                else if (FilePath.File.extension() == ".tga")
                {
                    string_path fn = {};
                    FS.update_path(fn, _textures_, "");
                    xr_string OldPath = FilePath.File;

                    auto CharIndex = OldPath.find(fn);
                    if (CharIndex != xr_string::npos)
                    {
                        xr_string NewPath = OldPath.substr(OldPath.find(fn) + xr_strlen(fn));
                        NewPath = NewPath.substr(0, NewPath.find_last_of("."));
                        ExecCommand(COMMAND_IMAGE_EDITOR_SELECT, NewPath, false);
                    }
                    else
                    {
                        FS.update_path(fn, _import_, "");
                        CharIndex = OldPath.find(fn);

                        if (CharIndex != xr_string::npos)
                        {
                            xr_string NewPath = OldPath.substr(OldPath.find(fn) + xr_strlen(fn));
                            ExecCommand(COMMAND_IMAGE_EDITOR_SELECT, NewPath, true);
                        }
                    }
                }
            }
        }
    }
}

void CContentView::ClearFileList()
{
    Files.clear();
}

void CContentView::RescanDirectory()
{
    IsDelWatcher = true;
    xr_delete(WatcherPtr);

    ClearFileList();
    for (const auto& file : xr_dir_iter{ CurrentDir.data() })
    {
        if (std::filesystem::is_directory(file))
        {
            Files.push_back({ file, true });
        }
    }
    for (const xr_dir_entry& file : xr_dir_iter { CurrentDir.data() })
    {
        if (!file.is_directory() && CheckFile(file))
        {
            Files.push_back({ file, false });
        }
    }

    WatcherPtr = new filewatch::FileWatch<std::string>
    (
        CurrentDir.data(),
        [this](const std::string&, const filewatch::Event)
        {
            NeedRescan = true;
        }
    );
}

void CContentView::Destroy()
{
    MenuIcon.destroy();
    Icons.clear();

    IsWndDestroyed = true;
}

void CContentView::ResetBegin() {
}

void CContentView::ResetEnd() {
}

void CContentView::LoadCustomIcons()
{
    if (EPrefs->custom_icons.size() == 0)
        return;

    for (auto el : EPrefs->custom_icons)
    {
        if (Icons.contains(el.second.c_str()))
        {
            Icons[el.first.c_str()] = Icons[el.second.c_str()];
            Icons[el.first.c_str()].UseButtonColor = true;
        }
        else
        {
            string_path Path = {};
            sprintf(Path, "%s%s", "ed\\content_browser\\", el.second);
            Icons[el.first.c_str()] = { EDevice->Resources->_CreateTexture(Path),    true };
        }

    }
    
}

void CContentView::RemoveCustomIcon(const xr_string& icon)
{
    if (Icons.contains(icon))
        Icons.erase(icon);
}

void CContentView::Init()
{
    Icons["Folder"] = {EDevice->Resources->_CreateTexture("ed\\content_browser\\folder"),    true};
    Icons[".."]        = {EDevice->Resources->_CreateTexture("ed\\content_browser\\folder"),    true};
    Icons["thm"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\thm"),        true};
    Icons["logs"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\log"),        true};
    Icons["ogg"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\ogg"),        true};
    Icons["level"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\level"),    true};
    Icons["wav"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\wav"),        true};
    Icons["object"] = {EDevice->Resources->_CreateTexture("ed\\content_browser\\object"),    true};
    Icons["image"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\image"),    true};
    Icons["seq"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\seq"),        true};
    Icons["tga"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\tga"),        true};
    Icons["file"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\file"),        true};
    Icons["exe"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\exe"),        true};
    Icons["cmd"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\cmd"),        true};
    Icons["dll"]    = {EDevice->Resources->_CreateTexture("ed\\content_browser\\dll"),        true};
    Icons["backup"] = {EDevice->Resources->_CreateTexture("ed\\content_browser\\backup"),    true};
    Icons["env_mod"]= {EDevice->Resources->_CreateTexture("ed\\content_browser\\env_mod"),    true};

    Icons["search"]= {EDevice->Resources->_CreateTexture("ed\\content_browser\\search"),    false};

    MenuIcon = EDevice->Resources->_CreateTexture("ed\\bar\\menu");

    LoadCustomIcons();
}

bool CContentView::DrawItem(const FileOptData& FilePath, size_t& HorBtnIter, const size_t IterCount)
{
    bool IsClicked = false;

    switch (ViewMode)
    {
    case EViewMode::Tile: IsClicked = DrawItemByTile(FilePath, HorBtnIter, IterCount); break;
    case EViewMode::List: IsClicked = DrawItemByList(FilePath, HorBtnIter, IterCount); break;
    }

    return IsClicked;
}

bool CContentView::DrawItemByList(const FileOptData& InitFileName, size_t& HorBtnIter, const size_t IterCount)
{
    if (InitFileName.File.empty())
        return false;

    xr_path FilePath = InitFileName.File;
    const ImVec2& CursorPos = ImGui::GetCursorPos();

    xr_string FileName = FilePath.xfilename();
    IconData* IconPtr = nullptr;

    bool OutValue = false;
    //if (Contains())
    {
        ImVec4* colors = ImGui::GetStyle().Colors;
        IconPtr = InitFileName.IsDir ? &GetTexture("Folder") : &GetTexture(FilePath);
        ImVec4 IconColor = IconPtr->UseButtonColor ? colors[ImGuiCol_CheckMark] : ImVec4(1, 1, 1, 1);

        if (!IconPtr->Icon)
            return false;

        if (FilePath == CopyObjectPath && IsCutting)
            IconColor.w = 0.3;

        OutValue = ImGui::ImageButton
        (
            FileName.c_str(),
            IconPtr->Icon->pSurface, BtnSize,
            ImVec2(0, 0), ImVec2(1, 1),
            ImVec4(0, 0, 0, 0), IconColor
        );
    }

    ImVec4 TextColor = ImGui::GetStyle().Colors[ImGuiCol_Text];

    bool RenameThisItem = RenameObject.Active && RenameObject.Path == FilePath;

    if (!RenameThisItem && DrawItemHelper(FilePath, FileName, InitFileName, IconPtr))
        TextColor.w = 0.3;

    ImVec2 NextCursorPos = ImGui::GetCursorPos();
    ImGui::SameLine(); 

    ImVec2 StartCursorPos = ImGui::GetCursorPos();
    StartCursorPos.y += BtnSize.y / 2.f;
    StartCursorPos.y -= (TextHeight + 2) ;
    ImGui::SetCursorPos(StartCursorPos);

    if (RenameObject.Path == FilePath)
    {
        if (RenameObject.Active)
        {
            if (RenameObject.SetText)
            {
                RenameObject.SetText = false;
                RenameObject.RenameBuf = Platform::ANSI_TO_UTF8(FileName).c_str();
                ImGui::SetKeyboardFocusHere();
            }

            ImGuiIO& io = ImGui::GetIO();

            if (ImGui::InputText("##ren", RenameObject.RenameBuf.data(), 255, ImGuiInputTextFlags_EnterReturnsTrue))
                RenameObject.Active = false;
            
            if (io.KeysDown[ImGuiKey_Escape])
                RenameActionEnd();

            RenameObject.Focus = ImGui::IsItemHovered();
        }
        else
        {
            if (strcmp(Platform::ANSI_TO_UTF8(FileName).c_str(), RenameObject.RenameBuf.c_str()))
                RenameAction(FilePath, RenameObject.RenameBuf.c_str());

            RenameActionEnd();
        }
    }
    else
    {
        ImGui::TextColored(TextColor, Platform::ANSI_TO_UTF8(FileName).c_str());

        if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
        {
            if (FilePath.xstring() != ".." && !FilePath.parent_path().empty() && !IsSpawnElement)
                RenameActionActivate(FilePath);
        }
    }
    
    StartCursorPos.y += TextHeight + 2.f;
    ImGui::SetCursorPos(StartCursorPos);

    ImVec4 TooltipTextColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
    TooltipTextColor.w *= 0.5f;

    {
        if (InitFileName.IsDir)
        {
            ImGui::TextColored(TooltipTextColor, "Directory");
        }
        else if (FileName.ends_with(".dds"))
        {
            ImGui::TextColored(TooltipTextColor, "Texture Asset");
        }
        else if (FileName.ends_with(".tga"))
        {
            ImGui::TextColored(TooltipTextColor, "Raw Texture Asset");
        }
        else if (FileName.ends_with(".png"))
        {
            ImGui::TextColored(TooltipTextColor, "Image");
        }
        else if (FileName.ends_with(".object"))
        {
            ImGui::TextColored(TooltipTextColor, "Object Asset");
        }
        else if (FileName.ends_with(".group"))
        {
            ImGui::TextColored(TooltipTextColor, "Group object Asset");
        }
        else if (FileName.ends_with(".ogf"))
        {
            ImGui::TextColored(TooltipTextColor, "Object");
        }
        else if (FileName.ends_with(".wav"))
        {
            ImGui::TextColored(TooltipTextColor, "Raw Sound");
        }
        else if (FileName.ends_with(".ogg"))
        {
            ImGui::TextColored(TooltipTextColor, "Sound Asset");
        }
        else if (FileName.ends_with(".ise"))
        {
            ImGui::TextColored(TooltipTextColor, "Spawn Component");
        }
        else if (FileName.ends_with(".skl"))
        {
            ImGui::TextColored(TooltipTextColor, "Raw Single Animation Asset");
        }
        else if (FileName.ends_with(".skls"))
        {
            ImGui::TextColored(TooltipTextColor, "Raw Animations Asset");
        }
        else if (FileName.ends_with(".omf"))
        {
            ImGui::TextColored(TooltipTextColor, "Animations Asset");
        }
        else if (FileName.ends_with(".ltx"))
        {
            xr_string PathName = FilePath;
            if (PathName.Contains("scripts\\"))
            {
                ImGui::TextColored(TooltipTextColor, "Logic Preference");
            }
            else
            {
                ImGui::TextColored(TooltipTextColor, "Config");
            }
        }
        else if (FileName.ends_with(".script"))
        {
            ImGui::TextColored(TooltipTextColor, "Lua Script");
        }
    }
    ImGui::SetCursorPos(NextCursorPos);
    ImGui::Separator();

    return OutValue;
}


void CContentView::AcceptDragDropAction(xr_path& FilePath)
{
    if (!std::filesystem::is_directory(FilePath) || (FilePath==".." && xr_path(CurrentDir).parent_path().empty()) || IsSpawnElement || !ImGui::BeginDragDropTarget())
    {
        return;
    }
    
    auto ImData = ImGui::AcceptDragDropPayload("TEST");

    if (ImData == nullptr)
        ImData = ImGui::AcceptDragDropPayload("FLDR");

    if (ImData == nullptr)
        ImData = ImGui::AcceptDragDropPayload("OTHR");

    if (ImData != nullptr)
    {
        if (ImData != nullptr)
            Data = *(DragDropData*)ImData->Data;

        if (Data.FileName != FilePath.xstring()) //На всякий случай
        {
            CutAction(Data.FileName);
            PasteAction(FilePath);
        }
    }

    ImGui::EndDragDropTarget();
}

bool CContentView::BeginDragDropAction(xr_path& FilePath, xr_string& FileName, const CContentView::FileOptData& InitFileName, CContentView::IconData* IconPtr)
{
    /*
        Разделение на 3 типа dnd:

            TEST - ".object", ".group", ".ise"
                    Объекты, которые принимает вьюпорт

            FLDR - Только папки.
                    Для работы только в Content View

            OTHR - Все иные объекты.
                    В дальнейшем можно как-нибудь использовать.
                    Или просто удалить совместив с FLDR.
    */

    bool WeCanDrag = false;

    
    if (FilePath.has_extension()) //File DnD
    {
        xr_string Extension = FilePath.extension().string().c_str();
        WeCanDrag = Extension == ".object" || Extension == ".group" || Extension == ".ise" || Extension == ".dti";

        if (!ImGui::BeginDragDropSource())
        {
            return false;
        }

        if (WeCanDrag)
        {
            if (IsSpawnElement || FilePath.xstring().ends_with(".ise"))
            {
                if (InitFileName.ISESect.size() > 0)
                {
                    Data.FileName = InitFileName.ISESect.c_str();
                }
            }
            else
            {
                Data.FileName = FilePath;
            }

            xr_string PayloadName = "TEST";
            if (FilePath.xstring().ends_with(".dti"))
            {
                PayloadName += "#dti";
            }

            ImGui::SetDragDropPayload(PayloadName.c_str(), &Data, sizeof(DragDropData));
        }
        else 
        {
            Data.FileName = FilePath;
            ImGui::SetDragDropPayload("OTHR", &Data, sizeof(DragDropData));
        }
    }
    else
    {
        if (
                FilePath == ".." || 
                !std::filesystem::is_directory(FilePath) ||
                FilePath.parent_path().empty() || 
                !ImGui::BeginDragDropSource()
           )
        {
            return false;
        }

        Data.FileName = FilePath;
        ImGui::SetDragDropPayload("FLDR", &Data, sizeof(DragDropData));
    }

    xr_string LabelText = FilePath.has_extension() ? FileName.substr(0, FileName.length() - FilePath.extension().string().length()).c_str() : FileName.c_str();

    ImGui::ImageButton(FilePath.xfilename().c_str(), IconPtr->Icon->pSurface, BtnSize);
    ImGui::Text(LabelText.data());
    ImGui::EndDragDropSource();
    return true; 
}

bool CContentView::DrawItemHelper(xr_path& FilePath, xr_string& FileName, const CContentView::FileOptData& InitFileName, CContentView::IconData* IconPtr)
{
    if (!DrawContext(FilePath))
    {
        if (ViewMode == EViewMode::Tile && ImGui::IsItemHovered())
        {
            ImVec2 DrawHintPos = ImGui::GetMousePos() - ImGui::GetWindowPos() + ImVec2{ ImGui::GetScrollX(), ImGui::GetScrollY() };
            DrawHintPos.y -= 15;
            CurrentItemHint = { Platform::ANSI_TO_UTF8(FileName) ,DrawHintPos, true };
        }
    }

    AcceptDragDropAction(FilePath);
    return BeginDragDropAction(FilePath, FileName, InitFileName, IconPtr);
}


bool CContentView::DrawItemByTile(const FileOptData& InitFileName, size_t& HorBtnIter, const size_t IterCount)
{
    if (InitFileName.File.empty())
        return false;
    
    xr_path FilePath = InitFileName.File;
    xr_string FileName = FilePath.xfilename();

    if (FileName.empty())
        return false;

    const ImVec2& CursorPos = ImGui::GetCursorPos();
    IconData* IconPtr = nullptr;
    bool OutValue = false;
    
    auto InvalidateLambda = [&FileName, this, &CursorPos, &HorBtnIter, IterCount]()
    {
        if (HorBtnIter != IterCount)
        {
            ImGui::SetCursorPosY(CursorPos.y);
            ImGui::SetCursorPosX(CursorPos.x + 15 + BtnSize.x);
            HorBtnIter++;
        }
        else
        {
            HorBtnIter = 0;
        }
    };

    if (Contains())
    {
        ImVec4* colors = ImGui::GetStyle().Colors;

        if (InitFileName.IsDir)
        {
            IconPtr = &GetTexture("Folder");
        }
        else if (InitFileName.ISESect.size() > 0)
        {
            xr_string HackName = InitFileName.ISESect.c_str();
            HackName += ".ise";

            IconPtr = &GetTexture(HackName.c_str());
        }
        else
        {
            IconPtr = &GetTexture(FilePath);
        }
        ImVec4 IconColor = IconPtr->UseButtonColor ? colors[ImGuiCol_CheckMark] : ImVec4(1, 1, 1, 1);

        if (!IconPtr->Icon)
            return false;

        if (FilePath == CopyObjectPath && IsCutting)
            IconColor.w = 0.3;

        OutValue = ImGui::ImageButton
        (
            FileName.c_str(),
            IconPtr->Icon->pSurface, BtnSize,
            ImVec2(0, 0), ImVec2(1, 1),
            ImVec4(0, 0, 0, 0), IconColor
        );
    }
    else
    {
        ImGui::Button(FileName.c_str(), BtnSize);
        ImGui::Text(FileName.c_str());

        InvalidateLambda();
        return false;
    }

    ImVec4 TextColor = ImGui::GetStyle().Colors[ImGuiCol_Text];

    bool RenameThisItem = RenameObject.Active && RenameObject.Path == FilePath;

    if (!RenameThisItem && DrawItemHelper(FilePath, FileName, InitFileName, IconPtr))
        TextColor.w = 0.3;

    xr_string LabelText = FilePath.has_extension() ? FileName.substr(0, FileName.length() - FilePath.extension().string().length()).c_str() : FileName.c_str();
    
    if (RenameObject.Path == FilePath)
    {
        if (RenameObject.Active)
        {
            if (RenameObject.SetText)
            {
                RenameObject.SetText = false;
                RenameObject.RenameBuf = Platform::ANSI_TO_UTF8(LabelText);
                ImGui::SetKeyboardFocusHere();
            }

            ImGuiIO& io = ImGui::GetIO();

            ImGui::SetCursorPosX(CursorPos.x);
            ImGui::SetNextItemWidth(BtnSize.x + 10);
            if (ImGui::InputText("##ren", RenameObject.RenameBuf.data(), 255,ImGuiInputTextFlags_EnterReturnsTrue))
                RenameObject.Active = false;
             
            if (io.KeysDown[ImGuiKey_Escape])
                RenameActionEnd();

            RenameObject.Focus = ImGui::IsItemHovered();
        }
        else 
        {
            if (strcmp(Platform::ANSI_TO_UTF8(LabelText).c_str(), RenameObject.RenameBuf.c_str()))
                RenameAction(FilePath, RenameObject.RenameBuf.c_str());
            
            RenameActionEnd();
        }
    }
    else 
    {
        float TextPixels = ImGui::CalcTextSize(Platform::ANSI_TO_UTF8(LabelText).data()).x;

        while (TextPixels > BtnSize.x)
        {
            LabelText = LabelText.substr(0, LabelText.length() - 4) + "..";
            TextPixels = ImGui::CalcTextSize(Platform::ANSI_TO_UTF8(LabelText).data()).x;
        }

        ImGui::SetCursorPosX(CursorPos.x + (((10 + BtnSize.x) - TextPixels) / 2));
        ImGui::TextColored(TextColor, Platform::ANSI_TO_UTF8(LabelText).data());

        if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
        {
            if (FilePath.xstring() != ".." && !FilePath.parent_path().empty() && !IsSpawnElement)
            RenameActionActivate(FilePath);
        }
    }

    InvalidateLambda();
    return OutValue;
}

bool CContentView::Contains()
{
    float ScrollValue = ImGui::GetScrollY();
    float CursorPosY = ImGui::GetCursorPosY();

    bool IsNotAfter = CursorPosY < ScrollValue + ImGui::GetWindowSize().y;
    bool IsNotBefor = CursorPosY > ScrollValue - BtnSize.y;
    return IsNotAfter && IsNotBefor;
}



bool CContentView::CheckFile(const xr_path& File) const
{
    bool TestTHM = IsThmMode || (File.has_extension() && File.extension().string() != ".thm");
    bool TestWinTrash = File.xfilename() != "desktop.ini";

    return TestTHM && TestWinTrash;
}

bool CContentView::DrawFormContext()
{
    if (!ImGui::BeginPopupContextItem("##contentbrowsercontext"))
    {
        return false;
    }

    ImGui::BeginDisabled(CopyObjectPath.empty());
    if (ImGui::MenuItem("Paste"))
    {
        PasteAction(CurrentDir);
    }
    ImGui::EndDisabled();

    ImGui::Separator();

    if (ImGui::MenuItem("Create Folder"))
    {
        CreateAction();
    }
    ImGui::EndPopup();
    return true;
}

bool CContentView::DrawContext(const xr_path& Path)
{
    if (Path.xstring() == ".." || Path.parent_path().empty() || !ImGui::BeginPopupContextItem())
    {
        return false;
    }


    if (Path.has_extension() && Path.extension().string() == ".level")
    {
        if (ImGui::MenuItem("Open"))
        {
            UI->SetStatus("Level loading...");
            ExecCommand(COMMAND_CLEAR);
            FS.TryLoad(Path.xstring());
            IReader* R = FS.r_open(Path.xstring().c_str());
            if (!R)
            {
                ImGui::EndPopup();
                return false;
            }
            char ch;
            R->r(&ch, sizeof(ch));
            bool is_ltx = (ch == '[');
            FS.r_close(R);
            bool res;
            LTools->m_LastFileName = Path.xstring();

            if (is_ltx)
                Scene->LoadLTX(Path.xstring().c_str(), false);
            else
                Scene->Load(Path.xstring().c_str(), false);
        }
        ImGui::Separator();
    }

    if (Path.has_extension() && Path.extension().string() == ".wav")
    {
        if (ImGui::MenuItem("Open"))
        {
            ExecCommand(COMMAND_SOUND_EDITOR, xr_path(Path.stem()).xstring());
        }

        ImGui::Separator();
    }


    if (ImGui::MenuItem("Cut"))
    {
        CutAction(Path);
    }
    if (ImGui::MenuItem("Copy"))
    {
        CopyAction(Path);
    }
    if (ImGui::MenuItem("Rename"))
    {
        RenameActionActivate(Path);
    }
    if (ImGui::MenuItem("Delete"))
    {
        DeleteAction(Path);
    }

    ImGui::Separator();


    if (ImGui::BeginMenu("Properties"))
    {
        if (ImGui::MenuItem("Change icon"))
        {
            ExecCommand(COMMAND_ICON_PICKER, Path.xstring());
        }


        ImGui::EndMenu();
    }


    

    bool ShowConvert = false;

    const xr_set<xr_string> supportedExtensionsConvert = { ".dds", ".tga", ".png", ".wav" };

    if (Path.has_extension() && supportedExtensionsConvert.count(xr_path(Path.extension()).xstring()) > 0)
    {
        ImGui::Separator();

        if (auto ex = xr_path(Path.extension()).xstring(); ImGui::BeginMenu("Convert"))
        {
            if ((ex == ".dds" || ex == ".png") && ImGui::MenuItem("TGA"))
            {
                xr_path OutFile = Path;
                OutFile.replace_extension(".tga");

                DXTUtils::Converter::MakeTGA(Path, OutFile);

            }

            if ((ex == ".dds" || ex == ".tga") && ImGui::MenuItem("PNG"))
            {
                xr_path OutFile = Path;
                OutFile.replace_extension(".png");

                DXTUtils::Converter::MakePNG(Path, OutFile);
            }

            if (ex == ".wav" && ImGui::MenuItem("OGG"))
            {
                xr_path OutFile = Path;
                OutFile.replace_extension(".ogg");
                
                xr_string stem = xr_path(Path.stem());
                ESoundThumbnail* pTHM = new ESoundThumbnail(stem.c_str());
                
                SndLib->MakeGameSound(pTHM, Path.xstring().c_str(), OutFile.xstring().c_str());
                xr_delete(pTHM);
            }

            ImGui::EndMenu();
        }
    }

    ImGui::EndPopup();
    return true;
}

CContentView::IconData & CContentView::GetTexture(const xr_string & IconPath)
{
    if (Icons.contains(IconPath))
        return Icons[IconPath];

    if (IconPath.Contains(".~"))
        return Icons["backup"];

    if (IconPath.ends_with(".ltx"))
        return Icons["thm"];
    
    if (IconPath.ends_with(".ogg"))
        return Icons["ogg"];
    
    if (IconPath.ends_with(".level"))
        return Icons["level"];
    
    if (IconPath.ends_with(".wav"))
        return Icons["wav"];

    if (IconPath.ends_with(".seq"))
        return Icons["seq"];

    if (IconPath.ends_with(".dll"))
        return Icons["dll"];

    if (IconPath.ends_with(".exe"))
        return Icons["exe"];

    if (IconPath.ends_with("$") && IconPath.starts_with("$"))
        return Icons["Folder"];

    if (IconPath.ends_with(".cmd") ||
        IconPath.ends_with(".bat"))
        return Icons["cmd"];
    
    if (IconPath.Contains(LogsDir))
        return Icons["logs"];

    if (!Icons.contains(IconPath))
    {
        if (IconPath.ends_with(".ise"))
        {
            if (IconPath == "$env_mod.ise")
            {
                return Icons["env_mod"];
            }

            ESceneSpawnTool* SpTool = (ESceneSpawnTool*)Scene->GetTool(OBJCLASS_SPAWNPOINT);
            xr_string ValidPath = IconPath;
            ValidPath = ValidPath.erase(ValidPath.length() - 4);

            if (pSettings->line_exist(ValidPath.data(), "$ed_icon"))
            {
                Icons[IconPath] = { EDevice->Resources->_CreateTexture(pSettings->r_string_wb(ValidPath.data(), "$ed_icon").c_str()), false };
                Icons[IconPath].Icon->Load();
            }
            else
            {
                Icons[IconPath] = Icons["file"];
            }
        }
        else if (IconPath.ends_with(".object"))
        {
            string_path fn = {};
            FS.update_path(fn, _objects_, fn);
            Icons[IconPath] = Icons["object"];

            if(IconPath.find(fn) != xr_string::npos) {
                xr_string NewPath = IconPath.substr(IconPath.find(fn) + xr_strlen(fn));

                EObjectThumbnail* m_Thm = (EObjectThumbnail*)ImageLib.CreateThumbnail(NewPath.data(), EImageThumbnail::ETObject);
                CTexture* TempTexture = new CTexture();
                m_Thm->Update(TempTexture->pSurface);

                if(TempTexture->pSurface != nullptr) {
                    Icons[IconPath] = {TempTexture, false};
                }
                else {
                    xr_delete(TempTexture);
                }
            }
        }
        else if (IconPath.ends_with(".group"))
        {
            string_path fn = {};
            FS.update_path(fn, _groups_, "");
            Icons[IconPath] = Icons["object"];

            if (IconPath.find(fn) != xr_string::npos) {
                xr_string NewPath = IconPath.substr(IconPath.find(fn) + xr_strlen(fn));

                EGroupThumbnail* m_Thm = new EGroupThumbnail(NewPath.data());
                //EObjectThumbnail* m_Thm = (EObjectThumbnail*)ImageLib.CreateThumbnail(NewPath.data(), EImageThumbnail::ETTexture);
                CTexture* TempTexture = new CTexture();
                m_Thm->Update(TempTexture->pSurface);

                if (TempTexture->pSurface != nullptr) {
                    Icons[IconPath] = { TempTexture, false };
                }
                else {
                    xr_delete(TempTexture);
                }
            }
        }
        else if (IconPath.ends_with(".png") || IconPath.ends_with(".tga"))
        {
            U8Vec Pixels = DXTUtils::GitPixels(IconPath.c_str(), BtnSize.x, BtnSize.y);
            if (!Pixels.empty())
            {
                CTexture* TempTexture = new CTexture();
                ID3DTexture2D* pTexture = nullptr;
                Icons[IconPath] = { TempTexture, false };
                R_CHK(REDevice->CreateTexture(BtnSize.x, BtnSize.x, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, 0));
                {
                    D3DLOCKED_RECT rect;
                    R_CHK(pTexture->LockRect(0, &rect, 0, D3DLOCK_DISCARD));
                    memcpy(rect.pBits, Pixels.data(), Pixels.size());
                    R_CHK(pTexture->UnlockRect(0));

                    TempTexture->pSurface = pTexture;
                }
            }
            else if (IconPath.ends_with(".tga"))
            {
                Icons[IconPath] = Icons["tga"];
            }
        }
        else if(IconPath.ends_with(".dds")) 
        {
            xr_string NewPath = IconPath.substr(0, IconPath.length() - 4);

            Icons[IconPath] = {EDevice->Resources->_CreateTexture(NewPath.c_str()), false};
            Icons[IconPath].Icon->Load();

            if(!Icons[IconPath].Icon->pSurface) {
                Icons[IconPath] = Icons["image"];
            }
        }
        else
        {
            Icons[IconPath] = Icons["file"];
        }
    }

    return Icons[IconPath];
}

xr_map<xr_string, CContentView::FileOptData> CContentView::ScanConfigs(const xr_string& StartPath)
{
    xr_map<xr_string, FileOptData> TempPath;
    CInifile::Root& data = ((CInifile*)pSettings)->sections();

    for (CInifile::RootIt it = data.begin(); it != data.end(); it++)
    {
        LPCSTR val;
        if ((*it)->line_exist("$spawn", &val))
        {
            shared_str caption = pSettings->r_string_wb((*it)->Name, "$spawn");
            shared_str sect = (*it)->Name;
            if (caption.size())
            {
                xr_string FileName = caption.c_str();

                if (!FileName.Contains(StartPath) && !StartPath.empty())
                    continue;

                if (FileName == StartPath)
                    continue;

                if (StartPath.empty())
                {
                    size_t DirStart = FileName.find('\\');

                    if (DirStart != xr_string::npos)
                    {
                        xr_string DirName = FileName.substr(0, DirStart);
                        if (TempPath.contains(DirName))
                            continue;

                        TempPath[DirName] = { DirName.c_str(), true };
                        continue;
                    }
                }
                else
                {
                    xr_string Delimer = StartPath;
                    if (!Delimer.ends_with('\\'))
                    {
                        Delimer += '\\';
                    }

                    size_t DirStart = FileName.find(Delimer);

                    if (DirStart != xr_string::npos)
                    {
                        xr_string DirName = FileName.substr(DirStart + Delimer.length());
                        if (TempPath.contains(DirName))
                            continue;

                        int DirIter = DirName.find('\\');
                        if (DirIter != xr_string::npos)
                        {
                            xr_string ExtractedDirName = DirName.substr(0, DirIter);
                            TempPath[ExtractedDirName] = { ExtractedDirName.c_str(), true };
                        }
                        else
                        {
                            TempPath[DirName] = { (DirName + ".ise").c_str(), false, sect };
                        }
                    }

                    continue;
                }

                TempPath[FileName] = { (FileName + ".ise").c_str(), false, sect };
            }
        }
    }

    return std::move(TempPath);
}

#pragma region ObjectActions

void CContentView::CheckFileNameRecursive(xr_path& FilePath, const xr_string& postfix) const
{
    xr_path NewFileName = FilePath.stem();
    NewFileName += " - ";
    NewFileName += postfix.c_str();
    NewFileName += FilePath.extension();

    FilePath.replace_filename(NewFileName);
    if (std::filesystem::exists(FilePath))
    {
        CheckFileNameRecursive(FilePath, postfix);
    }

    return;
}

void CContentView::PasteAction(const xr_string& Path) /*const*/
{
    xr_path OutDir = ((Path == "..") ? CurrentDir / xr_path(Path) : xr_path(Path)) / CopyObjectPath.xfilename();

    if (CopyObjectPath == OutDir || std::filesystem::exists(OutDir))
    {
        CheckFileNameRecursive(OutDir, "Copy");
    }

    if (std::filesystem::is_directory(CopyObjectPath))
    {
        std::filesystem::copy(CopyObjectPath, OutDir, std::filesystem::copy_options::recursive);
    }
    else 
    {
        std::filesystem::copy(CopyObjectPath, OutDir);

        if (auto ThmFile = CopyObjectPath; ShouldTheFileHaveTHM(CopyObjectPath) && 
            ThmFile.extension() != ".thm" && std::filesystem::exists(ThmFile.replace_extension(".thm")))
        {
            std::filesystem::copy(ThmFile, OutDir.replace_extension(".thm"));
        }
    }
    if (IsCutting)
    {
        DeleteAction(CopyObjectPath);
        IsCutting = false;
    }

    CopyObjectPath.clear();

    FS.rescan_path(OutDir.parent_path().string().c_str(), true);
}

void CContentView::DeleteAction(const xr_path& Path) /*const*/
{
    if (std::filesystem::is_directory(Path))
    {
        std::filesystem::remove_all(Path);
    }
    else
    {
        std::filesystem::remove(Path);

        if (auto ThmFile = Path; ShouldTheFileHaveTHM(Path) && 
            Path.extension() != ".thm" && std::filesystem::exists(ThmFile.replace_extension(".thm")))
        {
            std::filesystem::remove(ThmFile);
        }
    }

    // For some reason, FS does not want to register that the file has been deleted. \
                Temporarily removed the "const" and made the Rescan Directory();

        //FS.rescan_path(Path.parent_path().string().c_str() , true);
    RescanDirectory();
}

void CContentView::CopyAction(const xr_path& Path) const
{
    CopyObjectPath = Path;
    IsCutting = false;
}

void CContentView::CutAction(const xr_path& Path) const
{
    CopyAction(Path);
    IsCutting = true;
}

void CContentView::RenameAction(const xr_path& FilePath, const xr_string NewName)
{
    xr_path TempFileName = xr_path(FilePath).replace_filename(std::filesystem::path(std::tmpnam(nullptr)).stem());
    xr_path NewFileName = FilePath;
    NewFileName.replace_filename(NewName.c_str());
    NewFileName.replace_extension(FilePath.extension());

    std::filesystem::rename(FilePath, TempFileName);

    if (std::filesystem::exists(NewFileName))
    {
        CheckFileNameRecursive(NewFileName, "Copy");
    }

    std::filesystem::rename(TempFileName, NewFileName);

    if (!std::filesystem::is_directory(NewFileName) && ShouldTheFileHaveTHM(FilePath))
    {
        if (auto ThmFile = FilePath; ThmFile.extension() != ".thm" && std::filesystem::exists(ThmFile.replace_extension(".thm")))
        {
            std::filesystem::rename(ThmFile, NewFileName.replace_extension(".thm"));
        }
    }

    FS.rescan_path(NewFileName.parent_path().string().c_str(), true);

}

void CContentView::RenameActionActivate(const xr_path& Path)
{
    RenameObject.SetText = true;
    RenameObject.Focus = false;
    RenameObject.Active = true;
    RenameObject.RenameBuf.clear();
    RenameObject.Path = Path;
}

void CContentView::RenameActionEnd()
{
    RenameObject.Active = false;
    RenameObject.RenameBuf.clear();
    RenameObject.Path.clear();
}

bool CContentView::ShouldTheFileHaveTHM(const xr_path& file) const
{
    if (!file.has_extension())
        return false;

    if (auto e = file.extension();
        e == ".group" || e == ".object" || e == ".dds" || e == ".tga")
        return true;

    return (false);
}

void CContentView::CreateAction() /*const*/
{
    xr_path OutDir = CurrentDir / xr_path("Folder");

    if (std::filesystem::exists(OutDir))
    {
        CheckFileNameRecursive(OutDir, "New");
    }

    std::filesystem::create_directory(OutDir);

    RenameActionActivate(OutDir);

    // For some reason, FS does not want to register that the file has been deleted. \
                Temporarily removed the "const" and made the Rescan Directory();
    RescanDirectory();
}
#pragma endregion
