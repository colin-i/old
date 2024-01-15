
#define WINVER _WIN32_WINNT_WIN7

#include "resource.h"

#include <new>
#include <windows.h>
#include <windowsx.h>  //HANDLE_MSG
#include <mfplay.h>
//#include <mferror.h>
//#include <shobjidl.h>   // defines IFileOpenDialog
#include <strsafe.h> //StringCbPrintf

//#include "Slider.h"
#include <commctrl.h>

static BOOL    InitializeWindow(HWND *pHwnd);
static HRESULT PlayMediaFile(HWND hwnd, const WCHAR *sURL);
static void    ShowErrorMessage(PCWSTR format, HRESULT hr);

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Window message handlers
static void    OnClose(HWND hwnd);
static void    OnPaint(HWND hwnd);
static void    OnPaint2(HWND hwnd);
static void    OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void    OnCommand2(HWND,WORD,HWND);
static void    OnSize(HWND hwnd, UINT state, int cx, int cy);
static void    OnSize2(LPARAM);
static void    OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
// Menu handlers
static void    OnFileOpen(HWND hwnd);

// MFPlay event handler functions.
static void OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT *pEvent);
static void OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT *pEvent);

// Constants
static const WCHAR CLASS_NAME[]  = L"MFPlay Window Class";
static const WCHAR WINDOW_NAME[] = L"MFPlay Sample Application";
static const WCHAR CLASS_NAME2[]  = L"MFPlay Window Class2";
static const WCHAR WINDOW_NAME2[] = L"MFPlay Sample Application2";
static const UINT_PTR  IDT_TIMER1 = 1;     // Timer ID
static const UINT      TICK_FREQ = 999;    // Timer frequency in msec

//-------------------------------------------------------------------
//
// MediaPlayerCallback class
//
// Implements the callback interface for MFPlay events.
//
//-------------------------------------------------------------------

//#include <shlwapi.h>
typedef struct {
  const IID *piid;
#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)
  DWORD dwOffset;
#else
  int dwOffset;
#endif
} QITAB, *LPQITAB;
typedef const QITAB *LPCQITAB;
#ifdef __cplusplus
#define QITABENTMULTI(Cthis, Ifoo, Iimpl) { &__uuidof(Ifoo), OFFSETOFCLASS(Iimpl, Cthis) }
#else
#define QITABENTMULTI(Cthis, Ifoo, Iimpl) { (IID*) &IID_##Ifoo, OFFSETOFCLASS(Iimpl, Cthis) }
#endif
//#define QITABENTMULTI2(Cthis, Ifoo, Iimpl) { (IID*) &Ifoo, OFFSETOFCLASS(Iimpl, Cthis) }
#define QITABENT(Cthis, Ifoo) QITABENTMULTI(Cthis, Ifoo, Ifoo)
#ifndef OFFSETOFCLASS
#define OFFSETOFCLASS(base, derived) ((DWORD)(DWORD_PTR)(static_cast<base*>((derived*)8))-8)
#endif
  STDAPI QISearch(void *that, LPCQITAB pqit, REFIID riid, void **ppv);

class MediaPlayerCallback : public IMFPMediaPlayerCallback
{
    long m_cRef; // Reference count

public:

    MediaPlayerCallback() : m_cRef(1)
    {
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        static const QITAB qit[] =
        {
            QITABENT(MediaPlayerCallback, IMFPMediaPlayerCallback),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }
    STDMETHODIMP_(ULONG) AddRef()
    {
            return InterlockedIncrement(&m_cRef);
    }
    STDMETHODIMP_(ULONG) Release()
    {
        ULONG count = InterlockedDecrement(&m_cRef);
        if (count == 0)
        {
            delete this;
            return 0;
        }
        return count;
    }

    // IMFPMediaPlayerCallback methods
    void STDMETHODCALLTYPE OnMediaPlayerEvent(MFP_EVENT_HEADER *pEventHeader);
};


// Global variables

#define control_left 20
#define track_top 20
#define track_hg 30

#define bookmarks_top track_top+track_hg+10
#define bookmarks_hg track_hg

#define main_controls_top bookmarks_top+bookmarks_hg+10
#define main_controls_wd 110
#define main_controls_hg 30
#define IDC_PAUSE 1
#define IDC_STOP 2
#define IDC_BUTTON 3
#define bookmarks_limit 100
#define bookmarks_limit_first IDC_BUTTON
#define bookmarks_limit_adjusted IDC_BUTTON+bookmarks_limit
#define main_controls_separator 10

#define bookmark_button_left 80
#define bookmark_button_right bookmark_button_left+main_controls_wd
#define bookmark_button_right bookmark_button_left+main_controls_wd
#define pause_button_left bookmark_button_right+main_controls_separator
#define pause_button_right pause_button_left+main_controls_wd
#define stop_button_left pause_button_right+main_controls_separator

#define VAL_1X     -1
#define VAL_2X     VAL_1X,  VAL_1X
#define VAL_4X     VAL_2X,  VAL_2X
#define VAL_8X     VAL_4X,  VAL_4X
#define VAL_16X    VAL_8X,  VAL_8X
#define VAL_32X    VAL_16X, VAL_16X
#define VAL_64X    VAL_32X, VAL_32X

static IMFPMediaPlayer         *g_pPlayer = NULL;      // The MFPlay player object.
static MediaPlayerCallback     *g_pPlayerCB = NULL;    // Application callback object.

static BOOL                    g_bHasVideo = FALSE;

static UINT_PTR m_timerID=NULL;
static MFTIME bookmarks[bookmarks_limit_adjusted]={0, VAL_64X , VAL_32X , VAL_4X};//long long
static LPARAM bookmarks_graph[bookmarks_limit_adjusted];
static HWND bookmarks_control[bookmarks_limit_adjusted];

static HWND main_window;

/////////////////////////////////////////////////////////////////////

INT WINAPI wWinMain(HINSTANCE,HINSTANCE,LPWSTR,INT)
{
    (void)HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
    {
        return 0;
    }

    HWND hwnd;// = 0;

    if (!InitializeWindow(&hwnd))
    {
        return 0;
    }

	//HWND slider=Slider_Win();
	//if(slider!=NULL){
		MSG msg;// = {0};
		// Message loop
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if(!IsDialogMessage(hwnd, &msg)){//for tabs
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	//	DestroyWindow(slider);
	//}

    DestroyWindow(hwnd);
    DestroyWindow(main_window);
    CoUninitialize();

    return 0;
}

static HWND hTrack;
static HWND hlbl;
static HWND bookmark_button;
static HWND pause_button;
static HWND stop_button;
static bool OnCreate(HWND hwnd, LPCREATESTRUCT){
//    HWND hLeftLabel = CreateWindowW(L"Static", L"0",
  //      WS_CHILD | WS_VISIBLE, 0, 0, 10, 30, hwnd, 0, NULL, NULL);

    //HWND hRightLabel = CreateWindowW(L"Static", L"100",
      //  WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, hwnd, 0, NULL, NULL);

	if(hlbl = CreateWindow(L"Static", L"0", WS_CHILD | WS_VISIBLE,
		control_left, main_controls_top, 50, main_controls_hg, hwnd, (HMENU)2, NULL, NULL)){
		if(bookmark_button=CreateWindow(L"BUTTON", L"Bookmark", WS_DISABLED | WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, bookmark_button_left, main_controls_top, main_controls_wd, main_controls_hg, hwnd, (HMENU)IDC_BUTTON, NULL, NULL)){
			if(pause_button=CreateWindow(L"BUTTON", L"Pause/Resume", WS_DISABLED | WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, pause_button_left, main_controls_top, main_controls_wd, main_controls_hg, hwnd, (HMENU)IDC_PAUSE, NULL, NULL)){
				if(stop_button=CreateWindow(L"BUTTON", L"Stop", WS_DISABLED | WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, stop_button_left, main_controls_top, main_controls_wd, main_controls_hg, hwnd, (HMENU)IDC_STOP, NULL, NULL)){
					INITCOMMONCONTROLSEX icex;
					icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
					icex.dwICC  = ICC_LISTVIEW_CLASSES;
					if(InitCommonControlsEx(&icex)){
						if(hTrack = CreateWindow(TRACKBAR_CLASSW, L"",
						     WS_TABSTOP | WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
						     control_left, track_top, 170, track_hg, hwnd, 0, NULL, NULL))
						{
							SendMessage(hTrack, TBM_SETRANGE,  TRUE, MAKELONG(0, 170));
							SendMessage(hTrack, TBM_SETPAGESIZE, 0,  10);//mouse clicks
							SendMessage(hTrack, TBM_SETTICFREQ, 10, 0);//Sets the interval frequency for tick marks in a trackbar, displayed
							//SendMessage(hTrack, TBM_SETPOS, FALSE, 0);//comes already at 0
							//SendMessageW(hTrack, TBM_SETBUDDY, TRUE, (LPARAM) hLeftLabel);
							//SendMessageW(hTrack, TBM_SETBUDDY, FALSE, (LPARAM) hRightLabel);
						}
					}
				}
			}
		}
	}
return TRUE;
}
static HRESULT GetDuration(MFTIME *phnsDuration)
{
    HRESULT hr;// = E_FAIL;

    PROPVARIANT var;
    PropVariantInit(&var);

        hr = g_pPlayer->GetDuration(
            MFP_POSITIONTYPE_100NS,
            &var
            );

        if (SUCCEEDED(hr))
        {

            *phnsDuration = var.uhVal.QuadPart;
        }

    PropVariantClear(&var);
    return hr;
}
static HRESULT GetPosition(MFTIME*pposition){
	HRESULT hr;// = S_OK;

	PROPVARIANT var;
	PropVariantInit(&var);

	hr = g_pPlayer->GetPosition(MFP_POSITIONTYPE_100NS, &var);

	if (SUCCEEDED(hr))
	{
		*pposition=var.hVal.QuadPart;
	}

	PropVariantClear(&var);
	return hr;
}
static void SetPosition(MFTIME pos){
	PROPVARIANT var;
	PropVariantInit(&var);
	var.vt = VT_I8;
	var.hVal.QuadPart = pos;
	g_pPlayer->SetPosition(MFP_POSITIONTYPE_100NS,&var);//attention at pause
	PropVariantClear(&var);
}

static LRESULT printpos(){
    LRESULT pos = SendMessage(hTrack, TBM_GETPOS, 0, 0);
    wchar_t buf[6];
    StringCbPrintf(buf, sizeof(buf), L"%ld", pos);
    //wsprintfW(buf, L"%ld", pos);
    SetWindowTextW(hlbl, buf);
	return pos;
}

static void OnHScroll(HWND , HWND , UINT , int )
{
	LRESULT pos=printpos();

	if (g_pPlayer){
		MFTIME duration;
		HRESULT hr = GetDuration(&duration);
		if (FAILED(hr)) { return; }
		LRESULT max=SendMessage(hTrack,TBM_GETRANGEMAX,0,0);
		SetPosition(duration*pos/max);//is longlong*long/long, is not truncating, tested in asm

		//SetFocus(h1);//so space can work again, not focusing the tracker //but the solution is not with the focus mess, must be with a subclass or lowlevel hook
	}
}
static void OnTimer(){
	MFTIME pos;

	HRESULT hr = GetPosition(&pos);
	if (FAILED(hr)) { return; }

	MFTIME duration;
	hr = GetDuration(&duration);
	if (SUCCEEDED(hr)) {
		LRESULT max=SendMessage(hTrack,TBM_GETRANGEMAX,0,0);
		LONG t=pos*max/duration;//is long=longlong*long/longlong, is not truncating, tested in asm
		SendMessage(hTrack, TBM_SETPOS, TRUE, t);
		printpos();
	}
}

//-------------------------------------------------------------------
// WindowProc
//
// Main window procedure.
//-------------------------------------------------------------------

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_PAINT,   OnPaint);
	case WM_TIMER:          // Timer message
		OnTimer();
		return 0;
        HANDLE_MSG(hwnd, WM_KEYDOWN, OnKeyDown);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_SIZE,    OnSize);
        HANDLE_MSG(hwnd, WM_CLOSE,   OnClose);

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
static LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_HSCROLL, OnHScroll);
        HANDLE_MSG(hwnd, WM_KEYDOWN, OnKeyDown);
	case WM_COMMAND:
		OnCommand2(hwnd,LOWORD(wParam),(HWND)lParam);
		return 0;
    case WM_PAINT: //whithout, will not redraw when trackbar is resizing on wm_size
	OnPaint2(hwnd);
        return 0;
    case WM_SIZE://WM_SIZING nothing
	OnSize2(lParam);
	return 0;

        HANDLE_MSG(hwnd, WM_CREATE,  OnCreate);
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

//-------------------------------------------------------------------
// InitializeWindow
//
// Creates the main application window.
//-------------------------------------------------------------------

static BOOL InitializeWindow(HWND *pHwnd)
{
    WNDCLASS wc = {0};// if there are fewer initialisers in the list than there are members in the struct, then each member not explicitly initialised is default-initialised.
//there is member that needs default at register call

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = CLASS_NAME;
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    WNDCLASS wc2 = {0};

    wc2.lpfnWndProc   = WindowProc2;
    wc2.hInstance     = GetModuleHandle(NULL);
    wc2.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc2.lpszClassName = CLASS_NAME2;

    if (!RegisterClass(&wc2))
    {
        return FALSE;
    }

    main_window = CreateWindow(
        CLASS_NAME,
        WINDOW_NAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
        );

    if (!main_window)
    {
        return FALSE;
    }

    HWND hwnd = CreateWindowEx(WS_EX_CONTROLPARENT,//for tabs
        CLASS_NAME2,
        WINDOW_NAME2,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
        );
	if (!hwnd){
		DestroyWindow(main_window);return FALSE;
	}

    ShowWindow(main_window, SW_SHOWMAXIMIZED);//SW_SHOWDEFAULT
    UpdateWindow(main_window);
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);

    *pHwnd = hwnd;

    return TRUE;
}

//-------------------------------------------------------------------
// OnClose
//
// Handles the WM_CLOSE message.
//-------------------------------------------------------------------

static void close_playercb(){
	g_pPlayerCB->Release();
	g_pPlayerCB = NULL;
}
static void close_playertimer(HWND hwnd){
	KillTimer(hwnd, m_timerID);
	m_timerID=NULL;
}
static void close_previous(HWND hwnd){
	if (g_pPlayerCB)
	{
		close_playercb();
		if(m_timerID!=NULL){
			close_playertimer(hwnd);
			if (g_pPlayer)
			{
				g_pPlayer->Shutdown();
				g_pPlayer->Release();
				g_pPlayer = NULL;

				//and close bookmarks
				for(int i=bookmarks_limit_first;i<bookmarks_limit_adjusted;i++){
					if(bookmarks[i]!=-1)DestroyWindow(bookmarks_control[i]);
				}

				//and buttons to first state
				EnableWindow(bookmark_button,FALSE);
				EnableWindow(pause_button,FALSE);
				EnableWindow(stop_button,FALSE);
			}
		}
	}
}
static void OnClose(HWND hwnd)
{
	close_previous(hwnd);

    PostQuitMessage(0);
}


//-------------------------------------------------------------------
// OnPaint
//
// Handles the WM_PAINT message.
//-------------------------------------------------------------------

static void OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;// = 0;

    hdc = BeginPaint(hwnd, &ps);

    if (g_pPlayer && g_bHasVideo)
    {
        // Playback has started and there is video.

        // Do not draw the window background, because the video
        // frame fills the entire client area.

        g_pPlayer->UpdateVideo();
    }
    else
    {
        // There is no video stream, or playback has not started.
        // Paint the entire client area.

        FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
    }

    EndPaint(hwnd, &ps);
}
static void OnPaint2(HWND hwnd){
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	// All painting occurs here, between BeginPaint and EndPaint.
	FillRect(hdc, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
	EndPaint(hwnd, &ps);
}

//-------------------------------------------------------------------
// OnSize
//
// Handles the WM_SIZE message.
//-------------------------------------------------------------------

static void OnSize(HWND /*hwnd*/, UINT state, int /*cx*/, int /*cy*/)
{
    if (state == SIZE_RESTORED)
    {
        if (g_pPlayer)
        {
            // Resize the video.
            g_pPlayer->UpdateVideo();
        }
    }
}
static void OnSize2(LPARAM dims)
{
	int wd=dims&0x0000FFff;int hg=dims>>16;
	int newwd=wd-20-20;
	SendMessage(hTrack, TBM_SETRANGE,  TRUE, MAKELONG(0, newwd));
	SetWindowPos(hTrack,0,0,0,newwd,30,SWP_NOMOVE);
}


//-------------------------------------------------------------------
// OnKeyDown
//
// Handles the WM_KEYDOWN message.
//-------------------------------------------------------------------

// Toggle between playback and paused/stopped.
static void pause(){
	MFP_MEDIAPLAYER_STATE state;// = MFP_MEDIAPLAYER_STATE_EMPTY; //it's only [out]

	HRESULT hr = g_pPlayer->GetState(&state);

	if (SUCCEEDED(hr))
	{
	    if (state == MFP_MEDIAPLAYER_STATE_PAUSED || state == MFP_MEDIAPLAYER_STATE_STOPPED)
	    {
	        hr = g_pPlayer->Play();
	    }
	    else if (state == MFP_MEDIAPLAYER_STATE_PLAYING)
	    {
	        hr = g_pPlayer->Pause();
	    }
	}
    if (FAILED(hr))
    {
        ShowErrorMessage(TEXT("Playback Error"), hr);
    }
}
static void OnKeyDown(HWND /*hwnd*/, UINT vk, BOOL /*fDown*/, int /*cRepeat*/, UINT /*flags*/)
{
	if(vk==VK_SPACE){
        if (g_pPlayer)
        {
		pause();
        }
	}
}


//-------------------------------------------------------------------
// OnCommand
//
// Handles the WM_COMMAND message.
//-------------------------------------------------------------------

static void OnCommand(HWND hwnd, int id, HWND /*hwndCtl*/, UINT /*codeNotify*/)
{
    switch (id)
    {
        case ID_FILE_OPEN:
            OnFileOpen(hwnd);
            break;

        case ID_FILE_EXIT:
            OnClose(hwnd);
            //break;
    }
}

static void OnCommand2(HWND hwnd,WORD param,HWND control){
	//if (g_pPlayer){
		if(param==IDC_BUTTON){
			for(int i=bookmarks_limit_first;i<bookmarks_limit_adjusted;i++){
				if(bookmarks[i]==-1){
					RECT rect;
					SendMessage(hTrack,TBM_GETTHUMBRECT,0,(LPARAM)&rect);
					if(bookmarks_control[i]=CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, control_left+rect.left, bookmarks_top, rect.right-rect.left, bookmarks_hg, hwnd, (HMENU)(i+1), NULL, NULL)){
						GetPosition(&bookmarks[i]);
						bookmarks_graph[i]=SendMessage(hTrack, TBM_GETPOS, 0, 0);
					}
					return;
				}
			}
			return;
		}else if(param==IDC_PAUSE){
			pause();
			return;
		}else if(param==IDC_STOP){
			close_previous(main_window);
			return;
		}
		WORD pos=param-1;
		SetPosition(bookmarks[pos]);
		SendMessage(hTrack, TBM_SETPOS, TRUE, bookmarks_graph[pos]);
		DestroyWindow(control);
		bookmarks[pos]=-1;
	//}
}

//-------------------------------------------------------------------
// OnFileOpen
//
// Handles the "File Open" command.
//-------------------------------------------------------------------

static void OnFileOpen(HWND hwnd)
{
    const WCHAR *lpstrFilter =
        L"Media Files\0*.aac;*.asf;*.avi;*.m4a;*.mp3;*.mp4;*.wav;*.wma;*.wmv;*.3gp;*.3g2\0"
        L"All files\0*.*\0";

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    WCHAR szFileName[MAX_PATH];
    szFileName[0] = L'\0';

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;//m_hDlg;
    ofn.hInstance = GetModuleHandle(NULL);
    ofn.lpstrFilter = lpstrFilter;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
    {
	// Open the media file.
	HRESULT hr;// = S_OK;
	hr = PlayMediaFile(hwnd, szFileName);

	if (FAILED(hr))
	{
		ShowErrorMessage(L"Could not open file.", hr);
	}
    }
    else
    {
        // GetOpenFileName can return FALSE because the user cancelled,
        // or because it failed. Check for errors.

        DWORD err = CommDlgExtendedError();
        if (err != 0)
        {
		ShowErrorMessage(L"GetOpenFileName failed.", err);
        }
    }
}


//-------------------------------------------------------------------
// PlayMediaFile
//
// Plays a media file, using the IMFPMediaPlayer interface.
//-------------------------------------------------------------------

static HRESULT PlayMediaFile(HWND hwnd, const WCHAR *sURL)
{
    HRESULT hr;// = S_OK;

    // Create the MFPlayer object.
    if (g_pPlayer != NULL)close_previous(hwnd);

        g_pPlayerCB = new (std::nothrow) MediaPlayerCallback();

        if (g_pPlayerCB == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto done;
        }

	m_timerID=SetTimer(hwnd, IDT_TIMER1, TICK_FREQ, NULL);
	if(m_timerID==NULL){//is _PTR
		close_playercb();
		hr = E_OUTOFMEMORY;goto done;
	}

        hr = MFPCreateMediaPlayer(
            NULL,
            FALSE,          // Start playback automatically?
            0,              // Flags
            g_pPlayerCB,    // Callback pointer
            hwnd,           // Video window
            &g_pPlayer
            );

	if (FAILED(hr)) {
		close_playercb();
		close_playertimer(hwnd);
		goto done;
	}

	EnableWindow(bookmark_button,TRUE);
	EnableWindow(pause_button,TRUE);
	EnableWindow(stop_button,TRUE);

    // Create a new media item for this URL.
    hr = g_pPlayer->CreateMediaItemFromURL(sURL, FALSE, 0, NULL);

    // The CreateMediaItemFromURL method completes asynchronously.
    // The application will receive an MFP_EVENT_TYPE_MEDIAITEM_CREATED
    // event. See MediaPlayerCallback::OnMediaPlayerEvent().

done:
    return hr;
}


//-------------------------------------------------------------------
// OnMediaPlayerEvent
//
// Implements IMFPMediaPlayerCallback::OnMediaPlayerEvent.
// This callback method handles events from the MFPlay object.
//-------------------------------------------------------------------

void MediaPlayerCallback::OnMediaPlayerEvent(MFP_EVENT_HEADER * pEventHeader)
{
    if (FAILED(pEventHeader->hrEvent))
    {
        ShowErrorMessage(L"Playback error", pEventHeader->hrEvent);
        return;
    }

    switch (pEventHeader->eEventType)
    {
    case MFP_EVENT_TYPE_MEDIAITEM_CREATED:
        OnMediaItemCreated(MFP_GET_MEDIAITEM_CREATED_EVENT(pEventHeader));
        break;

    case MFP_EVENT_TYPE_MEDIAITEM_SET:
        OnMediaItemSet(MFP_GET_MEDIAITEM_SET_EVENT(pEventHeader));
        //break;
    }
}


//-------------------------------------------------------------------
// OnMediaItemCreated
//
// Called when the IMFPMediaPlayer::CreateMediaItemFromURL method
// completes.
//-------------------------------------------------------------------

static void OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT *pEvent)
{
    HRESULT hr;// = S_OK;

    // The media item was created successfully.

    if (g_pPlayer) //this check: CreateMediaItemFromURL, async(return there), some new dll loads and threads, can be OnClose
    {
        BOOL bHasVideo = FALSE, bIsSelected = FALSE;

        // Check if the media item contains video.
        hr = pEvent->pMediaItem->HasVideo(&bHasVideo, &bIsSelected);

        if (FAILED(hr)) { goto done; }

        g_bHasVideo = bHasVideo && bIsSelected;

        // Set the media item on the player. This method completes asynchronously.
        hr = g_pPlayer->SetMediaItem(pEvent->pMediaItem);
    }

done:
    if (FAILED(hr))
    {
        ShowErrorMessage(L"Error playing this file.", hr);
    }
}


//-------------------------------------------------------------------
// OnMediaItemSet
//
// Called when the IMFPMediaPlayer::SetMediaItem method completes.
//-------------------------------------------------------------------

static void OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT * /*pEvent*/)
{
    HRESULT hr;// = S_OK;

    hr = g_pPlayer->Play();

    if (FAILED(hr))
    {
        ShowErrorMessage(L"IMFPMediaPlayer::Play failed.", hr);
    }
}


static void ShowErrorMessage(PCWSTR format, HRESULT hrErr)
{
    HRESULT hr;// = S_OK;
    WCHAR msg[MAX_PATH];

    hr = StringCbPrintf(msg, sizeof(msg), L"%s (hr=0x%X)", format, hrErr);

    if (SUCCEEDED(hr))
    {
        MessageBox(NULL, msg, L"Error", MB_ICONERROR);
    }
}

//wchar_t buf[256];
//FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//               NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//               buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
//printf("%S\n",buf);
