


	.capture5:

;;jump for starting from the second frame
	cmp	[frames],1
	jnz	.jumpmark2
;;



	.toloop:
	mov	ecx,[framestotalcounter]
	cmp	ecx,[frames]

	jz	.toclusterend

	push	[SEEK_CUR]
	push	0
	push	[bmih.biSizeImage]
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

.jumpmark2:
	push	1
	push	currentsec
	push	[temp_file_handler]
	call	[read]
	add	esp,12



	mov	al,[currentsec]
	mov	cl,[timemin]
	mov	dl,[timemax]
	mov	[timemin],al

	cmp	cl,dl
	ja	.cmp2

	cmp	al,cl
	jb	.toclusterend

	cmp	al,dl
	ja	.toclusterend

	jmp	.comparesec

	.cmp2:
	cmp	al,cl
	jae	.comparesec

	cmp	al,dl
	ja	.toclusterend

	.comparesec:
	cmp	[accumulatorsec],al
	jnz	.timeflag

	inc	[frames_in_sec]

	.anotherclusterentry:
	inc	[framestotalcounter]
	jmp	.toloop




	.toclusterend:
	mov	[clendflag],1
	mov	al,[currentsec]


include "time.inc"

	mov	[clendflag],0
	.clusterend:



	mov	eax,[framestotalcounter]
	sub	eax,[framesnote]
	mov	[framesperinterval],eax
	mov	[frames_per_interval],eax

include "capture6.inc"


;;;adding the just closed cluster time to the total time
	mov	edx,[localtimecode]
	add	[cl_timecode],edx

	mov	eax,[framestotalcounter]
	cmp	eax,[frames]
	jz	.afterloops

	mov	[framesnote],eax
	mov	eax,[storeraw]
	mul	[frames_per_interval]
	add	[tfile_pointer_lows2],eax
	jnc	.prepareanothercl
	inc	[tfile_pointer_highs2]

	.prepareanothercl:
	mov	[localtimecode],0
	mov	[ltcursor],localtimes

	inc	[framestotalcounter]
	mov	ecx,[framestotalcounter]
	cmp	ecx,[frames]
	jz	.toclusterend

	add	[tfile_pointer_lows1],eax
	jnc	.seek
	inc	[tfile_pointer_highs1]

	.seek:
	push	[SEEK_SET]
	push	[tfile_pointer_highs1]
	push	[tfile_pointer_lows1]
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

	mov	al,[timemin]
	mov	[timemax],al
	add	[timemax],31
	jmp	.jumpmark2




	.afterloops:

;write cues

;it is also required to get this position for inputing it in the seekhead, at its location
	push	[SEEK_END]
	push	0
	push	0
	push	[file_handler]
	call	[seek]
	add	esp,16
	sub	eax,52
	jnc	.jumpnotbelow
	dec	edx
	.jumpnotbelow:
	mov	[seekcues_highs],edx
	mov	[seekcues_lows],eax

	push	4
	push	Cues
	push	[file_handler]

	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.thecues
	jmp	.tolastend

	.thecues:
	mov	ecx,[cues_size]
	;flag is 0
	call	size

	push	eax
	push	buffer1
	push	[file_handler]
	call	[write]
	add	esp,12

	cmp	eax,-1
	jnz	.thecues2
	jmp	.tolastend

	.thecues2:
	push	[SEEK_SET]
	push	[temp_pointer_highs]
	push	[temp_pointer_lows]
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

	push	[cues_size]
	call	[malloc]
	cmp	eax,NULL
	jnz	.thecues3
	mov	[storevalue],8
	jmp	.lastend

	.thecues3:
	mov	[cuespointer],eax

	push	eax
	push	[temp_file_handler]
	call	[read]

	add	esp,4
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.thecues4
	jmp	.tolastend

	.thecues4:
	push	[cuespointer]
	call	[free]
	add	esp,4

include "capture8.inc"




