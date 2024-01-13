




help db "The command line is: ss_recorder [Method]"
	db 0x0A,0x0A,"Method:"
		db 0x0A,0x09,"['h' or '-h' or '--help'] is this section"
		db 0x0A,0x09,"['c' or '-c' or '--codec'] for codec detection"
		db 0x0A,0x09,"['r' or '-r' or '--record'] [Optional arguments]"
	db 0x0A,0x0A,"Optional arguments:"
	db 0x0A,"general:"
		db 0x0A,0x09,"-output_file (default, time since 1 Jan 1970)"
		db 0x0A,0x09,"-temporary_file (default, same, +1 sec if both)"
		db 0x0A,0x09,"-delay (a number from 0 to 9, default 6)"
		db 0x0A,0x09,"-stop_key (alphanumeric key only, default 'S')"
	db 0x0A,"video:"
		db 0x0A,0x09,"-fps (a number from 1 to 32, default 25)"
		db 0x0A,0x09,"-left (32-9000, lower than right, default 0)"
		db 0x0A,0x09,"-right (32-9000, greater than left, default 1024)"
		db 0x0A,0x09,"-top (32-9000, lower than bottom, default 0)"
		db 0x0A,0x09,"-bottom (32-9000, greater than top, default 768)"
		db 0x0A,0x09,"-FOURCC_identifier (uncompressed if skipped)",0
unrecognized1 db "Unrecognized method: '%s'",0
unrecognized2 db "Unrecognized optional argument name: '%s'",0
unrecognized3 db "Unrecognized optional argument value: '%s'",0
unrecognized4 db "Missing optional argument value",0
bad_argument db "Unrecognized argument: '%s'",0
few db "Too few arguments",0
many db "Too many arguments",0
longinput db "Output path too long",0
codec db 0xA,"Codec",0xA,"---------",0xA,0
nocodec db "No codec detected",0
str1 db "h",0
str2 db "-h",0
str3 db "--help",0
str4 db "c",0
str5 db "-c",0
str6 db "--codec",0
str7 db "r",0
str8 db "-r",0
str9 db "--record",0
rva1 dd str1
rva2 dd str2
rva3 dd str3
rva4 dd str4
rva5 dd str5
rva6 dd str6
rva7 dd str7
rva8 dd str8
rva9 dd str9
opt1  db 1,"output_file",0
opt2  db 2,"temporary_file",0
opt3  db 3,"delay",0
opt4  db 4,"stop_key",0
opt5  db 5,"fps",0
opt6  db 6,"left",0
opt7  db 7,"right",0
opt8  db 8,"top",0
opt9  db 9,"bottom",0
opt10 db 10,"FOURCC_identifier",0
orva1  dd opt1+1
orva2  dd opt2+1
orva3  dd opt3+1
orva4  dd opt4+1
orva5  dd opt5+1
orva6  dd opt6+1
orva7  dd opt7+1
orva8  dd opt8+1
orva9  dd opt9+1
orva10 dd opt10+1

counter rb 1
pointer rd 1

;;files
time_t rd 2
timestring rb 15
	db 0
lli_conv db "%lli",0
files_flag db 0      ;0 none, 1 file, 2 temp, 3 both
extension db ".mkv",0
;;files

;;;;;;;;;;;;file names strings
file_out_name rb MAX_PATH
temp_file_out_name rb MAX_PATH


;;;the main structure for dll
file_out_name_pointer dd file_out_name
overwrite_file_out db 0    ;bool for overwrite file out, 0 is no overwrite, 1 is overwrite
temp_file_out_name_pointer dd temp_file_out_name
delay db 6		   ;BYTE
stop_key db 0x53	   ;Virtual-Key Code
fps db 25		   ;BYTE
left dw 0		   ;WORD
right dw 1024		   ;WORD
top dw 0		   ;WORD
bottom dw 768		   ;WORD
FOURCC_identifier dd 0	   ;DWORD

;;;;;;;;codec
stringpointer rd 1
querycodec rd 1
	db 0
;;;;;;;;

;;
exeeventstr db "incomingsignal_srecorder",0
dlleventstr db "outgoingsignal_srecorder",0
exeevent rd 1
dllevent rd 1
endflag db 0
infostr db 0xa,0xa,"Writing the mkv file...",0xa,"Completed: ",0
totalframes rd 1
frame dd 0
printprecentage db '%u%%',0
INFINITE dd 0xFFFFFFFF
hconsole rd 1
coord.X dw 11
coord.Y rw 1

;CONSOLE_SCREEN_BUFFER_INFO
  ;COORD dwSize
	csbi.dwSize.X rw 1
	csbi.dwSize.Y rw 1
  ;COORD dwCursorPosition
	csbi.dwCursorPosition.X rw 1
	csbi.dwCursorPosition.Y rw 1

	csbi.wAttributes rw 1

  ;SMALL_RECT srWindow
	csbi.Left rw 1
	csbi.Top rw 1
	csbi.Right rw 1
	csbi.Bottom rw 1

  ;COORD dwMaximumWindowSize
	csbi.dwMaximumWindowSize.X rw 1
	csbi.dwMaximumWindowSize.Y rw 1
;;

already db 0xa,0xa,"Output file already exist. Overwrite? (Yes) ",0
overstr rb 4
size_t dd 3
yes db "Yes",0

count db 0xa,"The capture delay is %i seconds. The stop key is '%c'.",0

open_output db 0xa,0xa,"Cannot open output file.",0
stop_err db 0xa,0xa,"The stop key isn't alpha-numeric.",0
fps_err db 0xa,0xa,"The fps value must be from 1 to 32.",0
fourcc_err db 0xa,0xa,"Cannot open the codec.",0
temp_file_err db 0xa,0xa,"Cannot open the temporary file.",0
write_err db 0xa,0xa,"Cannot write to output file.",0
allocation_err db 0xa,0xa,"Cannot allocate memory.",0
coord_err db 0xa,0xa,"Wrong coordinates. Left must be lower than right and top than bottom. The width must be a multiple of 4. The height must be a multiple of 2. The width and height must be greater or equal with 32 and must not be greater than 9000.",0
temp_write_err db 0xa,0xa,"Cannot write to temporary file.",0
compatible_dc_err db 0xa,0xa,"Cannot create a device context.",0
dib_err db 0xa,0xa,"Cannot create a dib section.",0
hook_err db 0xa,0xa,"Cannot add a windows hook.",0
framestart_err db 0xa,0xa,"Cannot initialize the compressor.",0

event_err db 0xa,0xa,"Cannot create an event.",0
thread_err db 0xa,0xa,"Cannot create a thread.",0

completed db "100%%",0