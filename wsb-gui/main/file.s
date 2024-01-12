

format elfobj

include "../include/wsb.h"

import "texter" texter
import "sprintf" sprintf
import "errno_disp" errno_disp

import "fclose" fclose
#file/0
function file_open(ss filepath,ss method)
    import "fopen" fopen
    sd file
    setcall file fopen(filepath,method)
    If file==0
        chars info#MAX_PATH+safe_chars_to_add
        chars *=0
        call sprintf(#info,"Cannot open a file: %s",filepath)
        call texter(#info)
    EndIf
    return file
endfunction

#bool
function file_write(sd file,sd buffer,sd size)
    if size==0
        return (TRUE)
    endif
    import "fwrite" fwrite
    sd len
    setcall len fwrite(buffer,1,size,file)
    if len!=0
        return (TRUE)
    endif
    str er="File write error number"
    call texter(er)
    return (FALSE)
endfunction
import "free" free
import "memalloc" memalloc

#forward/FALSE
function file_get_content_forward_data(ss filename,sd forward,sd data)
    ss r="rb"
    sd file
    setcall file file_open(filename,r)
    if file==0
        return (FALSE)
    endif
    sd bool
    setcall bool file_seek(file,0,(SEEK_END))
    if bool!=(TRUE)
        call fclose(file)
        return (FALSE)
    endif
    sd len
    setcall len file_tell(file)
    if len==-1
        call fclose(file)
        return (FALSE)
    endif
    setcall bool file_seek(file,0,(SEEK_SET))
    if bool!=(TRUE)
        call fclose(file)
        return (FALSE)
    endif
    sd mem
    setcall mem memalloc(len)
    if mem==0
        call fclose(file)
        return (FALSE)
    endif
    setcall bool file_read(file,mem,len)
    if bool!=(TRUE)
        call free(mem)
        call fclose(file)
        return (FALSE)
    endif
    sd result
    setcall result forward(mem,len,data)
    call free(mem)
    call fclose(file)
    return result
endfunction
#forward
function file_get_content_forward(ss filename,sd forward)
    sd result
    setcall result file_get_content_forward_data(filename,forward,0)
    return result
endfunction

#bool
function file_seek(ss file,sd off,sd method)
    import "fseek" fseek
    sd seekint
    setcall seekint fseek(file,off,method)
    If seekint!=0
        str seekerr="File seek error"
        call texter(seekerr)
        return (FALSE)
    endif
    return (TRUE)
endfunction
#sz/-1
function file_tell(sd file)
    import "ftell" ftell
    sd sz
    setcall sz ftell(file)
    if sz==-1
        str ftellerr="File tell error"
        call texter(ftellerr)
    endif
    return sz
endfunction
#bool
function file_read(sd file,sd mem,sd size)
    import "fread" fread
    sd read
    setcall read fread(mem,1,size,file)
    if read!=size
        str er="Read error"
        call errno_disp(er)
        return (FALSE)
    endif
    return (TRUE)
endfunction
import "dialog_message_format" dialog_message_format
import "_get_errno" get_errno
#bool
function mkdir_message_on_exist(ss dirname)
    import "_mkdir" mkdir
    sd int
    setcall int mkdir(dirname)
    if int==0
        return (TRUE)
    endif
    sd errno
    call get_errno(#errno)
    if errno==(EEXIST)
        sd bool
        setcall bool dialog_message_format("Folder '%s' already exists.Use this folder?",dirname)
        return bool
    endif
    return (FALSE)
endfunction

#file/0
function file_open_overwrite(ss filename)
    sd file
    sd bool=TRUE
    setcall file fopen(filename,"r")
    if file!=0
        call fclose(file)
        setcall bool dialog_message_format("File '%s' already exists.Overwrite?",filename)
    else
        setcall bool file_open_overwrite_test(file)
    endelse
    if bool==(FALSE)
        return 0
    endif
    setcall file file_open(filename,"wb")
    return file
endfunction
#bool
function file_open_overwrite_test()
    sd errno
    call get_errno(#errno)
    if errno==(ENOENT)
        return (TRUE)
    endif
    return (FALSE)
endfunction
