


format elfobj

include "constants.h"

import "texter_warning" texter_warning

#err
Function memoryrealloc(data ptrpointer,data size)
        Data newpointer#1
        Data oldpointer#1
        Set oldpointer ptrpointer#

        import "_realloc" realloc
        SetCall newpointer realloc(oldpointer,size)
        Data null=NULL
        If newpointer==null
                Chars newmem="Realloc failed with error: "
                str pnewmem^newmem
                call strerrno(pnewmem)
                Return pnewmem
        EndIf
        Set ptrpointer# newpointer
        Data noerr=noerror
        Return noerr
EndFunction

#err
function memoryalloc(data size,data memptr)
        Data err#1
        Data null=NULL
        Set memptr# null
        SetCall err memoryrealloc(memptr,size)
        Return err
endfunction


###########dif
import "__errno" errno
#errno
function geterrno()
        data err#1
        setcall err errno()
        return err#
endfunction
function system_variables_alignment_pad(data value,data greatest)
    sub greatest value
    return greatest
endfunction
Import "_GetModuleFileNameA@12" GetModuleFileName
#err
function Scriptfullpath(data ptrfullpath)
    data MAX_PATH=260
    data null=0
    data err#1
    data noerr#1
    setcall err memoryalloc(MAX_PATH,ptrfullpath)
    if err!=noerr
        return err
    endif
    data size#1
    setcall size GetModuleFileName(null,ptrfullpath#,MAX_PATH)
    if size==null
        str e="Error."
        call texter_warning(e)
        return e
    endif
    return noerr
endfunction
###########dif

#e
function slen_s(ss str,sd size,sd ptrszout)
    Chars term={0}
    Chars byte={0}
    Data zero=0
    data one=1
    sd loop

    set ptrszout# size
    set loop one
    while loop==one
        if size==zero
            set loop zero
        else
            Set byte str#
            if byte==term
                set loop zero
            else
                Inc str
                dec size
            endelse
        endelse
    endwhile
    sub ptrszout# size
    if size==zero
        str er="String null termination expected."
        call texter_warning(er)
        return er
    endif
    data ne=noerror
    return ne
endfunction

#sizeof the string
function slen(str str)
        data maxsize=0xffFFffFF
        sd sz
        sd ptrsz^sz
        call slen_s(str,maxsize,ptrsz)
        return sz
endfunction

#void
function strvaluedisp(str text,data part2,data formattype)
    str int="%s%i"
    str uint="%s%u"
    str str="%s%s"

    data si=stringinteger
    data su=stringUinteger

    const infomax=200
    chars disp#infomax
    str display^disp

    data max=infomax
    data truncation#1
    data notruncation=0
    data dword=4
    chars truncdata="..."
    data truncdots^truncdata

    set truncation notruncation

    str format#1
    if formattype==si
        set format int
    elseif formattype==su
        set format uint
    else
        set format str

        data sz#1
        setcall sz slen(text)
        addcall sz slen(part2)
        if sz>=max
            set truncation display
            add truncation max
            sub truncation dword
        endif
    endelse

    import "__snprintf" snprintf
    call snprintf(display,max,format,text,part2)

    if truncation!=notruncation
        set truncation# truncdots#
    endif

    call texter_warning(display)
endfunction

function strerrno(str text)
        data err#1
        setcall err geterrno()
        data si=stringinteger
        call strvaluedisp(text,err,si)
endfunction





#e
import "__chdir" chdir
function dirch(str value)
    data change#1
    data zero=0
    setcall change chdir(value)
    if change!=zero
        str chdirerr="Change dir err: "
        call strerrno(chdirerr)
        return chdirerr
    endif
    data noerr=noerror
    return noerr
endfunction




import "_free" free

#true if match or false
Function filepathdelims(chars chr)
        Chars bslash="\\"
        Chars slash="/"
        Data true=TRUE
        Data false=FALSE
        If chr==bslash
                Return true
        EndIf
        If chr==slash
                Return true
        EndIf
        Return false
EndFunction

#folders ('c:\folder\file.txt' will be pointer starting at 'file.txt')
Function endoffolders(str path)
        Data strsize#1
        Str max#1
        Str cursor#1
        Str endfolders#1
        Data bool=0
        Data one=1
        Data true=TRUE

        SetCall strsize slen(path)
        Set max path
        Add max strsize
        Sub max one

        Set endfolders path
        Sub endfolders one

        Set cursor max

        While endfolders<cursor
                Chars chr={0}
                Set chr cursor#
                SetCall bool filepathdelims(chr)
                If bool==true
                        Set endfolders cursor
                EndIf
                If endfolders<cursor
                        Sub cursor one
                EndIf
        EndWhile
        Add endfolders one
        Return endfolders
EndFunction

#void/err
function movetoScriptfolder()
    data path#1
    data ptrpath^path
    data err#1
    data noerr=noerror
    setcall err Scriptfullpath(ptrpath)
    if err!=noerr
        return err
    endif

    data pointer#1
    chars z=0
    setcall pointer endoffolders(path)
    set pointer# z

    setcall err dirch(path)
    if err!=noerr
        return err
    endif

    call free(path)
endfunction









#err
Function openfile(data pfile,str path,str fmode)
        data null=0
        import "_fopen" fopen
        SetCall pfile# fopen(path,fmode)
        If pfile#==null
            str err="Cannot open a file. Error: "
            call strerrno(err)
            return err
        EndIf
        Data noerr=noerror
        Return noerr
EndFunction

##forward simple

#err/returns what forward returns
function file_forward(ss path,sd method,sd forward)
    sd returnvalue
    data noerr=noerror
    sd file
    sd ptrfile^file
    SetCall returnvalue openfile(ptrfile,path,method)
    If returnvalue!=noerr
            Return returnvalue
    EndIf
    setcall returnvalue forward(file)
    import "_fclose" fclose
    call fclose(file)
    return returnvalue
EndFunction


function getreadmode()
    str read="rb"
    return read
endfunction

function getwritemode()
    str write="wb"
    return write
endfunction

#err
function file_length(sd file,sd ptr_size)
    Data zero=0
    Data seek_end=SEEK_END
    data seekint#1
    import "_fseek" fseek
    SetCall seekint fseek(file,zero,seek_end)
    If seekint!=zero
        str endseekerr="File seek error: "
        call strerrno(endseekerr)
        return endseekerr
    endif

    import "_ftell" ftell
    setcall ptr_size# ftell(file)
    data INVALID_HANDLE=INVALID_HANDLE
    if ptr_size#==INVALID_HANDLE
        str ftellerr="File tell error: "
        call strerrno(ftellerr)
        return ftellerr
    endif
    return (noerror)
endfunction

#read
#err
function readtomem(data file,data ptrsize,data ptrmem,data offset,data trail)
    sd size
    sd ptr_size^size
    sd err
    setcall err file_length(file,ptr_size)
    if err!=(noerror)
        return err
    endif

    #seek back for reading
    Call fseek(file,0,(SEEK_SET))

    Set ptrsize# size

    add size offset
    add size trail

    data noerr=noerror
    SetCall err memoryalloc(size,ptrmem)
    If err!=noerr
        return err
    endif

    Data mem#1
    Set mem ptrmem#

    add mem offset

    data byte=1
    import "_fread" fread
    Call fread(mem,byte,ptrsize#,file)

    return noerr
EndFunction

#err
#get: offset+size+trail
Function file_get_content_offset_trail(str path,data ptrsize,data ptrmem,data offset,data trail)
        Data err#1
        Data noerr=noerror

        Data file#1
        Data ptrfile^file

        str readfile#1
        setcall readfile getreadmode()
        SetCall err openfile(ptrfile,path,readfile)
        If err!=noerr
                Return err
        EndIf

        setcall err readtomem(file,ptrsize,ptrmem,offset,trail)
        Call fclose(file)
        Return err
EndFunction

#err
function file_get_content(str path,data ptrsize,data ptrmem)
    data err#1
    data null=0
    setcall err file_get_content_offset_trail(path,ptrsize,ptrmem,null,null)
    return err
EndFunction

function file_get_content_forward(ss filename,sd forward)
    sd err
    sd noerr=noerror

    sd size
    sd ptr_size^size
    sd mem
    sd ptr_mem^mem
    setcall err file_get_content(filename,ptr_size,ptr_mem)
    if err!=noerr
        return err
    endif
    call forward(mem,size)

    call free(mem)
endfunction


##write

#e
function file_write(sd buffer,sd size,sd file)
    data z=0
    data noe=noerror
    if size==z
        return noe
    endif
    data byte=1
    sd sizewrote
    import "_fwrite" fwrite
    setcall sizewrote fwrite(buffer,byte,size,file)
    if sizewrote!=z
        return noe
    endif
    str er="File write error number: "
    call strerrno(er)
    return er
endfunction

function file_write_forward(ss file,sd forward)
    sd method
    setcall method getwritemode()
    call file_forward(file,method,forward)
endfunction

import "mainwidget" mainwidget
import "_gtk_file_chooser_dialog_new" gtk_file_chooser_dialog_new
import "_gtk_dialog_run" gtk_dialog_run
function filechooserfield_forward_data(sd forward,sd data)
    str sel_file="Select File"
    sd main
    setcall main mainwidget()
    data open=GTK_FILE_CHOOSER_ACTION_OPEN
    str GTK_STOCK_CANCEL="gtk-cancel"
    data responsecancel=GTK_RESPONSE_CANCEL
    str GTK_STOCK_OPEN="gtk-open"
    data responseaccept=GTK_RESPONSE_ACCEPT
    data null=0
    sd dialog
    setcall dialog gtk_file_chooser_dialog_new(sel_file,main,open,GTK_STOCK_CANCEL,responsecancel,GTK_STOCK_OPEN,responseaccept,null)
    sd resp
    setcall resp gtk_dialog_run(dialog)
    if resp==responseaccept
        ss file
        import "_gtk_file_chooser_get_filename_utf8" gtk_file_chooser_get_filename
        setcall file gtk_file_chooser_get_filename(dialog)
        if file!=null
            call forward(file,data)
            import "_g_free" g_free
            call g_free(file)
        endif
    endif
    import "_gtk_widget_destroy" gtk_widget_destroy
    call gtk_widget_destroy(dialog)
endfunction
