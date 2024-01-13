

;;EBML head
;;Segment
;;      seek head
;;              3rd entry
		seekcues_highs rd 1
		seekcues_lows rd 1
;;      segment info
		;;date
		dwordvalue rd 1
		multiplicator dd 1000000000;means 1 000 000 000nanoseconds per second
		mul1 rd 1
		mul2 rd 1
		substractionloss rd 1