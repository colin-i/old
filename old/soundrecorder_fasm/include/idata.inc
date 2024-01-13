



library kernel,'kernel32.dll',\
	msvcr100,'msvcr100.dll',\
	winmm,'Winmm.dll'

import kernel,\
	ExitProcess,'ExitProcess',\
	CreateThread,"CreateThread",\
	Sleep,"Sleep",\
	GetTickCount,"GetTickCount"

import msvcr100,\
	printf,'printf',\
	getch,'_getch',\
	time,"_time64",\
	sprintf,'sprintf',\
	sopen_s,'_sopen_s',\
	write,"_write",\
	strcat,"strcat",\
	malloc,"malloc",\
	lseek,"_lseek"

import winmm,\
	waveInGetDevCaps,"waveInGetDevCapsA",\
	waveInOpen,"waveInOpen",\
	waveInPrepareHeader,"waveInPrepareHeader",\
	waveInAddBuffer,"waveInAddBuffer",\
	waveInStart,"waveInStart",\
	waveInStop,"waveInStop"