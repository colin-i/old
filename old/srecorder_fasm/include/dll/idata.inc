




library kernel,'kernel32.dll',\
	msvcr100,'msvcr100.dll',\
	user32,'user32.dll',\
	gdi32,'gdi32.dll',\
	msvfw,'Msvfw32.dll',\
	Advapi32,"Advapi32.dll",\
	Winmm,"Winmm.dll"

import kernel,\
       Sleep,"Sleep",\
       GetTickCount,"GetTickCount",\
       SetEvent,"SetEvent",\
       WaitForSingleObject,"WaitForSingleObject",\
       ResetEvent,"ResetEvent"

import msvcr100,\
	sopen_s,'_sopen_s',\
	sprintf,'sprintf',\
	malloc,"malloc",\
	seek,"_lseeki64",\
	write,"_write",\
	read,"_read",\
	close,"_close",\
	free,"free",\
	stat,"_stat64",\
	lldiv,"lldiv",\
	get_errno,'_get_errno'

import user32,\
       SetWindowsHookEx,"SetWindowsHookExA",\
       UnhookWindowsHookEx,"UnhookWindowsHookEx",\
       PeekMessage,"PeekMessageA",\
       CallNextHookEx,"CallNextHookEx",\
       GetWindowDC,"GetWindowDC"

import gdi32,\
       CreateCompatibleDC,"CreateCompatibleDC",\
       CreateDIBSection,"CreateDIBSection",\
       SelectObject,"SelectObject",\
       BitBlt,"BitBlt",\
       DeleteDC,"DeleteDC",\
       DeleteObject,"DeleteObject"

import msvfw,\
	ICOpen,'ICOpen',\
	ICClose,'ICClose',\
	ICSeqCompressFrameStart,"ICSeqCompressFrameStart",\
	ICSeqCompressFrameEnd,"ICSeqCompressFrameEnd",\
	ICCompressorFree,"ICCompressorFree",\
	ICSendMessage,"ICSendMessage",\
	ICSeqCompressFrame,"ICSeqCompressFrame"

import Advapi32,\
	RegOpenKeyEx,"RegOpenKeyExA",\
	RegGetValue,"RegGetValueA",\
	RegSetValueEx,"RegSetValueExA",\
	RegCloseKey,"RegCloseKey",\
	RegDeleteValue,"RegDeleteValueA"

import Winmm,\
	PlaySound,"PlaySound"