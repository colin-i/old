


format elfobj

include "constants.h"

function content_size(sd ptrdata1,sd ptrdata2)
    set ptrdata2# ptrdata1#
    data dw=4
    add ptrdata1 dw
    add ptrdata2 dw
    set ptrdata2# ptrdata1#
endfunction

#return neg(nr)
Function neg(data nr)
        Data negative#1
        Set negative nr
        Sub nr negative
        Sub nr negative
        Return nr
EndFunction

#advance the content/size by value
Function move_cursors(sd str_sz,data nr)
    sd mem
    sd size

    sd ptrdata^mem

    call content_size(str_sz,ptrdata)

    Add mem nr

    #backward advance
    #take nr if nr>0 or -nr if nr<0
    Data zero=0
    If nr<zero
        SetCall nr neg(nr)
    EndIf

    Sub size nr
    call content_size(ptrdata,str_sz)
EndFunction


#void
function cpymem(str dest,str src,data size)
    data zero=0
    while size!=zero
            set dest# src#
            inc dest
            inc src
            dec size
    endwhile
endfunction

import "texter_warning" texter_warning

#e
function cpymem_safesrc_advance(sd dest,sd src_sz,sd size)
    sd src
    sd sz
    sd srcsz^src
    call content_size(src_sz,srcsz)
    if sz<size
        str cpysafesrcerr="Error with the length of some data."
        call texter_warning(cpysafesrcerr)
        return cpysafesrcerr
    endif
    call cpymem(dest,src,size)
    call move_cursors(src_sz,size)
    data noerr=noerror
    return noerr
endfunction

#e
function get_mem_int_advance(sd dest,sd src_sz)
    sd er
    data a=4
    setcall er cpymem_safesrc_advance(dest,src_sz,a)
    return er
endfunction



#e
function getvar(sd var,sd str_sz)
    data mem#1
    data size#1

    sd ptrdata^mem
    call content_size(str_sz,ptrdata)

    import "slen_s" slen_s
    sd sz
    sd ptrsz^sz
    sd err
    sd noerr=noerror

    setcall err slen_s(mem,size,ptrsz)
    if err!=noerr
        return err
    endif
    set var# mem

    inc sz
    call move_cursors(str_sz,sz)

    return noerr
endfunction

function get_str_advance(sd ptr_str,sd max_size,sd str_sz)
    sd var
    sd ptr_var^var
    sd err
    setcall err getvar(ptr_var,str_sz)
    if err!=(noerror)
        return err
    endif
    sd sz

    import "slen" slen
    setcall sz slen(var)
    inc sz
    if sz>max_size
        str sizeerr="String length too great."
        call texter_warning(sizeerr)
        return sizeerr
    endif
    call cpymem(ptr_str,var,sz)
    return (noerror)
endfunction

##valinmem

#return the size of the value, if the delim is found the size counts until there
Function valinmemsens(str content,data size,chars delim,data sens)
        Data length#1
        Set length size
        Chars byte#1
        Data zero=0

        If size==zero
                Return size
        EndIf
        Data backward=BACKWARD
        If sens==backward
                Dec content
        EndIf
        Set byte content#
        While byte!=delim
                If sens!=backward
                        Inc content
                Else
                        Dec content
                EndElse
                Dec size
                If size==zero
                        Set byte delim
                Else
                        Set byte content#
                EndElse
        EndWhile

        Sub length size
        Return length
EndFunction

#return valueinmem
Function valinmem(str content,data size,chars delim)
        Data returnvalue#1
        Data forward=FORWARD
        SetCall returnvalue valinmemsens(content,size,delim,forward)
        Return returnvalue
Endfunction
