

format elfobj

include "../include/wsb.h"

function open_last()
    import "proj_work_with_all_projects" proj_work_with_all_projects
    call proj_work_with_all_projects()
    import "file_get_content_forward" file_get_content_forward
    ss name
    setcall name get_recent_path()
    data f^open_last_com
    call file_get_content_forward(name,f)
endfunction
import "texter" texter
function open_last_com(sd mem,sd size)
    import "slen_s" slen_s
    sd len
    sd bool
    setcall bool slen_s(mem,size,#len)
    if bool!=(TRUE)
        str n="Nothing to open"
        call texter(n)
        return (void)
    endif
    ss folder_name
    set folder_name mem
    inc len
    add mem len
    sub size len
    setcall bool slen_s(mem,size,#len)
    if bool!=(TRUE)
        str er="Error at open"
        call texter(er)
        return (void)
    endif
    ss name_string
    set name_string mem
    import "proj_set" proj_set
    ss filename
    setcall filename proj_set(folder_name,name_string)
    if filename==0
        return (void)
    endif
    import "file_get_content_forward_data" file_get_content_forward_data
    import "proj_read_file" proj_read_file
    call file_get_content_forward_data(filename,proj_read_file,name_string)
endfunction

function recent_file()
    str path="recent.data"
    return path
endfunction
import "data_folder" data_folder
import "get_folder_fname" get_folder_fname
#path/0
function get_data_folder_file(ss filename)
    ss d
    setcall d data_folder()
    ss name
    setcall name get_folder_fname(d,filename)
    return name
endfunction
#path/0
function get_recent_path()
    ss r
    setcall r recent_file()
    ss name
    setcall name get_data_folder_file(r)
    return name
endfunction
import "strlen" strlen
import "file_write" file_write
import "fclose" fclose
function write_last_site_opened(sd paths)
    ss folder
    ss projname
    set folder paths#
    add paths (DWORD)
    set projname paths#
    import "file_open" file_open
    ss name
    setcall name get_recent_path()
    ss w="wb"
    sd file
    setcall file file_open(name,w)
    if file==0
        return (void)
    endif
    sd len
    setcall len strlen(folder)
    inc len
    sd bool
    setcall bool file_write(file,folder,len)
    if bool==(TRUE)
        setcall len strlen(projname)
        inc len
        call file_write(file,projname,len)
    endif
    call fclose(file)
endfunction
