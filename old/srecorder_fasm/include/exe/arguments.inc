

	call	[GetCommandLine]
	mov	bh,0x20
	mov	bl,[eax]
	cmp	bl,0x22 ;quotation sign
	jnz	 .tojump
	mov	bh,0x22

	.tojump:
	inc	eax
	mov	bl,[eax]
	cmp	bl,0
	jz	.helplabel
	cmp	bl,bh
	jz	.cmdparse
	jmp	.tojump

	.cmdparse:
	mov	dl,0   ;out
	mov	dh,0x20;space for closing argument switch
	mov	edi,0  ;counter for variables

	.cmdparsenew:
	inc	eax
	mov	bl,[eax]
	.cmdparsefromquotation:
	cmp	bl,0		 ;0 is the end of this loop
	jz	.cmdparsedone

	cmp	dl,1
	jz	.cmdparsearg	 ;dl = 1 is when it is in argument

	cmp	bl,0x20        ;space compare
	jz	.cmdparsenew
	mov	dl,1	       ;dl = 1, argument start
	inc	edi

	cmp	bl,0x22 	 ;quotation sign
	jnz	.newarg 	 ;jump if not

	mov	dh,0x22        ;closing argument switch becomes quotation sign
	inc	eax
	push	eax
	mov	bl,[eax]
	jmp	.cmdparsefromquotation

	.newarg:
	push	eax	       ;push the start for the argument

	.cmdparsearg:
	cmp	bl,dh	       ;compare with the closing argument switch
	jnz	.cmdparsenew

	mov	dh,0x20 	;if is , default dh = 'space'
	mov	dl,0		;dl = 0 outside argument
	sub	eax,3		;null ending , so the argument is a null term string starting from the pointer pushed on the stack
	mov	ecx,[eax]	;null ending
	and	ecx,0xFFFFFF	;null ending
	mov	[eax],ecx	;null ending
	add	eax,3		;null ending

	jmp	.cmdparsenew

	.cmdparsedone:
	cmp	dh,0x22;;when "xy[NULL] encountered
	jnz	.properly

	push	bad_argument   ;not good
	jmp	.toend

	.properly:
	cmp	edi,0
	jz	.helplabel

	mov	esi,rva1       ;esi pointer to rva1
	mov	bl,0	       ;bl first iterator

	mov	eax,edi        ;eax total variables
	dec	eax	       ;decrement and shift left for going at the begining of the arguments
	shl	eax,2
	mov	ebp,esp        ;ebp pointer for arguments
	add	ebp,eax        ;substracting the eax for the first argument
	mov	eax,[ebp]      ;moving eax the first arg pointer
	push	eax	       ;pushing the first string

	.loopstart1:
	mov	bh,0	    ;bh = second iterator
	 .loopstart2:
	 mov	eax,[esi]
	 push	eax	    ;push the second string
	 call	[strcmp]    ;comparing
	 cmp	eax,0	    ;compare with 0
	 jz	.endloop1   ;if equal the 'Method' was chosed
	 add	esp,4	    ;if not , the next one
	 add	esi,4	    ;

	 cmp	bh,2		   ;iterator
	 jz	.endloop2
	 inc	bh		   ;iterator
	 jmp	.loopstart2
	 .endloop2:

	cmp	bl,2		 ;until max
	jz	.endloop1
	inc	bl
	jmp	.loopstart1
	.endloop1:

	cmp	eax,0  ;compare with 0
	jz	.helpcompare
	push	unrecognized1	   ;if not the 'Method isn't recognized
	jmp	.toend

	.helpcompare:
	cmp	bl,0		  ;help display
	jnz	.viewcompare
	.helplabel:
	push	help
	jmp	.toend

	.viewcompare:
	cmp	bl,1		  ;codec
	jnz	.maxcompare

	include "codec.inc"

	.maxcompare:		 ;maximum of arguments
	mov	esi,edi
	sub	esi,21	  ;1 is the method and 10 optional arguments*2(--delay 0) = 21
	jbe	.optargs
	push	many	     ;error too many
	jmp	.toend

	.optargs:
	cmp	edi,1	     ;if no optional arguments jump at default files names
	jz	.f_t_out
	mov	bl,1	     ;iterator 1
	mov	esi,0	     ;switch, 0 is the name('--delay'), 1 is the value('0')
	mov	eax,edi
	mov	[counter],al ;counter is the maximum variables

	.loopentry1:
	cmp	bl,[counter]	 ;iteration
	jz	.endsec

	sub	ebp,4	       ;next argument
	mov	edx,[ebp]

	cmp	esi,0		;if 1 jump to value
	jnz	.valuenext

	mov	[pointer],edx
	add	edx,1 ;example: from '-delay' to 'delay'
	push	edx	       ;name argument for comparisson
	mov	bh,1	       ;iterator 2
	mov	edi,orva1      ;rva

	  .loopentry2:
	  mov	eax,[edi]      ;arguments from the list
	  push	eax
	  call	[strcmp]
	  cmp	eax,0	     ;0 -> eguals
	  jz	.next

	  cmp	bh,10	     ;compare with maxim
	  jz	.else

	  inc	bh	     ;next argument
	  add	edi,4
	  add	esp,4
	  jmp	.loopentry2

	  .else:
	  push	[pointer]
	  push	unrecognized2	      ;error secondary name
	  jmp	.toend

	.valuenext:

	mov	eax,[edi]
	dec	eax
	mov	cl,[eax]	;cl is the argument index

	cmp	cl,2	 ;;file name and temporary file name
	ja	.cmp1

	mov	edi,0	;for maximum_path
	cmp	cl,1
	jnz	.temp_out
	mov	ecx,file_out_name ;ecx destination
	or	[files_flag],1
	jmp	.filesloop1
	.temp_out:
	mov	ecx,temp_file_out_name;
	or	[files_flag],2

	.filesloop1:
	mov	bh,0
	.filesloop2:
	mov	al,[edx]
	mov	[ecx],al
	inc	ecx
	inc	edx
	cmp	al,0
	jz	.next
	inc	edi
	cmp	edi,MAX_PATH
	jnz	.notmax
	push	longinput	  ;input path too long
	jmp	.toend
	.notmax:
	cmp	bh,3
	jz	.filesloop1
	inc	bh
	jmp	.filesloop2

	.cmp1:
	mov	edi,edx  ;for 'atoi' and print error
	mov	edx,[edx];the value
;0 to error (delay,fourcc related with this and for overall is ok)


	cmp	cl,3	;;delay
	jnz	.cmp2

	cmp	dh,0
	jnz	.err
	call	numeric
	cmp	al,0
	jz	.err
	sub	dl,48
	mov	[delay],dl
	jmp	.next

	.cmp2:		 ;;stop_key
	cmp	cl,4
	jnz	.cmp3
	cmp	dh,0
	jnz	.err

	cmp	dl,97
	jb	.assign
	cmp	dl,122
	ja	.assign
	sub	dl,32

	.assign:
	mov	[stop_key],dl
	jmp	.next

	.cmp3:		 ;;fps
	cmp	cl,5
	jnz	.cmp4
	mov	ch,0

	.cmp3ext:
	call	numeric
	cmp	al,0
	jz	.err

	cmp	dh,0
	jz	.cmp3ext2

	cmp	ch,0
	jnz	.err

	inc	ch
	ror	edx,8
	jmp	.cmp3ext

	.cmp3ext2:
	push	edi
	call	[atoi]
	mov	[fps],al;eax
	jmp	.next

	.cmp4:
	cmp	dl,0
	jz	.err
	cmp	cl,10	     ;;left,right,top,bottom
	jz	.cmp8
	mov	ch,0	;iterator

	.cmp4ext:
	cmp	ch,4
	jz	.cmp4ext2

	call	numeric
	cmp	al,0
	jz	.cmp4ext3

	ror	edx,8
	inc	ch
	jmp	.cmp4ext

	.cmp4ext2: ;if 5th is 0 (4444)
	mov	eax,edi
	add	eax,4
	mov	dl,[eax]
	call	numeric
	cmp	al,0
	jnz	.err
	jmp	.cmp4ext4

	.cmp4ext3: ;if 2-4 is 0 is ok (4,44,444)
	cmp	dl,0
	jnz	 .err

	.cmp4ext4:
	mov	bh,cl

	push	edi
	call	[atoi] ;string to int

	cmp	bh,6	;left
	jnz	.cmp5
	mov	[left],ax
	jmp	.next

	.cmp5:		;right
	cmp	bh,7
	jnz	.cmp6
	mov	[right],ax
	jmp	.next

	.cmp6:		;top
	cmp	bh,8
	jnz	.cmp7
	mov	[top],ax
	jmp	.next

	.cmp7:		;bottom
	mov	[bottom],ax
	jmp	.next

	.cmp8:		       ;;fourcc
	mov	eax,edx
	cmp	dh,0
	jz	.err
	ror	edx,8
	cmp	dh,0
	jz	.err
	ror	edx,8
	cmp	dh,0
	jz	.err
	add	edi,4
	mov	dl,[edi]
	cmp	dl,0
	jnz	.err

	mov	[FOURCC_identifier],eax
	jmp	.next

	.err:
	push	edi
	push	unrecognized3	      ;error secondary value with the value displayed
	jmp	.toend

	.next:
	inc	bl    ;iteration 1
	xor	esi,1 ;switch to value
	jmp	.loopentry1

	.endsec:
	cmp	esi,0
	jz	.f_t_out
	push	unrecognized4	     ;error missing secondary value
	jmp	.toend

	.f_t_out:		   ;default file, fileout our tempout, if not specified, they will be the time from 1 jan 1970 (+1 sec temp if both)
	cmp	[files_flag],3
	jz	.call

	push	time_t
	call	[time]

	mov	eax,time_t;;
	mov	ecx,[eax] ;;high 8 bits
	add	eax,4	  ;;
	mov	edx,[eax] ;;low 8 bits
	push	edx	  ;;
	push	ecx	  ;;


	push	lli_conv  ;;to string

	cmp	[files_flag],1
	jz	.pushtemp1
	push	file_out_name
	mov	bl,1
	jmp	.aftercmp1
	.pushtemp1:
	push	temp_file_out_name
	mov	bl,2
	.aftercmp1:

	call	[sprintf]

;;
	;;if file_out is the default then it is required to add ".mkv" extension
	cmp	bl,1
	jnz	.anothercomparisson
	push	extension
	push	file_out_name
	call	[strcat]
	add	esp,8
;;
	.anothercomparisson:
	or	[files_flag],bl
	cmp	[files_flag],3
	jz	.call

	add	esp,8
	mov	eax,[esp]
	inc	eax
	mov	[esp],eax
	sub	esp,4

	push	temp_file_out_name

	call	[sprintf]










