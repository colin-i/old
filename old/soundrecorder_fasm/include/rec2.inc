



push	recording
call	[printf]

push	[hwi]
call	[waveInStart]

push	1000
call	[Sleep]
call	[GetTickCount]
mov	ebx,eax
mov	esi,lpData1
mov	edi,[lpData1]

.loop:
cmp	[flag],1
jz	.stop
add	ebx,2000
call	[GetTickCount]

mov	edx,ebx
sub	edx,eax
push	edx
call	[Sleep]

push	(2*22050)
push	edi
push	[file_handler]
call	[write]
add	esp,12
cmp	eax,-1
jnz	.label
push	err3
call	[printf]
jmp	.stop

.label:
push	32
push	esi
push	[hwi]
call	[waveInAddBuffer]
cmp	eax,0
jz	.movevars
push	err7
call	[printf]
jmp	.stop

.movevars:
cmp	esi,lpData1
jz	.add2
mov	esi,lpData1
mov	edi,[lpData1]
jmp	.loop
.add2:
mov	esi,lpData2
mov	edi,[lpData2]
jmp	.loop


.stop:
push	[hwi]
call	[waveInStop]

push	[SEEK_END]
push	0
push	[file_handler]
call	[lseek]
sub	eax,8
mov	[riffsize],eax

push	[SEEK_SET]
push	4
push	[file_handler]
call	[lseek]

push	4
push	riffsize
push	[file_handler]
call	[write]

sub	[riffsize],38

push	[SEEK_SET]
push	42
push	[file_handler]
call	[lseek]

push	4
push	riffsize
push	[file_handler]
call	[write]