#include "resource.h"
#include "../../xrCore/xrCore.h"
#include <time.h>
#include <mmsystem.h>
#include <CommCtrl.h>
#include "cl_log.h"

i_lc_log* lc_log = 0;
//************************* Log-thread data
static xrCriticalSection    csLog
#ifdef PROFILE_CRITICAL_SECTIONS
(MUTEX_PROFILE_ID(csLog))
#endif // PROFILE_CRITICAL_SECTIONS
;

xr_queue<xr_string> myLogQueue;
void MyLogCallback(const char* string) {
    xrCriticalSectionGuard LogGuard(&csLog);
    myLogQueue.push(string);
}

volatile BOOL                bClose                = FALSE;

static char                    status    [1024    ]    ="";
static char                    phase    [1024    ]    ="";
static float                progress            = 0.0f;
static u32                    phase_start_time    = 0;
static BOOL                    bStatusChange        = FALSE;
static BOOL                    bPhaseChange        = FALSE;
static u32                    phase_total_time    = 0;

static HWND hwLog        = 0;
static HWND hwProgress    = 0;
static HWND hwInfo        = 0;
static HWND hwStage        = 0;
static HWND hwTime        = 0;
static HWND hwPText        = 0;
static HWND hwPhaseTime    = 0;

//************************* Log-thread data
static INT_PTR CALLBACK logDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp ) {
    switch( msg ){
        case WM_DESTROY:
            break;
        case WM_CLOSE:
            ExitProcess        (0);
//            bClose = TRUE;
            break;
        case WM_COMMAND:
            if( LOWORD(wp)==IDCANCEL )
            {
                ExitProcess    (0);
//                bClose = TRUE;
            }
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

static void _process_messages(void) {
    MSG msg;
    if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) 
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

std::string make_time    (u32 sec) {
    char        buf[64];
    xr_sprintf        (buf,"%2.0d:%2.0d:%2.0d",sec/3600,(sec%3600)/60,sec%60);
    int len        = int(xr_strlen(buf));
    for (int i=0; i<len; i++) if (buf[i]==' ') buf[i]='0';
    return std::string(buf);
}

void __cdecl Status(const char* format, ...) {
    csLog.Enter();
    va_list                mark;
    va_start(mark, format);
    vsprintf(status, format, mark);
    bStatusChange = TRUE;
    Msg("    | %s", status);
    csLog.Leave();
}

void Progress(const float F) {
    progress = F;
}

void Phase(const char* phase_name) {
    while (!(hwPhaseTime && hwStage)) Sleep(1);

    csLog.Enter();
    // Replace phase name with TIME:Name 
    char    tbuf[512];
    bPhaseChange = TRUE;
    phase_total_time = timeGetTime() - phase_start_time;
    xr_sprintf(tbuf, "%s : %s", make_time(phase_total_time / 1000).c_str(), phase);
    SendMessageA(hwPhaseTime, LB_DELETESTRING, SendMessageA(hwPhaseTime, LB_GETCOUNT, 0, 0) - 1, 0);
    SendMessageA(hwPhaseTime, LB_ADDSTRING, 0, (LPARAM)tbuf);

    // Start _new phase
    phase_start_time = timeGetTime();
    xr_strcpy(phase, phase_name);
    SetWindowTextA(hwStage, phase_name);
    xr_sprintf(tbuf, "--:--:-- * %s", phase);
    SendMessageA(hwPhaseTime, LB_ADDSTRING, 0, (LPARAM)tbuf);
    SendMessageA(hwPhaseTime, LB_SETTOPINDEX, SendMessageA(hwPhaseTime, LB_GETCOUNT, 0, 0) - 1, 0);
    Progress(0);

    // Release focus
    Msg("\n* New phase started: %s", phase_name);
    csLog.Leave();
}

// TODO: windows specific stuff, dunno about Linux
HWND logWindow=0;
void logThread(void* dummy) {
    SetProcessPriorityBoost(GetCurrentProcess(), TRUE);

    logWindow = CreateDialog(
        HINSTANCE(GetModuleHandle(0)),
        MAKEINTRESOURCE(IDD_LOG),
        0, logDlgProc);
    if (!logWindow) {
        R_CHK(GetLastError());
    };
    SetWindowPos(logWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    hwLog = GetDlgItem(logWindow, IDC_LOG);
    hwProgress = GetDlgItem(logWindow, IDC_PROGRESS);
    hwInfo = GetDlgItem(logWindow, IDC_INFO);
    hwStage = GetDlgItem(logWindow, IDC_STAGE);
    hwTime = GetDlgItem(logWindow, IDC_TIMING);
    hwPText = GetDlgItem(logWindow, IDC_P_TEXT);
    hwPhaseTime = GetDlgItem(logWindow, IDC_PHASE_TIME);

    SendMessageA(hwProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 1000));
    SendMessageA(hwProgress, PBM_SETPOS, 0, 0);

    Msg("\"LevelBuilder v4.1\" beta build\nCompilation date: %s\n", __DATE__);
    {
        char tmpbuf[128];
        Msg("Startup time: %s", _strtime(tmpbuf));
    }

    BOOL        bHighPriority = FALSE;
    string256    u_name;
    unsigned long        u_size = sizeof(u_name) - 1;
    GetUserNameA(u_name, &u_size);
    _strlwr(u_name);
    if ((0 == xr_strcmp(u_name, "oles")) || (0 == xr_strcmp(u_name, "alexmx")))    bHighPriority = TRUE;

    // Main cycle
    u32        LogSize = 0;
    float    PrSave = 0;
    xrLogger::AddLogCallback(MyLogCallback);

    while (TRUE)
    {
        SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);    // bHighPriority?NORMAL_PRIORITY_CLASS:IDLE_PRIORITY_CLASS

        // transfer data
        while (!csLog.TryEnter()) {
            _process_messages();
            Sleep(1);
        }
        if (progress > 1.f)        progress = 1.f;
        else if (progress < 0)    progress = 0;

        BOOL bWasChanges = FALSE;
        char tbuf[256];
        {
            xrCriticalSectionGuard LogGuard(&csLog);

            if (LogSize != myLogQueue.size())
            {
                bWasChanges = TRUE;
                for (size_t Iter = 0; Iter < myLogQueue.size(); Iter++)
                {
                    string256 S = {};
                    xr_strcpy(S, myLogQueue.front().c_str());
                    
                    if (S[0])
                        SendMessageA(hwLog, LB_ADDSTRING, 0, (LPARAM)S);
                    myLogQueue.pop();
                }
                SendMessageA(hwLog, LB_SETTOPINDEX, LogSize - 1, 0);
            }
        }
        if (_abs(PrSave - progress) > EPS_L) {
            bWasChanges = TRUE;
            PrSave = progress;
            SendMessageA(hwProgress, PBM_SETPOS, u32(progress * 1000.f), 0);

            // timing
            if (progress > 0.005f) {
                u32 dwCurrentTime = timeGetTime();
                u32 dwTimeDiff = dwCurrentTime - phase_start_time;
                u32 secElapsed = dwTimeDiff / 1000;
                u32 secRemain = u32(float(secElapsed) / progress) - secElapsed;
                xr_sprintf(tbuf,
                    "Elapsed: %s\n"
                    "Remain:  %s",
                    make_time(secElapsed).c_str(),
                    make_time(secRemain).c_str()
                );
                SetWindowTextA(hwTime, tbuf);
            }
            else {
                SetWindowTextA(hwTime, "");
            }

            // percentage text
            xr_sprintf(tbuf, "%3.2f%%", progress * 100.f);
            SetWindowTextA(hwPText, tbuf);
        }

        if (bStatusChange) {
            bWasChanges = TRUE;
            bStatusChange = FALSE;
            SetWindowTextA(hwInfo, status);
        }
        if (bWasChanges) {
            UpdateWindow(logWindow);
            bWasChanges = FALSE;
        }
        csLog.Leave();

        _process_messages();
        if (bClose)            break;
        Sleep(200);
    }

    // Cleanup
    DestroyWindow(logWindow);
}

void clLog(LPCSTR msg) {
    csLog.Enter();
    Log(msg);
    csLog.Leave();
}

void __cdecl clMsg(const char* format, ...) {
    va_list        mark;
    char buf[4 * 256];
    va_start(mark, format);
    vsprintf(buf, format, mark);


    string1024        _out_;
    xr_strconcat(_out_, "    |    | ", buf);
    clLog(_out_);
}

class client_log_impl : public i_lc_log {
    virtual void clMsg(LPCSTR msg) override { ::clMsg(msg); }
    virtual void clLog(LPCSTR msg) override { ::clLog(msg); }
    virtual void Status(LPCSTR msg) override { ::Status(msg); }
    virtual    void Progress(const float F) override { ::Progress(F); }
    virtual    void Phase(LPCSTR phase_name) override { ::Phase(phase_name); }
public:
    client_log_impl() { lc_log = this; }
} client_log_impl;
