
#define WINVER _WIN32_WINNT_WIN7

#include <new>
#include <windows.h>
#include <windowsx.h>  //HANDLE_MSG
#include <mfplay.h>
//#include <mferror.h>
//#include <shobjidl.h>   // defines IFileOpenDialog
#include <strsafe.h> //StringCbPrintf

#include "resource.h"

//#include "Slider.h"
#include <commctrl.h>

BOOL    InitializeWindow(HWND *pHwnd,HWND *pHwnd2);
HRESULT PlayMediaFile(HWND hwnd, const WCHAR *sURL);
void    ShowErrorMessage(PCWSTR format, HRESULT hr);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Window message handlers
void    OnClose(HWND hwnd);
void    OnPaint(HWND hwnd);
void    OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void    OnSize(HWND hwnd, UINT state, int cx, int cy);
void    OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);

// Menu handlers
void    OnFileOpen(HWND hwnd);

// MFPlay event handler functions.
void OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT *pEvent);
void OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT *pEvent);

// Constants
const WCHAR CLASS_NAME[]  = L"MFPlay Window Class";
const WCHAR WINDOW_NAME[] = L"MFPlay Sample Application";
const WCHAR CLASS_NAME2[]  = L"MFPlay Window Class2";
const WCHAR WINDOW_NAME2[] = L"MFPlay Sample Application2";
const UINT_PTR  IDT_TIMER1 = 1;     // Timer ID
const UINT      TICK_FREQ = 999;    // Timer frequency in msec

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

IMFPMediaPlayer         *g_pPlayer = NULL;      // The MFPlay player object.
MediaPlayerCallback     *g_pPlayerCB = NULL;    // Application callback object.

BOOL                    g_bHasVideo = FALSE;

UINT_PTR m_timerID=NULL;

/////////////////////////////////////////////////////////////////////

INT WINAPI wWinMain(HINSTANCE,HINSTANCE,LPWSTR,INT)
{
    (void)HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
    {
        return 0;
    }

    HWND hwnd;// = 0;
    HWND hwnd2;

    if (!InitializeWindow(&hwnd,&hwnd2))
    {
        return 0;
    }

	//HWND slider=Slider_Win();
	//if(slider!=NULL){
		MSG msg;// = {0};
		// Message loop
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	//	DestroyWindow(slider);
	//}

    DestroyWindow(hwnd2);
    DestroyWindow(hwnd);
    CoUninitialize();

    return 0;
}

static HWND hTrack;
static HWND hlbl;
bool OnCreate(HWND hwnd, LPCREATESTRUCT){
//    HWND hLeftLabel = CreateWindowW(L"Static", L"0",
  //      WS_CHILD | WS_VISIBLE, 0, 0, 10, 30, hwnd, (HMENU)1, NULL, NULL);

    //HWND hRightLabel = CreateWindowW(L"Static", L"100",
      //  WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, hwnd, (HMENU)2, NULL, NULL);

	if(hlbl = CreateWindowW(L"Static", L"0", WS_CHILD | WS_VISIBLE,
		20, 60, 30, 30, hwnd, (HMENU)1, NULL, NULL)){
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC  = ICC_LISTVIEW_CLASSES;
		if(InitCommonControlsEx(&icex)){
			if(hTrack = CreateWindowW(TRACKBAR_CLASSW, L"", 
			     WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
			     20, 20, 170, 30, hwnd, (HMENU)2, NULL, NULL))
			{
				SendMessageW(hTrack, TBM_SETRANGE,  TRUE, MAKELONG(0, 100));
				SendMessageW(hTrack, TBM_SETPAGESIZE, 0,  10);
				SendMessageW(hTrack, TBM_SETTICFREQ, 10, 0);
				SendMessageW(hTrack, TBM_SETPOS, FALSE, 0);
				//SendMessageW(hTrack, TBM_SETBUDDY, TRUE, (LPARAM) hLeftLabel);
				//SendMessageW(hTrack, TBM_SETBUDDY, FALSE, (LPARAM) hRightLabel);
			}
		}
	}
return TRUE;
}
HRESULT GetDuration(MFTIME *phnsDuration)

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

LRESULT printpos(){
    LRESULT pos = SendMessageW(hTrack, TBM_GETPOS, 0, 0);
    wchar_t buf[4];
    StringCbPrintf(buf, sizeof(buf), L"%ld", pos);
    //wsprintfW(buf, L"%ld", pos);
    SetWindowTextW(hlbl, buf);
	return pos;
}

void OnHScroll(HWND h1, HWND , UINT , int )
{
	LRESULT pos=printpos();

	if (g_pPlayer){
		PROPVARIANT var;
		PropVariantInit(&var);
		var.vt = VT_I8;
		MFTIME duration;
		HRESULT hr = GetDuration(&duration);
		if (FAILED(hr)) { return; }

		var.hVal.QuadPart = duration*pos/100;

		g_pPlayer->SetPosition(MFP_POSITIONTYPE_100NS,&var);//attention at pause
		PropVariantClear(&var);

		SetFocus(h1);//so space can work again, not focusing the tracker
	}
}
BOOL OnSize2(LPARAM dims)
{
	int wd=dims&0x0000FFff;int hg=dims>>16;
	SetWindowPos(hTrack,0,0,0,wd-20-20,30,SWP_NOMOVE);
	return TRUE;
}
void OnTimer(){
	HRESULT hr;// = S_OK;

	PROPVARIANT var;
	PropVariantInit(&var);

	hr = g_pPlayer->GetPosition(MFP_POSITIONTYPE_100NS, &var);

	if (SUCCEEDED(hr))
	{
		MFTIME duration;
		HRESULT hr = GetDuration(&duration);
		if (SUCCEEDED(hr)) {
			LONG t=100*var.hVal.QuadPart/duration;
			SendMessageW(hTrack, TBM_SETPOS, TRUE, t);
			printpos();
		}
	}

	PropVariantClear(&var);
}
//-------------------------------------------------------------------
// WindowProc
//
// Main window procedure.
//-------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_PAINT,   OnPaint);
	case WM_TIMER:          // Timer message
		OnTimer();
		return 0;
        HANDLE_MSG(hwnd, WM_KEYDOWN, OnKeyDown);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
    case WM_ERASEBKGND:
        return 1;
        HANDLE_MSG(hwnd, WM_SIZE,    OnSize);
        HANDLE_MSG(hwnd, WM_CLOSE,   OnClose);

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
LRESULT CALLBACK WindowProc2(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_HSCROLL, OnHScroll);
        HANDLE_MSG(hwnd, WM_KEYDOWN, OnKeyDown);

    case WM_SIZE://WM_SIZING nothing
        return OnSize2(lParam);
    case WM_ERASEBKGND:
        return 1;

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

BOOL InitializeWindow(HWND *pHwnd,HWND *pHwnd2)
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

    HWND hwnd = CreateWindow(
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

    if (!hwnd)
    {
        return FALSE;
    }

    HWND hwnd2 = CreateWindow(
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
	if (!hwnd2){
		DestroyWindow(hwnd);return FALSE;
	}

    ShowWindow(hwnd, SW_SHOWMAXIMIZED);//SW_SHOWDEFAULT
    UpdateWindow(hwnd);
    ShowWindow(hwnd2, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd2);

    *pHwnd = hwnd;
    *pHwnd2 = hwnd2;

    return TRUE;
}

//-------------------------------------------------------------------
// OnClose
//
// Handles the WM_CLOSE message.
//-------------------------------------------------------------------

void close_previous(HWND hwnd){
	if (g_pPlayerCB)
	{
		g_pPlayerCB->Release();
		g_pPlayerCB = NULL;
		if (g_pPlayer)
		{
			g_pPlayer->Shutdown();
			g_pPlayer->Release();
			g_pPlayer = NULL;
			if(m_timerID!=NULL){
				KillTimer(hwnd, m_timerID);
				m_timerID=NULL;
			}
		}
	}
}
void OnClose(HWND hwnd)
{
	close_previous(hwnd);

    PostQuitMessage(0);
}


//-------------------------------------------------------------------
// OnPaint
//
// Handles the WM_PAINT message.
//-------------------------------------------------------------------

void OnPaint(HWND hwnd)
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


//-------------------------------------------------------------------
// OnSize
//
// Handles the WM_SIZE message.
//-------------------------------------------------------------------

void OnSize(HWND /*hwnd*/, UINT state, int /*cx*/, int /*cy*/)
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


//-------------------------------------------------------------------
// OnKeyDown
//
// Handles the WM_KEYDOWN message.
//-------------------------------------------------------------------

void OnKeyDown(HWND /*hwnd*/, UINT vk, BOOL /*fDown*/, int /*cRepeat*/, UINT /*flags*/)
{
    HRESULT hr = S_OK;

    switch (vk)
    {
    case VK_SPACE:

        // Toggle between playback and paused/stopped.
        if (g_pPlayer)
        {
            MFP_MEDIAPLAYER_STATE state;// = MFP_MEDIAPLAYER_STATE_EMPTY; //it's only [out]

            hr = g_pPlayer->GetState(&state);

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
        }
        //break;
    }

    if (FAILED(hr))
    {
        ShowErrorMessage(TEXT("Playback Error"), hr);
    }
}


//-------------------------------------------------------------------
// OnCommand
//
// Handles the WM_COMMAND message.
//-------------------------------------------------------------------

void OnCommand(HWND hwnd, int id, HWND /*hwndCtl*/, UINT /*codeNotify*/)
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


//-------------------------------------------------------------------
// OnFileOpen
//
// Handles the "File Open" command.
//-------------------------------------------------------------------

void OnFileOpen(HWND hwnd)
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

HRESULT PlayMediaFile(HWND hwnd, const WCHAR *sURL)
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

        hr = MFPCreateMediaPlayer(
            NULL,
            FALSE,          // Start playback automatically?
            0,              // Flags
            g_pPlayerCB,    // Callback pointer
            hwnd,           // Video window
            &g_pPlayer
            );

        if (FAILED(hr)) { goto done; }

	m_timerID=SetTimer(hwnd, IDT_TIMER1, TICK_FREQ, NULL);

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

void OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT *pEvent)
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

void OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT * /*pEvent*/)
{
    HRESULT hr;// = S_OK;

    hr = g_pPlayer->Play();

    if (FAILED(hr))
    {
        ShowErrorMessage(L"IMFPMediaPlayer::Play failed.", hr);
    }
}


void ShowErrorMessage(PCWSTR format, HRESULT hrErr)
{
    HRESULT hr;// = S_OK;
    WCHAR msg[MAX_PATH];

    hr = StringCbPrintf(msg, sizeof(msg), L"%s (hr=0x%X)", format, hrErr);

    if (SUCCEEDED(hr))
    {
        MessageBox(NULL, msg, L"Error", MB_ICONERROR);
    }
}
