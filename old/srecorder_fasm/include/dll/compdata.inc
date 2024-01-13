




;capture9
ICMODE_COMPRESS dd 1
ICMODE_QUERY dd 4
ICM_COMPRESS_GET_FORMAT dd (0x4000+0x0000+4);DRV_USER + ICM_USER + 4

;typedef struct {
	ic.cbSize dd 64
	ic.dwFlags dd 0x00000001;ICMF_COMPVARS_VALID
	ic.hic rd 1
	ic.fccType dd 'vidc'
	ic.fccHandler dd 'xvid'
	ic.lpbiIn dd NULL
	ic.lpbiOut dd NULL
	ic.lpBitsOut dd NULL
	ic.lpBitsPrev dd NULL
	ic.lFrame dd NULL
	ic.lKey dd NULL     ;LONG
	ic.lDataRate dd NULL;LONG
	ic.lQ dd 10000	    ;1 to 10,000
	ic.lKeyCount dd NULL;LONG
	ic.lpState dd NULL  ;LPVOID
	ic.cbState dd NULL  ;LONG
;} COMPVARS;

hkey rd 1
keypath db "Software\GNU\XviD",0
regstore rd 1
reg_dword dd 4

num_zones db "num_zones",0
store_num rd 1

regcounter rb 1
valuestorepointer rd 1
datastorepointer rd 1

valuestorestring1 db "zone%u_bvop_threshold",0
valuestorestring2 db "zone%u_cartoon_mode",0
valuestorestring3 db "zone%u_chroma_opt",0
valuestorestring4 db "zone%u_frame",0
valuestorestring5 db "zone%u_greyscale",0
valuestorestring6 db "zone%u_mode",0
valuestorestring7 db "zone%u_quant",0
valuestorestring8 db "zone%u_type",0
valuestorestring9 db "zone%u_weight",0
valuestore rb 32
rvastr1 dd valuestorestring1
rvastr2 dd valuestorestring2
rvastr3 dd valuestorestring3
rvastr4 dd valuestorestring4
rvastr5 dd valuestorestring5
rvastr6 dd valuestorestring6
rvastr7 dd valuestorestring7
rvastr8 dd valuestorestring8
rvastr9 dd valuestorestring9
datastore1 dd 0
datastore2 dd 0
datastore3 dd 0
datastore4 rd 1
datastore5 dd 0
datastore6 dd 0
datastore7 dd 0x1f4
datastore8 dd 1
datastore9 dd 0x64


RRF_RT_ANY dd 0x0000FFFF

zonecursor dd 0 ;;needed when clearing the zones (zonecurser++ until lastzone)
lastzone dd 0
storezone dd 0

profilevalue db "profile",0
profiledata db "MPEG4 Simple @ L6",0
reg_sz dd 18
profiledata2 rb 32
reg_sz2 dd 32












;capture10
storesize rd 1
storekey rd 1

framesininterval rd 1

pointerhighs rd 1
pointerlows rd 1

pointer rd 1

intervalsize rd 1

framesizespointer rd 1
uframesizespointer rd 1
frameszszpointer rd 1
keyframespointer rd 1

framesizes rd 800
uframesizes rd 800
frameszsz rb 800
keyframes rb 800
