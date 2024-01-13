

format PE console
entry start

include '..\INCLUDE\WIN32A.INC'

;======================================
section '.data' data readable writeable
;======================================

include 'include\exe\data.inc'

;=======================================
section '.code' code readable executable
;=======================================

include 'include\exe\code.inc'

;====================================
section '.idata' import data readable
;====================================

include 'include\exe\idata.inc'

;====================================
section '.rsrc' resource data readable
;====================================

include 'include\exe\rsrc.inc'