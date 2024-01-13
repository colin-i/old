

format PE console
entry start

include '..\INCLUDE\WIN32A.INC'

;======================================
section '.data' data readable writeable
;======================================

include 'include\data.inc'

;=======================================
section '.code' code readable executable
;=======================================

include 'include\code.inc'

;====================================
section '.idata' import data readable
;====================================

include 'include\idata.inc'