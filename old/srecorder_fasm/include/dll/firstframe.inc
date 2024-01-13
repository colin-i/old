

;;this file is because it is not required to compare the previous frame with the current frame for equality at the first frame



	push	PM_NOREMOVE
	push	0
	push	0
	push	0
	push	msg.hwnd
	call	[PeekMessage]
	cmp	[switch],0
	jz	.end6

	mov	eax,[pvBits1]
	mov	[lastbuffer],eax

	push	[hbm1]
	push	[destDC]
	call	[SelectObject]

	push	SRCCOPY
	push	[top]
	push	[left]
	push	[srcDC]
	push	[bottom]
	push	[right]
	push	0
	push	0
	push	[destDC]
	call	[BitBlt]

	jmp	.writeframe