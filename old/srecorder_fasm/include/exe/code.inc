

proc numeric	  ;the numeric proc for returning if dl is number
	cmp	dl,48
	jb	.err
	cmp	dl,57
	ja	.err
	mov	al,1
	ret
	.err:
	mov	al,0
	ret
endp

proc threadproc  ;the thread proc for printing the progress
	jmp	.incycle

	.printprecentage:
	push	[dllevent]
	call	[SetEvent]

	.incycle:
	push	[INFINITE]
	push	[exeevent]
	call	[WaitForSingleObject]
	cmp	[endflag],1
	jz	.end

	push	[exeevent]
	call	[ResetEvent]

	cmp	[frame],0
	ja	.frameprogress

	push	infostr
	call	[printf]
	mov	edi,coord.X
	mov	edi,[edi]
	mov	esi,100
	inc	[totalframes]

	.frameprogress:
	push	edi
	push	[hconsole]
	call	[SetConsoleCursorPosition]

	xor	edx,edx
	inc	[frame]
	mov	eax,[frame]
	mul	esi
	div	[totalframes]

	push	eax
	push	printprecentage
	call	[printf]
	add	esp,8
	jmp	.printprecentage

	.end:
	ret
endp

start:

include "arguments.inc"

	.call:
	push	exeeventstr
	push	NULL
	push	NULL
	push	NULL
	call	[CreateEvent]
	cmp	eax,NULL
	jnz	.call1
	push	event_err
	call	[printf]
	jmp	.end
	.call1:
	mov	[exeevent],eax

	push	dlleventstr
	push	NULL
	push	NULL
	push	NULL
	call	[CreateEvent]
	cmp	eax,NULL
	jnz	.call2
	push	event_err
	jmp	.toend
	.call2:
	mov	[dllevent],eax

	push	NULL
	push	NULL
	push	NULL
	push	threadproc
	push	NULL
	push	NULL
	call	[CreateThread]
	cmp	eax,NULL
	jnz	.continue2

	push	thread_err
	jmp	.toend

	.continue2:
	xor	eax,eax
	mov	al,[stop_key]
	push	eax
	mov	al,[delay]
	push	eax
	push	count
	call	[printf]

;;position for displaying the progress
	push	STD_OUTPUT_HANDLE
	call	[GetStdHandle]
	mov	[hconsole],eax

	push	csbi.dwSize.X
	push	eax
	call	[GetConsoleScreenBufferInfo]

	mov	ax,[csbi.dwCursorPosition.Y]
	add	ax,3
	cmp	[csbi.dwSize.Y],ax
	jae	.setY
	mov	ax,[csbi.dwSize.Y]
	dec	ax
	.setY:
	mov	[coord.Y],ax
;;


	push	[dllevent]
	push	[exeevent]
	push	totalframes
	push	file_out_name_pointer
	call	[capture]

	cmp	al,0
	jz	.goodend

	cmp	al,1
	jnz	.err2
	push	already
	call	[printf]

	.write:
	push	size_t
	push	4
	push	overstr
	call	[cgets]

	push	overstr
	push	yes
	call	[strcmp]
	jz	.again

	jmp	.end

	.again:
	mov	[overwrite_file_out],1
	add	[coord.Y],3	       ;;;;the coord point is increased by 3 if the answer received

	push	[dllevent]
	push	[exeevent]
	push	totalframes
	push	file_out_name_pointer
	call	[capture]

	cmp	al,0
	jz	.goodend

	.err2:
	cmp	al,2
	jnz	.err3
	push	open_output
	jmp	.toend

	.err3:
	cmp	al,3
	jnz	.err4
	push	stop_err
	jmp	.toend

	.err4:
	cmp	al,4
	jnz	.err5
	push	fps_err
	jmp	.toend

	.err5:
	cmp	al,5
	jnz	.err6
	push	fourcc_err
	jmp	.toend

	.err6:
	cmp	al,6
	jnz	.err7
	push	temp_file_err
	jmp	.toend

	.err7:
	cmp	al,7
	jnz	.err8
	push	write_err
	jmp	.toend

	.err8:
	cmp	al,8
	jnz	.err9
	push	allocation_err
	jmp	.toend

	.err9:
	cmp	al,9
	jnz	.err10
	push	coord_err
	jmp	.toend

	.err10:
	cmp	al,10
	jnz	.err11
	push	temp_write_err
	jmp	.toend

	.err11:
	cmp	al,11
	jnz	.err12
	push	compatible_dc_err
	jmp	.toend

	.err12:
	cmp	al,12
	jnz	.err13
	push	dib_err
	jmp	.toend

	.err13:
	cmp	al,13
	jnz	.err14
	push	hook_err
	jmp	.toend

	.err14:
	push	framestart_err



	.toend:
	call	[printf]

	.end:
	push	0
	call	[ExitProcess]	 ;termination

	.goodend:
	mov	edi,coord.X
	mov	edi,[edi]
	push	edi
	push	[hconsole]
	call	[SetConsoleCursorPosition]
	push	completed
	call	[printf]
	jmp	.end