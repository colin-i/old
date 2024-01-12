
format elfobj

#bool numeric
Function numeric(chars c)
        Chars zero="0"
        Chars nine="9"
        Data false=0
        Data true=1
        If c<zero
                Return false
        ElseIf c>nine
                Return false
        EndElseIf
        Return true
EndFunction

#bool
Function memtoint(str content,data size,data outvalue)
        if size==0
            return 0
        endif

        Data positive=1
        Data negative=-1
        Data sign=1
        Data zero=0
        Chars negsign="-"
        Chars byte={0}

        Set sign positive
        Set byte content#
        If byte==negsign
                Set sign negative
                Inc content
                Dec size
                If size==zero
                        Return zero
                EndIf
        EndIf

        Data value#1
        Data coef#1
        Data bool#1
        Chars asczero="0"

        Set value zero
        Set coef zero
        Add content size
            While size!=zero
                Dec content
                Set byte content#
                SetCall bool numeric(byte)
                If bool==zero
                        Return zero
                EndIf
                Sub byte asczero

                Data number#1
                Set number byte

                Data multp#1
                Data newnr#1
                Data addtimes=10

                Set multp coef
                While multp!=zero
                        Set newnr number
                        mult newnr addtimes
                        #3 xxx xxx xxx xxx
                        If newnr<number
                                #set the out value in case a dword
                                #between 0x80..00 and 0xff..ff is expected
                                                                add value newnr
                                Set outvalue# value
                                Return zero
                        EndIf
                        Set number newnr
                        Dec multp
                EndWhile
                Data vlNr#1
                Set vlNr value
                Add value number
                If value<vlNr
                #overflow
                        If sign==positive
                            #set the out value in case a dword
                            #between 0x80..00 and 0xff..ff is expected
                            Set outvalue# value
                            Return zero
                        EndIf
                        #check for 0x80..00,-2147483648
                        Dec vlNr
                        Add vlNr number
                        If vlNr<zero
                                return zero
                        EndIf
                EndIf
                Inc coef
                Dec size
        EndWhile
        If sign==negative
                #check for 0x80..00,-2147483648
                If value>zero
                        Data negvalue=0
                        Set negvalue value
                        Sub value negvalue
                        Sub value negvalue
                EndIf
        EndIf
        Set outvalue# value
        Return positive
EndFunction

import "slen" slen

#bool
function strtoint(sd str,sd ptrout)
    sd sz
    setcall sz slen(str)
    sd bool
    setcall bool memtoint(str,sz,ptrout)
    return bool
endfunction

import "texter_warning" texter_warning

#bool
function strtoint_positive(ss str,sd ptr_out)
    sd bool
    setcall bool strtoint(str,ptr_out)
    str posint="Positive integer number expected"
    if bool==0
        call texter_warning(posint)
        return 0
    endif
    if ptr_out#<0
        call texter_warning(posint)
        return 0
    endif
    return 1
endfunction

#bool
function strtoint_positive_minutes_to_decimal(ss str,sd ptr_out)
    sd bool
    setcall bool strtoint_positive(str,ptr_out)
    if bool==0
        return 0
    endif
    str minutesexpected="Minutes number from [0,59] expected"
    if ptr_out#>=60
        call texter_warning(minutesexpected)
        return 0
    endif
    mult ptr_out# 100
    div ptr_out# 60
    return 1
endfunction
