




push	48
push	wMid
push	0
call	[waveInGetDevCaps]

mov	eax,[dwFormats]
and	eax,[WAVE_FORMAT_4S16]
jnz	.format_is_supported
push	err1
call	[printf]
jmp	.end
.format_is_supported:

push	46
push	RIFF
push	[file_handler]
call	[write]
cmp	eax,-1
jnz	.jump
push	err3
call	[printf]
jmp	.end
.jump:

push	0
push	0
push	0
push	wFormatTag
push	0
push	hwi
call	[waveInOpen]
cmp	eax,0;MMSYSERR_NOERROR
jz	.alloc
push	err4
call	[printf]
jmp	.end

.alloc:
push	[dwBufferLength1]
call	[malloc]
cmp	eax,NULL
jnz	.alloc2
push	err5
call	[printf]
jmp	.end
.alloc2:
mov	[lpData1],eax
push	[dwBufferLength2]
call	[malloc]
cmp	eax,NULL
jnz	.allocdone
push	err5
call	[printf]
jmp	.end
.allocdone:
mov	[lpData2],eax

push	32
push	lpData1
push	[hwi]
call	[waveInPrepareHeader];prepare the input-buffer for using by input-device
cmp	eax,0
jz	.next1
jmp	.err6

.next1:
push	32
push	lpData1
push	[hwi]
call	[waveInAddBuffer];add the buffer to input-device
cmp	eax,0
jz	.next2
jmp	.err7

.next2:
push	32
push	lpData2
push	[hwi]
call	[waveInPrepareHeader]
cmp	eax,0
jz	.next3
jmp	.err6

.next3:
push	32
push	lpData2
push	[hwi]
call	[waveInAddBuffer]
cmp	eax,0
jz	.thread

.err7:
push	err7
call	[printf]
jmp	.end

.err6:
push	err6
call	[printf]
jmp	.end

.thread:
push	0
push	0
push	0
push	.getch
push	0
push	0
call	[CreateThread]
cmp	eax,NULL
jnz	.jumping
push	err8
call	[printf]
jmp	.end
.jumping:


