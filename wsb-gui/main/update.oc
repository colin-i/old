

format elfobj

include "../include/wsb.h"

const NO_ERROR=0
const INVALID_SOCKET=-1
const SOCKET_ERROR=-1

const AF_INET=2
const SOCK_STREAM=1
const IPPROTO_TCP=6

const WSAEWOULDBLOCK=10035

import "WSAStartup" WSAStartup

import "texter" texter

function updates()
    sd pref
    setcall pref update_pref_get()
    if pref!=(TRUE)
        return (void)
    endif

    chars WSASTARTUP#400

    const majorversion=2
    const minorversion=2
    const wsaversion=minorversion*0x100|majorversion

    sd err
    setcall err WSAStartup((wsaversion),#WSASTARTUP)
    if err!=(NO_ERROR)
        call texter("WSAStartup error")
        return (void)
    endif

    call updates_main()

    import "WSACleanup" WSACleanup
    call WSACleanup()
endfunction

function updates_main()
    import "socket" socket
    import "closesocket" closesocket
    sd connectsocket
    setcall connectsocket socket((AF_INET),(SOCK_STREAM),(IPPROTO_TCP))
    if connectsocket==(INVALID_SOCKET)
        call texter("socket error")
        return (void)
    endif
    call updates_proc(connectsocket)
    call closesocket(connectsocket)
endfunction

function updates_proc(sd con_socket)
    #set the socket to async state, for implementing timeouts
    import "ioctlsocket" ioctlsocket
    sd nonblk=1
    const IOC_IN=0x80000000
    const IOCPARM_MASK=0x7f
    const _IOW_a=IOC_IN
    const _IOW_b=DWORD&IOCPARM_MASK*0x100*0x100
    const _IOW_c=f*0x100|126
    const FIONBIO=_IOW_a|_IOW_b|_IOW_c
    call ioctlsocket(con_socket,(FIONBIO),#nonblk)

    const FD_SETSIZE=64
    const fd_set_array=FD_SETSIZE*DWORD
    const fd_set_size=DWORD+fd_set_array

    const sin_start=!

    chars sin_family={AF_INET,0}
    chars *sin_port={0,80}
    chars *sin_addr={127,0,0,1}
    data *reserved={0,0}

    sd size=!-sin_start

    import "connect" connect
    import "select" select
    str timeout="connect timeout"
    str sel_err="select error"
    sd res
    setcall res connect(con_socket,#sin_family,size)
    if res==(SOCKET_ERROR)
        import "WSAGetLastError" WSAGetLastError
        sd err
        setcall err WSAGetLastError()
        if err!=(WSAEWOULDBLOCK)
            call texter("connect error")
            return (void)
        endif
        #wait some time for this internet call
        chars fd_set_write#fd_set_size
        chars fd_set_err#fd_set_size
        sd fd_write^fd_set_write
        sd fd_err^fd_set_err
        set fd_write# 1
        set fd_err# 1
        add fd_write (DWORD)
        add fd_err (DWORD)
        set fd_write# con_socket
        set fd_err# con_socket

        sd tv_sec=10
        sd *tv_usec=0
        sd tv^tv_sec
        setcall res select(0,0,#fd_set_write,#fd_set_err,#tv)
        if res==0
            call texter(timeout)
            return (void)
        endif
        import "__WSAFDIsSet" WSAFDIsSet
        sd is_set
        setcall is_set WSAFDIsSet(con_socket,#fd_set_write)
        if is_set==0
            call texter(sel_err)
            return (void)
        endif
    endif

    #send the request
    const req_start=!

    chars request="GET /programs/websitebuilder/version.txt HTTP/1.1"
    chars *={0xd,0xa}
    chars *="Host: www.oapplications.com"
    chars *={0xd,0xa,0xd,0xa}

    sd req_sz=!-req_start

    import "send" send
    setcall res send(con_socket,#request,req_sz,0)
    if res==(SOCKET_ERROR)
        call texter("send error")
    endif

    #recv with a proper internet timeout
    chars fd_set_recv#fd_set_size
    sd fd_recv^fd_set_recv
    set fd_recv# 1
    add fd_recv (DWORD)
    set fd_recv# con_socket

    sd tv_recv_sec=10
    sd *tv_recv_usec=0
    sd tv_recv^tv_recv_sec
    sd next
    set next con_socket
    inc next
    setcall res select(next,#fd_set_recv,0,0,#tv_recv)
    if res==0
        call texter(timeout)
        return (void)
    elseif res==(SOCKET_ERROR)
        call texter(sel_err)
        return (void)
    endelseif

    #data must be here, so do a normal recv()
    import "recv" recv
    const recv_length=400
    chars recvbuf#recv_length
    setcall res recv(con_socket,#recvbuf,(recv_length),0)
    if res>(recv_length-1)
        call texter("recv too many data")
        return (void)
    endif

    #get mesage version data
    ss buf^recvbuf
    add buf res
    set buf# 0
    while buf#!=0xa
        if res==0
            set buf# 0xa
        else
            dec res
            dec buf
        endelse
    endwhile
    inc buf

    #get local version data
    import "get_data_folder_file" get_data_folder_file
    str version="version.txt"
    ss fname
    setcall fname get_data_folder_file(version)
    import "file_get_content_forward_data" file_get_content_forward_data
    call file_get_content_forward_data(fname,update_compare,buf)
endfunction

const update_text_size=30

function update_compare(ss mem,sd size,ss netversion)
    import "strlen" strlen
    sd netversion_sz
    setcall netversion_sz strlen(netversion)
    if netversion_sz==size
        import "memcmp" memcmp
        sd result
        setcall result memcmp(mem,netversion,size)
        if result==0
            return (void)
        endif
    endif
    if netversion_sz>(update_text_size)
        call texter("Update unrecognized data")
        return (void)
    endif
    sd info
    setcall info update_info()
    set info# (TRUE)
    call update_text(netversion)
endfunction

function update_info()
    data info#1
    return #info
endfunction
#text
function update_text(sd newversion)
    chars text="New version available at site: "
    chars var_text#update_text_size
    chars *=0
    if newversion!=0
        #need to set the text
        import "memcpy" memcpy
        sd sz
        setcall sz strlen(newversion)
        inc sz
        call memcpy(#var_text,newversion,sz)
        return (void)
    endif
    #all texts are ready
    import "strcat" strcat
    call strcat(#text,#var_text)
    return #text
endfunction
#path/0
function update_pref_fname()
    str pref="update.data"
    ss fname
    setcall fname get_data_folder_file(pref)
    return fname
endfunction
#bool
function update_pref_get()
    ss fname
    setcall fname update_pref_fname()
    import "file_get_content_forward" file_get_content_forward
    sd bool
    setcall bool file_get_content_forward(fname,update_pref_got_file,fname)
    return bool
endfunction
function update_pref_got_file(sd mem,sd size)
    if size<(DWORD)
        call texter("update option size error")
        return (FALSE)
    endif
    return mem#
endfunction

function update_pref_set(sd bool)
    ss fname
    setcall fname update_pref_fname()
    import "file_open" file_open
    import "file_write" file_write
    import "fclose" fclose
    sd file
    setcall file file_open(fname,"wb")
    if file==0
        return (void)
    endif
    call file_write(file,#bool,(DWORD))
    call fclose(file)
endfunction
