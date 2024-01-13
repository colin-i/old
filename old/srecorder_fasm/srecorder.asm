

format PE GUI 4.0 DLL
entry DllEntryPoint

include '..\INCLUDE\WIN32A.INC'

;======================================
section '.data' data readable writeable
;======================================

include 'include\dll\data.inc'

;=======================================
section '.code' code readable executable
;=======================================

include 'include\dll\code.inc'

;====================================
section '.edata' export data readable
;====================================

include 'include\dll\edata.inc'

;====================================
section '.idata' import data readable
;====================================

include 'include\dll\idata.inc'

;====================================
section '.rsrc' resource data readable
;====================================

include 'include\dll\rsrc.inc'

;====================================
section '.reloc' fixups data discardable readable
;====================================