



library srecorder,'srecorder.dll',\
	kernel,'kernel32.dll',\
	msvcr100,'msvcr100.dll'

import srecorder,\
       capture,'capture',\
       query,'query'

import kernel,\
       ExitProcess,'ExitProcess',\
       GetCommandLine,'GetCommandLineA',\
       WaitForSingleObject,"WaitForSingleObject",\
       CreateEvent,"CreateEventA",\
       CreateThread,"CreateThread",\
       SetEvent,"SetEvent",\
       GetStdHandle,"GetStdHandle",\
       GetConsoleScreenBufferInfo,"GetConsoleScreenBufferInfo",\
       SetConsoleCursorPosition,"SetConsoleCursorPosition",\
       ResetEvent,"ResetEvent"

import msvcr100,\
       printf,'printf',\
       cgets,'_cgets_s',\
       strcmp,'strcmp',\
       atoi,'atoi',\
       time,"_time64",\
       sprintf,'sprintf',\
       strcat,"strcat"
