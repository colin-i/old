
start:
	push	help
	call	[printf]
	call	[getch]
	cmp	eax,27
	jz	.end

	push	time_t
	call	[time]

	mov	eax,time_t;;
	mov	ecx,[eax] ;;high 8 bits
	add	eax,4	  ;;
	mov	edx,[eax] ;;low 8 bits
	push	edx	  ;;
	push	ecx	  ;;

	push	lli_conv  ;;to string
	push	file_out_name
	call	[sprintf]

	push	extension
	push	file_out_name
	call	[strcat]

	push	[_S_IWRITE]
	push	[_SH_DENYNO]
	mov	eax,[_O_CREAT]
	or	eax,[_O_BINARY]
	or	eax,[_O_WRONLY]
	or	eax,[_O_EXCL]
	push	eax
	push	file_out_name
	push	file_handler
	call	[sopen_s]
	cmp	eax,0
	jz	.opened
	push	err2
	call	[printf]
	jmp	.end
	.opened:
;;

include "include/rec1.inc"
include "include/rec2.inc"

	.end:
	push	0
	call	[ExitProcess]	 ;termination

.getch:
	call	[getch]
	mov	[flag],1
	ret