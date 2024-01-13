


;;
exeevent rd 1
dllevent rd 1
totalframes rd 1
INFINITE dd 0xFFFFFFFF
;;

include "compdata.inc"

file_out rd 1
f_bool rb 1
temp_out rd 1
delay rb 1
stop_key rb 1
fps rb 1
left rd 1
right rd 1
top rd 1
bottom rd 1
FOURCC_identifier rd 1

;;;;codec
stringpointerpointer rd 1
query_return rd 1
;;;;

file_handler rd 1
temp_file_handler rd 1
errno rd 1
storevalue dd 0
warnvalue dd 0

_O_WRONLY dd 0x0001
_O_RDWR dd 0x0002
_O_TEMPORARY dd 0x0040
_O_CREAT dd 0x0100
_O_TRUNC dd 0x0200
_O_EXCL dd 0x0400
_O_BINARY dd 0x8000
;_O_U8TEXT dd 0x40000

_SH_DENYNO dd 0x00000040
_S_IWRITE dd 0x00000080
EEXIST dd 17

WH_KEYBOARD_LL_changed dd 13
;in 2023 now, then was ~2011, symbol already defined, there is in INCLUDE/EQUATES/USER32.INC and in 64, if ; then operand size not specified, so changed WH_KEYBOARD_LL

;;;;size
sw rb 1
marker rd 1
;;;;

;;;;;;;;;;;;;1
;BITMAPINFO
    ; BITMAPINFOHEADER
    bmih.biSize dd 40
    bmih.biWidth rd 1
    bmih.biHeight rd 1
    bmih.biPlanes dw 1
    bmih.biBitCount dw 24
    bmih.biCompression dd BI_RGB
    bmih.biSizeImage rd 1
    bmih.biXPelsPerMeter dd 0
    bmih.biYPelsPerMeter dd 0
    bmih.biClrUsed dd 0
    bmih.biClrImportant dd 0
    ; RGBQUAD          bmiColors[1]; ignored it isn't necessary for 24 bits-per-pixel
    bmiColors.rgbBlue db 0
    bmiColors.rgbGreen db 0
    bmiColors.rgbRed db 0
    bmiColors.rgbReserved db 0


pvBits1 rd 1
pvBits2 rd 1

hbm1 rd 1
hbm2 rd 1

srcDC rd 1
destDC rd 1
DIB_RGB_COLORS dd 0

mspersecond dw 1000

SND_SYNC dd 0
sound db "SystemDefault",0

hookhandle rd 1
;;;;;;;;;;;;;;;1

;;;;;;;;;;;2
switch rb 1
selectedbuffer rb 1
nextsecond rd 1
seconds rd 1
max_address rd 1
framesinsec rb 1
lastbuffer rd 1


msg.hwnd rd 1
msg.message rd 1
msg.wParam rd 1
msg.lParam rd 1
msg.time rd 1
msg.pt_x rd 1
msg.pt_y rd 1
;;;;;;;;;;;;;;;;2

;;;;;;;;;;;;;;;;;3
SEEK_SET dd 0
SEEK_CUR dd 1
SEEK_END dd 2


include 'mkv.inc'
include 'mkv2.inc'

;;;;;;;;;;;;;;;;;3


;_stat64
_stat64.st_dev rd 1
_stat64.st_ino rw 1
_stat64.st_mode rw 1;unsigned short
_stat64.st_nlink rw 1;short
_stat64.st_uid rw 1;short
_stat64.st_gid rw 1;short
	       rw 1;;for data structure alignment
_stat64.st_rdev rd 1
		rd 1
_stat64.st_size rq 1;__int64
_stat64.st_atime rq 1;__time64_t
_stat64.st_mtime rq 1;__time64_t
_stat64.st_ctime rq 1;__time64_t

tempsecret db "Software\OApplicationsSRecorder",0

quotientnr dd 4

