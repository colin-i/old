






	.timeflag:
	mov	cl,[accumulatorsec]
	mov	[closedsec],cl;;;needed for time difference and case of comeback to have the closed sec the accumulator sec
	mov	[accumulatorsec],al;;;needed for time difference and case of comeback to have the open sec the sec from for_reading

	xor	edx,edx
	mov	eax,1000
	mov	cl,[frames_in_sec]
	and	cx,0xFF
	div	cx
	mov	[timecodedif],ax
	mov	[timecoderest],0
	mov	dh,1
	mov	ecx,[ltcursor]

	.timeloop:
	mov	eax,[localtimecode]
	mov	[ecx],ah
	add	ecx,1
	mov	[ecx],al
	add	ecx,1

	mov	ax,[timecodedif]
	add	[localtimecode],eax


	add	[timecoderest],dl
	jz	.afterrest
	mov	al,[timecoderest]
	mov	ah,[frames_in_sec]
	cmp	al,ah
	jb	.afterrest
	inc	[localtimecode]
	sub	[timecoderest],ah
	.afterrest:


	cmp	dh,[frames_in_sec]
	jz	.timedone
	inc	dh
	jmp	.timeloop
;

	.timedone:
	mov	[ltcursor],ecx
	mov	[frames_in_sec],1


;;;seconds that are not present
	mov	al,[accumulatorsec]
	mov	cl,[closedsec]
	and	ax,0xFF
	and	cx,0xFF
	cmp	cl,al
	jz	.tfend
	jb	.continuetimewr
	mov	ah,1
	.continuetimewr:
	sub	ax,cx
	dec	ax
	jz	.tfend
	mov	edx,1000
	mul	edx
	add	[localtimecode],eax
;;;









	.tfend:
	cmp	[clendflag],0
	jz	.anotherclusterentry

