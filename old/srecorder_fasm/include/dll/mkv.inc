


;Extensible Binary Meta Language

;IDs
;Element ID coded with an UTF-8 like system :bits, big-endian
;1xxx xxxx                                  - Class A IDs (2^7 -1 possible values) (base 0x8X)
;01xx xxxx  xxxx xxxx                       - Class B IDs (2^14-1 possible values) (base 0x4X 0xXX)
;001x xxxx  xxxx xxxx  xxxx xxxx            - Class C IDs (2^21-1 possible values) (base 0x2X 0xXX 0xXX)
;0001 xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx - Class D IDs (2^28-1 possible values) (base 0x1X 0xXX 0xXX 0xXX)

;Data sizes
;0x80=1xxx xxxx                                                                              - value 0 to  2^7-2
;01xx xxxx  xxxx xxxx                                                                   - value 0 to 2^14-2
;001x xxxx  xxxx xxxx  xxxx xxxx                                                        - value 0 to 2^21-2
;0001 xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx                                             - value 0 to 2^28-2
;0000 1xxx  xxxx xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx                                  - value 0 to 2^35-2
;0000 01xx  xxxx xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx                       - value 0 to 2^42-2
;0000 001x  xxxx xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx            - value 0 to 2^49-2
;0000 0001  xxxx xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx  xxxx xxxx - value 0 to 2^56-2

;File Format
;
;EBML head
;Segment
;       Seek head
;       Segment information
;       Segment tracks
;       Cluster 1
;       Cluster 2
;       .
;       .
;       Cluster n
;       Cue     1
;       Cue     2
;       .
;       .
;       Cue     n

EBML	db 0x1A,0x45,0xDF,0xA3		;Each EBML document has to start with this(class d)
	db (0x80 or 35) 		;this is the EBML constant size for the above element, so it is writted directly to be easily observerd
	EBMLVersion	db 0x42,0x86
			db (0x80 or 1)
				db 1	;this is the value for the EBMLVersion field which is version 1
	EBMLReadVersion db 0x42,0xF7
			db (0x80 or 1)
				db 1
	EBMLMaxIDLength db 0x42,0xF2
			db (0x80 or 1)
				db 4
	EBMLMaxSizeLength	db 0x42,0xF3
				db (0x80 or 1)
					db 8
	DocType db 0x42,0x82
		db (0x80 or 8)
			db 'matroska'
	DocTypeVersion	db 0x42,0x87
			db (0x80 or 1)
				db 2
	DocTypeReadVersion	db 0x42,0x85
				db (0x80 or 1)
					db 2
SegmentElement	db 0x18,0x53,0x80,0x67
		rb 8
;!!!reserved 1  ;here it is reserved 8 bytes,the size is unknown, it is calculated at the end
	SeekHead	db 0x11,0x4D,0x9B,0x74
			db (0x80 or 49)
		SeekEntry1	db 0x4D,0xBB;this seek is for segment information location
				db (0x80 or 11)
					db 0x53,0xAB;SeekID
					db (0x80 or 4)
						db 0x15,0x49,0xA9,0x66;Info
					db 0x53,0xAC;SeekPosition
					db (0x80 or 1)
						db 0x36
		SeekEntry2	db 0x4D,0xBB
				db (0x80 or 11)
					db 0x53,0xAB;SeekID
					db (0x80 or 4)
						db 0x16,0x54,0xAE,0x6B;Tracks
					db 0x53,0xAC;SeekPosition
					db (0x80 or 1)
						db 0x93
		SeekEntry3	db 0x4D,0xBB
				db (0x80 or 18)
					db 0x53,0xAB
					db (0x80 or 4)
						db 0x1C,0x53,0xBB,0x6B;Cues
					db 0x53,0xAC
					db (0x80 or 8)
						rb 8
;!!!reserved 2                                  ;8 bytes reserved(cluster 1..n and then cues)

	SegmentInformation	db 0x15,0x49,0xA9,0x66
				db (0x80 or 88)
		SegmentUID	db 0x73,0xA4
				db (0x80 or 16)
					SegmentUID_value rb 16
;!!!reserved 3                          ;a unique 128 bit number
		TimecodeScale	db 0x2A,0xD7,0xB1
				db (0x80 or 3)
					db 0x0F,0x42,0x40;this is 0xf4240=1 000 000,nanoseconds, so times are represented in milliseconds
		Duration	db 0x44,0x89
				db (0x80 or 4)
					rb 4
;!!!reserved 4                          ;4 bytes; duration is calculated at the end
		DateUTC 	db 0x44,0x61
				db (0x80 or 8)
		date_value_highs	rd 1
		date_value_lows 	rd 1
;!!!reserved 5                          ;nanoseconds relatively to Jan 01, 2001, 0:00:00 GMT
		MuxingApp	db 0x4D,0x80
				db (0x80 or 15)
					db 'Screen Recorder'
					;db 'SRecorder 1.101'
		WritingApp	db 0x57,0x41
				db (0x80 or 23);16)
					db 'Screen Recorder 1.1.0.2'
					;db 'S Recorder 1.101'
	Tracks	db 0x16,0x54,0xAE,0x6B
		stop_point1_default rw 1;(0x80 or 93)
;!!!stop point ;write functions stop here to calculate this value
													;other variables
													stop_point1_default_size rd 1
		TrackEntry	db 0xAE
				stop_point2_default rw 1;(0x80 or 91)
;!!!stop point
													;other variables
													stop_point2_default_size rd 1
			TrackNumber	db 0xD7
					db (0x80 or 1)
						db 1;the first track
			TrackUID	db 0x73,0xC5
					db (0x80 or 1)
						db 1;the unique ID is 1
			TrackType	db 0x83
					db (0x80 or 1)
						db 1	;1: video
			FlagLacing	db 0x9C
					db (0x80 or 1)
						db 0	;no lacing used
			Name		db 0x53,0x6E
					db (0x80 or 5)
						db "Video"
			CodecID 	db 0x86
					db (0x80 or 15)
						db "V_MS/VFW/FOURCC"
			CodecPrivate	db 0x63,0xA2
					stop_point3_default rw 1;dw (0x80 or 40); uncompressed is size of BITMAPINFOHEADER = 40
;!!!stop point                          ;Important remark! -> if the codec size is more than 40 it will be changed acordingly,possibly the same for trackentry size and tracks size
													;other variables
													stop_point3_default_size rd 1
						stop_point4_default rd 1
;!!!stop point                                  ;codec private data, uncompressed is pointing to BITMAPINFOHEADER
													;other variables
													stop_point4_default_size rd 1
			VideoTrack	db 0xE0
					db (0x80 or 8)
				PixelWidth	db 0xB0
						db (0x80 or 2)
				pixelwidth_size rw 1
;!!!reserved 6                                          ;for width
				PixelHeight	db 0xBA
						db (0x80 or 2)
				pixelheight_size rw 1
;!!!reserved 7                                          ;for height
	Cluster db 0x1F,0x43,0xB6,0x75
cluster_size1 dd 0x8;very big cluster
framestotalcounter rd 1
tfile_pointer_highs1 rd 1
tfile_pointer_lows1 rd 1
tfile_pointer_highs2 rd 1
tfile_pointer_lows2 rd 1
;;;;;;;;;;

;;;;;;;;;;capture5
framesnote rd 1
framesperinterval rd 1
frames_per_interval rd 1
storeraw rd 1
;;;;;;;;;;

flag rb 1;0 for unicde marker, 1 for no marker and 2 for capture8
buffer1 rd 1
buffer2 rd 1

frames rd 1;llquot
       rd 3;these bytes are unused but they form the lldiv structure


		Timecode	db 0xE7
		timecode_size	rd 1
;!!!variable point
			timecode_value rd 1
													;other variables
													cl_timecode rd 1
													cl_timecode_size rd 1
													cl_block_size rd 1
													clendflag rb 1
;!!!stop point
		SimpleBlock	db 0xA3
		simpleblock_size rd 1
													;other variables
													simpleblock_sizesize rd 1
													datawritesize rd 1
;!!!stop point
			Track_Number	 db (0x80 or 1)
			timecode_block dw 0 ;(relative to Cluster timecode, signed int16)
;!!!variable point
													;;vars
													frames_in_sec rb 1
													accumulatorsec rb 1
													closedsec rb 1
													currentsec rb 1
													;;
													localtimecode rd 1
													timecodedif rw 1
													timecoderest rb 1
													localtimes rw 800
													ltcursor rd 1
													;;
													timemin rb 1
													timemax rb 1
													;;

			flags db 0x80
;!!!variable point      1 byte for Flags:
;                               0 - Keyframe
;                               1-3 - Reserved, 0
;                               4   - Invisible, the codec should decode this frame but not display it
;                               5-6 - Lacing:
;                                       00 : no lacing
;                                       01 : Xiph lacing
;                                       11 : EBML lacing
;                                       10 : fixed-size lacing
;                               7 - Discardable, the frames of the Block can be discarded during playing if needed
;
;                               in this program will be used 0x80 for keyframes and 0x00 for P frames (if compression)
;
;!!!stop point         the raw frame is here
	Cues	db 0x1C,0x53,0xBB,0x6B
		cues_size rd 1 ;calculated at each cluster, unicode at the end
													;other variables
													temp_pointer_highs rd 1
													temp_pointer_lows rd 1
													cue_size rd 1
													cuespointer rd 1
;!!!stop point
		CuePoint	db 0xBB
		cuepoint_size rb 1
;!!!variable point
			CueTime db 0xB3
;!!!variable point and stop point              here is the timecode_size and timecode_value
				timecode_cues rb 1
				;timecode_value
;
			CueTrackPositions	db 0xB7
			cuetrackpositions_size	rb 1
;!!!variable point
				CueTrack	db 0xF7
						db (0x80 or 1)
							db 1
				CueClusterPosition	db 0xF1
				cueclusterposition_size rb 1
;!!!variable point
				cueclusterposition_value1 rd 1
				cueclusterposition_value2 rd 1
													;other variables
													cueshighssz rd 1
													cueslowssz rd 1
;!!!stop point



;;!!!reserved means that the value is placed there later in the program
;;!!!variable point means that there modifications will be but the write function can perform without stop if required
;;!!!stop point means that there modifications will be and the write function can stop there because the size will be variable