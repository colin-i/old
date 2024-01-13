
;arguments: an array pointer pointer (the dll create the array with the codecs and place the array pointer into this locations)
;return: the number of elements
proc query
	add	esp,4

	pop	[stringpointerpointer]
	sub	esp,12

	push	[ICMODE_QUERY]
	push	[ic.fccHandler]    ; 'xvid'
	push	[ic.fccType]
	call	[ICOpen]
	cmp	eax,0
	jz	.ret

	push	eax
	call	[ICClose]

	mov	ecx,[ic.fccHandler];at this version there is supported only 'xvid'
	mov	[query_return],ecx

	mov	edx,[stringpointerpointer]
	mov	ecx,query_return
	mov	[edx],ecx

	mov	eax,1

	.ret:
	add	esp,4
	ret
endp