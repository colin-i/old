




format elfobj

include "./constants.h"

import "valinmem" valinmem

function getnexttime_value(sd ptr_nexttime,sd amount)
    sd value
    set value ptr_nexttime#

    sd x
    set x value
    div x amount
    mult x amount

    sub value x
    return value
endfunction

import "actions" actions

#e method 0,void 1
function wallpapers_action(sd method,sd arg1,sd arg2)
if method==0
    sd mem
    sd size
    set mem arg1
    set size arg2
    #set times
    data timesindex#1
    const ptr_timesindex^timesindex
    inc timesindex
    if timesindex>5
        return (noerror)
    endif

    data timevalue#1
    data morning_start#1
    data day_start#1
    data evening_start#1
    data night_start#1
    data UTCmidnight#1

    chars a_time#20
    ss time_str^a_time
    if size>19
        str impropertime="Unrecognized time value: "
        import "strvaluedisp" strvaluedisp
        import "cpymem" cpymem
        call cpymem(time_str,mem,9)
        sd time_str_cursor_data
        set time_str_cursor_data time_str
        add time_str_cursor_data 6
        str dots="..."
        data p_dots^dots
        set p_dots p_dots#
        set time_str_cursor_data# p_dots#
        call strvaluedisp(impropertime,time_str,(stringstring))
        return impropertime
    endif
    call cpymem(time_str,mem,size)
    ss time_str_cursor_string
    set time_str_cursor_string time_str
    add time_str_cursor_string size
    set time_str_cursor_string# 0

    import "strtoint_positive" strtoint_positive

    #tm structure
    import "_gmtime" gmtime
    import "_mktime" mktime
    sd tm
    sd tm_min
    sd tm_hr

    import "texter_warning" texter_warning

    #time
    if timesindex==1
        data ptr_t^timevalue
        sd bool
        setcall bool strtoint_positive(time_str,ptr_t)
        if bool==0
            return (error)
        endif

        str gmtimeerr="Wrong time value"
        #data tmsec=0
        data tmmin=4
        data tmhr=8

        #get UTCmidnight for taking new times
        sd UTCmidnight_val
        set UTCmidnight_val timevalue
        #get the next day time
        sd add=60*60*24
        add UTCmidnight_val add
        sd ptr_UTCmidnight_val^UTCmidnight_val
        setcall tm gmtime(ptr_UTCmidnight_val)
        if tm==0
            call texter_warning(gmtimeerr)
            return gmtimeerr
        endif
        sd tmcursor
        set tmcursor tm
        set tmcursor# 0
        add tmcursor 4
        set tmcursor# 0
        add tmcursor 4
        set tmcursor# 0
        setcall UTCmidnight mktime(tm)
        #adjust to local offset
        data ptr_UTCmidnight^UTCmidnight
        sd auxlocaltm
        setcall auxlocaltm gmtime(ptr_UTCmidnight)
        sd auxlocaltime
        setcall auxlocaltime mktime(auxlocaltm)
        sub auxlocaltime UTCmidnight
        sub UTCmidnight auxlocaltime

        #set the tm struct for times
        setcall tm gmtime(ptr_t)
        if tm==0
            call texter_warning(gmtimeerr)
            return gmtimeerr
        endif

        set tm# 0

        set tm_min tm
        add tm_min tmmin

        set tm_hr tm
        add tm_hr tmhr

        return (noerror)
    else
    #key times
        chars sep=":"
        sd sz

        setcall sz valinmem(time_str,size,sep)
        set time_str_cursor_string time_str
        add time_str_cursor_string sz
        set time_str_cursor_string# 0

        data H#1
        data M#1
        data ptr_H^H
        data ptr_M^M

        #H
        setcall bool strtoint_positive(time_str,ptr_H)
        if bool==0
            return (error)
        endif
        if H>=24
            str greatH="Hour value is not in [0,23] interval"
            call texter_warning(greatH)
            return greatH
        endif

        #M
        add time_str sz
        sub size sz
        if size==0
            str minutesexp="Minutes expected"
            call texter_warning(minutesexp)
            return minutesexp
        endif
        inc time_str
        dec size
        setcall bool strtoint_positive(time_str,ptr_M)
        if bool==0
            return (error)
        endif
        if M>=60
            str greatM="Minutes value is not in [0,59] interval"
            call texter_warning(greatM)
            return greatM
        endif

        set tm_hr# H
        set tm_min# M

        if timesindex==2
            setcall morning_start mktime(tm)
        elseif timesindex==3
            setcall day_start mktime(tm)
        elseif timesindex==4
            setcall evening_start mktime(tm)
        else
            setcall night_start mktime(tm)
        endelse

        return (noerror)
    endelse
elseif method==1
#0 for not relaunching the timeout
    if timevalue==UTCmidnight
        setcall bool actions(5)
        if bool==0
            import "_gtk_main_quit" gtk_main_quit
            call gtk_main_quit()
        endif
        return 0
    endif
    sd nextcall
    if timevalue<morning_start
        set nextcall morning_start
        call actions(6,0)
    elseif timevalue<day_start
        set nextcall day_start
        call actions(6,1)
    elseif timevalue<evening_start
        set nextcall evening_start
        call actions(6,2)
    elseif timevalue<night_start
        set nextcall night_start
        call actions(6,3)
    else
        set nextcall morning_start
        add nextcall (60*60*24)
        if timevalue<nextcall
            call actions(6,0)
        else
            setcall bool actions(5)
            if bool==0
                call gtk_main_quit()
            endif
            return 0
        endelse
    endelse
    if UTCmidnight<=nextcall
        set nextcall UTCmidnight
    endif

    sd aux
    set aux nextcall
    sub nextcall timevalue
    set timevalue aux

    #convert the seconds in milliseconds
    mult nextcall 1000

    data f^wallpapers_action
    import "_gdk_threads_add_timeout" gdk_threads_add_timeout
    call gdk_threads_add_timeout(nextcall,f,1)
    return 0
else
#method=2, next duration
    import "_time" time
    sd currenttime
    setcall currenttime time(0)

    sd nexttime
    sd ptr_nexttime^nexttime

    set nexttime timevalue
    sub nexttime currenttime

    import "texter" texter
    chars spacedata#100
    str spaces^spacedata
    str format_spc="%s: %u hours, %u minutes and %u seconds"

    sd sec
    setcall sec getnexttime_value(ptr_nexttime,60)

    sd min
    setcall min getnexttime_value(ptr_nexttime,(60*60))
    div min 60

    sd hr
    setcall hr getnexttime_value(ptr_nexttime,(60*60*24))
    div hr (60*60)

    ss trail
    if timevalue==morning_start
        ss trailmr="Civil Twilight Start"
        set trail trailmr
    elseif timevalue==day_start
        ss traild="Sun Rise"
        set trail traild
    elseif timevalue==evening_start
        ss trails="Sun Set"
        set trail trails
    elseif timevalue==night_start
        ss trailn="Civil Twiligth End"
        set trail trailn
    elseif timevalue==UTCmidnight
        ss trailmd="UTC Midnight"
        set trail trailmd
    else
        #next day civil twilight start aprox
        set trail trailmr
    endelse

    import "_sprintf" sprintf
    call sprintf(spaces,format_spc,trail,hr,min,sec)
    call texter(spaces,(GTK_MESSAGE_INFO))
endelse
endfunction

#e
function wallpapers_times(sd mem,sd size)
    sd err
    setcall err wallpapers_action(0,mem,size)
    return err
endfunction

#e
function iterate_mem_size_separator_forward(sd mem,sd size,ss separator,sd forward)
    sd bool=1
    sd err
    while bool!=0
        sd sz
        setcall sz valinmem(mem,size,separator#)
        setcall err forward(mem,sz)
        if err!=(noerror)
            return err
        endif
        add mem sz
        sub size sz
        if size==0
            set bool 0
        else
            inc mem
            dec size
        endelse
    endwhile
    return (noerror)
endfunction

#e
function wallpapers(sd body,sd size)
    data F^wallpapers_times
    str separator=","
    data p%ptr_timesindex
    set p# 0
    sd err
    setcall err iterate_mem_size_separator_forward(body,size,separator,F)
    if err!=(noerror)
        return err
    endif
    call wallpapers_action(1)
    return (noerror)
endfunction
