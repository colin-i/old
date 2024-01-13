





;;open the registry
	mov	eax,KEY_QUERY_VALUE
	or	eax,KEY_SET_VALUE
	push	hkey
	push	eax
	push	0
	push	keypath
	push	HKEY_CURRENT_USER
	call	[RegOpenKeyEx]
	cmp	eax,0
	jz	.reg
	mov	[warnvalue],1
	jmp	.donekeys

	.reg:
	push	reg_dword
	push	regstore
	push	REG_NONE
	push	[RRF_RT_ANY]
	push	num_zones
	push	NULL
	push	[hkey]
	call	[RegGetValue]
	cmp	eax,0
	jz	.reg2
	mov	[warnvalue],2
	jmp	.donekeys

	.reg2:
	mov	eax,[regstore]
	mov	[zonecursor],eax
	mov	[lastzone],eax
	mov	[store_num],eax
	dec	[regstore]
	push	[regstore]
	push	valuestorestring4
	push	valuestore
	call	[sprintf]
	add	esp,12

	push	reg_dword
	push	regstore
	push	REG_NONE
	push	[RRF_RT_ANY]
	push	valuestore
	push	NULL
	push	[hkey]
	call	[RegGetValue]
	cmp	eax,0
	jz	.reg3
	mov	[warnvalue],3
	jmp	.donekeys

	.reg3:
	mov	[datastore4],0
;;jump for starting from the second frame
	cmp	[frames],0
	jnz	.jumpmark1
;;
	.looping:
	mov	eax,[datastore4]
	cmp	eax,[frames]
	jz	.setcounter

	push	[SEEK_CUR]
	push	0
	push	[bmih.biSizeImage]
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

.jumpmark1:
	push	1
	push	currentsec
	push	[temp_file_handler]
	call	[read]
	add	esp,12
	inc	[datastore4]

	mov	al,[currentsec]
	mov	cl,[timemin]
	mov	dl,[timemax]
	mov	[timemin],al

	cmp	cl,dl
	ja	.compare2
	cmp	al,cl
	jb	.ifr
	cmp	al,dl
	ja	.ifr
	jmp	.looping
	.compare2:
	cmp	al,cl
	jae	.looping
	cmp	al,dl
	jbe	.looping


	.ifr:
	mov	eax,[datastore4]
	cmp	eax,[regstore]
	jbe	.reg5

	mov	eax,[lastzone]
	mov	[storezone],eax
	inc	[lastzone]
	mov	[regcounter],1
	mov	[valuestorepointer],rvastr1
	mov	[datastorepointer],datastore1

	.regzone:
	push	[reg_dword]
	push	[datastorepointer]
	push	REG_DWORD
	push	NULL

	mov	eax,[valuestorepointer]
	mov	edx,[eax]
	push	[storezone]
	push	edx
	push	valuestore
	call	[sprintf]
	add	esp,12

	push	valuestore
	push	[hkey]
	call	[RegSetValueEx]
	cmp	eax,0
	jz	.reg4
	mov	[warnvalue],4
	jmp	.donekeys

	.reg4:
	cmp	[regcounter],9
	jz	.reg5
	inc	[regcounter]
	add	[valuestorepointer],4
	add	[datastorepointer],4
	jmp	.regzone

	.reg5:
	mov	al,[timemin]
	mov	[timemax],al
	add	[timemax],31
	jmp	.looping

	.setcounter:
	push	[reg_dword]
	push	lastzone
	push	REG_DWORD
	push	NULL
	push	num_zones
	push	[hkey]
	call	[RegSetValueEx]

	push	reg_sz2
	push	profiledata2
	push	REG_NONE
	push	[RRF_RT_ANY]
	push	profilevalue
	push	NULL
	push	[hkey]
	call	[RegGetValue]
	cmp	eax,0
	jz	.writeprofile
	mov	[warnvalue],5
	jmp	.donekeys

	.writeprofile:
	push	[reg_sz]
	push	profiledata
	push	REG_SZ
	push	NULL
	push	profilevalue
	push	[hkey]
	call	[RegSetValueEx]
	cmp	eax,0
	jz	.donekeys
	mov	[warnvalue],6

	.donekeys:


