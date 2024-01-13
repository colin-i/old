




;;;;;;;;;;;;initialize some variables for this call

;;size
	mov	[sw],0
;;

;;capture2
	mov	[switch],1
	mov	[selectedbuffer],0
	mov	[seconds],0
	mov	[framesinsec],1
;;

;;mkv
	mov	[substractionloss],0
	mov	[date_value_highs],0
	mov	[date_value_lows],0
  ;cl
	mov	[timecode_size],(0x80 or 1)
	mov	[cl_timecode],0
	mov	[cl_timecode_size],1
	mov	[clendflag],0
	mov	[framesnote],0
	mov	[tfile_pointer_highs2],0
	mov	[tfile_pointer_lows2],1
     ;simpleblock
	mov	[frames_in_sec],1
	mov	[currentsec],0
	mov	[accumulatorsec],0
	mov	[closedsec],0
	mov	[localtimecode],0
	mov	[ltcursor],localtimes

  ;cues
	mov	[cue_size],13
	mov	[cues_size],0
	mov	[cuepoint_size],(0x80 or 11)
	mov	[timecode_cues],(0x80 or 1)
	mov	[cuetrackpositions_size],(0x80 or 6)
	mov	[cueclusterposition_size],(0x80 or 1);;;;;;;;;;;;;;mare atentie ca asta se poate marii cand e codec mare
	mov	[cueclusterposition_value1],0
	mov	[cueclusterposition_value2],1
	mov	[cueshighssz],0
	mov	[cueslowssz],1


;;compdata
	mov	[store_num],0



;;;;;;;;;;;;

	push	NULL
	call	[GetWindowDC]
	mov	[srcDC],eax
	push	[srcDC]
	call	[CreateCompatibleDC]
	cmp	eax,NULL
	jnz	.compdc
	mov	[storevalue],11
	jmp	.end1
	.compdc:
	mov	[destDC],eax

	mov	eax,[bottom]
	sub	eax,[top]
	mov	[bmih.biHeight],eax
	mov	eax,[right]
	sub	eax,[left]
	mov	[bmih.biWidth],eax

;biSizeImage
	mov	ecx,3
	mul	ecx
	add	eax,3
	and	eax,0xFFFFFFFC
	mul	[bmih.biHeight]
	mov	[bmih.biSizeImage],eax

	push	NULL
	push	NULL
	push	pvBits1
	push	[DIB_RGB_COLORS]
	push	bmih.biSize
	push	destDC
	call	[CreateDIBSection]	 ;creating the dib section 1
	cmp	eax,NULL
	jnz	.dib1
	mov	[storevalue],12
	jmp	.end2
	.dib1:
	mov	[hbm1],eax
	mov	eax,[pvBits1]
	add	eax,[bmih.biSizeImage]
	sub	eax,4
	mov	[max_address],eax

	sub	esp,8
	push	pvBits2
	sub	esp,12
	call	[CreateDIBSection]	 ;creating the dib section 2
	cmp	eax,NULL
	jnz	.dib2
	mov	[storevalue],12
	jmp	.end3
	.dib2:
	mov	[hbm2],eax

	;;;;;;;;;;;;;;;;;temp file
	push	[_S_IWRITE]
	push	[_SH_DENYNO]

	push	hkey
	push	KEY_QUERY_VALUE
	push	0
	push	tempsecret
	push	HKEY_CURRENT_USER
	call	[RegOpenKeyEx]
	cmp	eax,0
	jz	.notempflag
	mov	 eax,[_O_TEMPORARY]
	jmp	.flags
	.notempflag:
	xor	eax,eax

	.flags:
	or	eax,[_O_CREAT]
	or	eax,[_O_BINARY]
	or	eax,[_O_RDWR]
	or	eax,[_O_EXCL]

	push	eax
	push	[temp_out]
	push	temp_file_handler
	call	[sopen_s]
	add	esp,20

	cmp	eax,0
	jz	.delay
	mov	[storevalue],6
	jmp	.end4

;;;;;;;;delay
	.delay:
	cmp	[delay],0
	jz	.skipsleep
	mov	al,[delay]
	mul	[mspersecond]
	push	eax
	call	[Sleep]
	.skipsleep:
;;;;;;;;

;;;;playsound
	push	[SND_SYNC]
	push	NULL
	push	sound
	call	[PlaySound]
;;;;

;;;;;;;;;keyboard hook
	push	0
	push	0
	push	LowLevelKeyboardProc
	push	[WH_KEYBOARD_LL_changed]
	call	[SetWindowsHookEx]
	cmp	eax,NULL
	jnz	.winhook
	mov	[storevalue],13
	jmp	.end5
	.winhook:
	mov	[hookhandle],eax
;;;;;;;;;
