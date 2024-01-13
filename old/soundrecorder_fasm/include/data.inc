





help db "Hit 'Esc' to exit or other key to start. After that, press any key to stop the record.",0
time_t rd 2
lli_conv db "%lli",0
file_out_name rd 4
file_handler rd 1
extension db ".wav",0

_O_WRONLY dd 0x0001
_O_CREAT dd 0x0100
_O_EXCL dd 0x0400
_O_BINARY dd 0x8000
;_O_U8TEXT dd 0x40000

_SH_DENYNO dd 0x00000040
_S_IWRITE dd 0x00000080

;;;;;;;;;;;;;;;;;;;;
RIFF db "RIFF"
riffsize dd 38
	WAVE db "WAVE"
	fmt db "fmt "
	fmtsize dd 0x12
		wFormatTag dw 1;WAVE_FORMAT_PCM
		nChannels dw 1
		nSamplesPerSec dd 11025
		nAvgBytesPerSec dd 22050;nBlockAlign * nSamplesPerSec
		nBlockAlign dw 2;nChannels * wBitsPerSample / 8
		wBitsPerSample dw 16
		cbSize dw 0;no extra data, simple PCM-format used
	rawdata db "data"
	rawdatasize dd 0





;WAVEINCAPS
wMid rw 1
wPid rw 1
vDriverVersion rd 1;MMVERSION
szPname rb 32;MAXPNAMELEN
dwFormats rd 1
wChannels rw 1
wReserved1 rw 1

WAVE_FORMAT_4S16 dd 0x800

hwi rd 1


lpData1 rd 1
dwBufferLength1 dd (2*22050)
dwBytesRecorded1 dd 0
dwUser1 dd 0
dwFlags1 dd 0
dwLoops1 dd 0
lpNext1 dd 0
reserved1 dd 0

lpData2 rd 1
dwBufferLength2 dd (2*22050)
dwBytesRecorded2 dd 0
dwUser2 dd 0
dwFlags2 dd 0
dwLoops2 dd 0
lpNext2 dd 0
reserved2 dd 0

flag db 0



SEEK_SET dd 0
SEEK_END dd 2


recording db 0xa,"Recording...",0

err1 db 0xa,"Wave input format not supported",0
err2 db 0xa,"Cannot open output file",0
err3 db 0xa,"Cannot write to output file",0
err4 db 0xa,"Cannot open the wave in device",0
err5 db 0xa,"Cannot allocate memory",0
err6 db 0xa,"Cannot prepare a header",0
err7 db 0xa,"Cannot add a buffer",0
err8 db 0xa,"Cannot create a thread",0
