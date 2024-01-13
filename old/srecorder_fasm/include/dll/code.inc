

proc DllEntryPoint, hinstDLL,fdwReason,lpvReserved
	mov	eax,TRUE
	ret
endp

include "codec.inc"

proc LowLevelKeyboardProc,nCode,wParam,lParam ;for detecting the stop key
	mov	eax,[wParam]
	cmp	ax,WM_KEYDOWN
	jnz	.end
	mov	eax,[lParam]
	mov	ecx,[eax]
	cmp	cl,[stop_key]
	jnz	.end

	mov	[switch],0

	.end:
	push	[lParam]
	push	[wParam]
	push	[nCode]
	push	NULL
	call	[CallNextHookEx]
	ret
endp

include "size.inc"

;arguments: pointer to MAIN_STRUCT
;           pointer to a dword(for total frames)
;       opt event handle 1 (ignored if pointer to a dword = NULL)
;       opt event handle 2 (ignored if pointer to a dword = NULL)
;
;return:  eax: 0 Success
;              1 Output file already exist
;              2 Cannot open output file
;              3 Stop key isn't alphanumeric
;              4 Frames per second isn't from 1 to 32
;              5 Cannot open the codec
;              6 Cannot open the temporary file
;              7 Cannot write to output file
;              8 Cannot allocate memory
;              9 Wrong coordinates
;              10 Cannot write to temporary file
;              11 Cannot create a device context
;              12 Cannot create a dib section
;              13 Cannot add a windows hook
;              14 Cannot initialize the compressor
;         edx: 0 No warning
;              1 RegOpenKeyEx failed
;              2 RegGetValue for num_zones failed
;              3 RegGetValue for first zone%u_frame failed
;              4 RegSetValueEx for zone.. failed
;              5 RegGetValue for profile data failed
;              6 RegSetValueEx for profile data failed at capture9 call
;              7 RegSetValueEx for profile data failed at code call

proc capture
	mov	[storevalue],0
	mov	[warnvalue],0
	add	esp,4
	pop	eax
	pop	[totalframes]
	pop	[exeevent]
	pop	[dllevent]
	sub	esp,20

	mov	edx,[eax]
	mov	[file_out],edx
	add	eax,4

	mov	dl,[eax]
	mov	[f_bool],dl
	add	eax,1

	mov	edx,[eax]
	mov	[temp_out],edx
	add	eax,4

	mov	dl,[eax]
	mov	[delay],dl
	add	eax,1

	mov	dl,[eax]
	mov	[stop_key],dl
	add	eax,1

	mov	dl,[eax]
	mov	[fps],dl
	add	eax,1

	xor	edx,edx
	mov	dx,[eax]
	mov	[left],edx
	add	eax,2

	mov	dx,[eax]
	mov	[right],edx
	add	eax,2

	mov	dx,[eax]
	mov	[top],edx
	add	eax,2

	mov	dx,[eax]
	mov	[bottom],edx
	add	eax,2

	mov	edx,[eax]
	mov	[FOURCC_identifier],edx
	add	eax,4

	push	[_S_IWRITE]
	push	[_SH_DENYNO]

	mov	eax,[_O_CREAT]
	or	eax,[_O_BINARY]
	or	eax,[_O_WRONLY]

	cmp	[f_bool],0
	jnz	 .else

	.if:
	or	eax,[_O_EXCL]
	jmp	.ifelse

	.else:
	or	eax,[_O_TRUNC]

	.ifelse:
	push	eax
	push	[file_out]
	push	file_handler
	call	[sopen_s]
	add	esp,20

	cmp	eax,0
	jz	.next

	cmp	[f_bool],0
	jnz	.err2

	push	errno
	call	[get_errno]
	add	esp,4

	mov	eax,[errno]
	cmp	eax,[EEXIST]

	jnz	.err2
	mov	[storevalue],1	      ;1 already exist
	jmp	.end

	.err2:
	mov	[storevalue],2;;cannot open
	jmp	.end

	.next:
	mov	al,[stop_key]
	cmp	al,48
	jb	.err3
	cmp	al,57
	jbe	.verify_fps
	cmp	al,65
	jb	.err3
	cmp	al,90
	jbe	.verify_fps

	.err3:
	mov	[storevalue],3
	jmp	.end1

	.verify_fps:
	mov	al,[fps]
	cmp	al,1
	jb	.fpserr
	cmp	al,32
	jbe	.lrtb

	.fpserr:
	mov	[storevalue],4
	jmp	.end1

	.lrtb:
	mov	eax,[right]
	cmp	[left],eax
	jae	.coorderr

	sub	eax,[left]
	cmp	eax,32
	jb	.coorderr

	cmp	eax,9000
	ja	.coorderr

	xor	edx,edx
	div	[quotientnr]
	cmp	edx,0
	jnz	.coorderr

	mov	eax,[bottom]
	cmp	[top],eax
	jae	.coorderr

	sub	eax,[top]
	cmp	eax,32
	jb	.coorderr

	cmp	eax,9000
	ja	.coorderr

	and	eax,1
	jz	.fourcc


	.coorderr:
	mov	[storevalue],9
	jmp	.end1

	.fourcc:
	cmp	[FOURCC_identifier],0	;0 is uncompressed
	jz	.afterfourcc

	push	[ICMODE_QUERY]
	push	[FOURCC_identifier]
	push	[ic.fccType]
	call	[ICOpen]

	cmp	eax,0
	jnz	.closequery
	mov	[storevalue],5
	jmp	.end1

	.closequery:
	push	eax
	call	[ICClose]
	.afterfourcc:

include "capture1.inc"
include "capture2.inc"
include "capture3.inc"

	.lastend:
	cmp	[FOURCC_identifier],0
	jz	.end6

	push	ic.cbSize
	call	[ICSeqCompressFrameEnd]
	push	ic.cbSize
	call	[ICCompressorFree]

	.end8:
	push	[ic.hic]
	call	[ICClose]

	.end7:
	cmp	[warnvalue],1
	jz	.end6

	cmp	[warnvalue],2
	jz	.endk

	push	[reg_dword]
	push	store_num
	push	REG_DWORD
	push	NULL
	push	num_zones
	push	[hkey]
	call	[RegSetValueEx]

	cmp	[warnvalue],3
	jz	.endk
	cmp	[warnvalue],4
	jz	.endk

	.removekeys:
	mov	eax,[zonecursor]
	cmp	eax,[lastzone]
	jz	.jumptokeyactions

	mov	[regcounter],1
	mov	[valuestorepointer],rvastr1

	.delzone:
	mov	eax,[valuestorepointer]
	mov	edx,[eax]
	push	[zonecursor]
	push	edx
	push	valuestore
	call	[sprintf]
	push	[hkey]
	call	[RegDeleteValue]
	add	esp,8

	cmp	[regcounter],9
	jz	.inccursor
	inc	[regcounter]
	add	[valuestorepointer],4
	jmp	.delzone

	.inccursor:
	inc	[zonecursor]
	jmp	.removekeys

	.jumptokeyactions:
	cmp	[warnvalue],0
	jnz	.endk

	push	[reg_sz2]
	push	profiledata2
	push	REG_SZ
	push	NULL
	push	profilevalue
	push	[hkey]
	call	[RegSetValueEx]
	cmp	eax,0
	jz	.endk
	mov	[warnvalue],7

	.endk:
	push	[hkey]
	call	[RegCloseKey]

	.end6:
	push	[hookhandle]
	call	[UnhookWindowsHookEx]

	.end5:
	push	[temp_file_handler]
	call	[close]
	add	esp,4

	.end4:
	push	[hbm2]
	call	[DeleteObject]

	.end3:
	push	[hbm1]
	call	[DeleteObject]

	.end2:
	push	[destDC]
	call	[DeleteDC]

	.end1:
	push	[file_handler]
	call	[close]
	add	esp,4

	.end:
	mov	edx,[warnvalue]
	mov	eax,[storevalue]
	ret

	.valuesend:
	add	esp,12
	.tolastend:
	mov	[storevalue],7
	jmp	.lastend
endp