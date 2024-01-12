


format elfobj

include "constants.h"

function pack_default(sd box,sd widget)
    import "_gtk_box_pack_start" gtk_box_pack_start
    call gtk_box_pack_start(box,widget,0,0,0)
endfunction

import "_gtk_container_add" gtk_container_add

function bordered(sd wid)
    import "_gtk_frame_new" gtk_frame_new
    sd frame
    setcall frame gtk_frame_new(0)
    call gtk_container_add(frame,wid)
    return frame
endfunction

import "labelfield_left_prepare" labelfield_left_prepare

import "_gtk_hbox_new" gtk_hbox_new

function containered(sd entry,ss text)
    sd packet
    setcall packet gtk_hbox_new(0,0)
    call gtk_container_add(packet,entry)
    sd t
    setcall t labelfield_left_prepare(text)
    call gtk_container_add(packet,t)
    setcall packet bordered(packet)
    return packet
endfunction

function geticon()
    str icon="files/img.png"
    return icon
endfunction

import "_gtk_entry_set_text" gtk_entry_set_text

function clicked_forward(sd filename,sd entry)
    call gtk_entry_set_text(entry,filename)
endfunction
function file_sel_clicked(sd *button,sd data)
    data f^clicked_forward
    import "filechooserfield_forward_data" filechooserfield_forward_data
    call filechooserfield_forward_data(f,data)
endfunction

data allok#1
const ptr_allok^allok

#0 init
#1 read
#2 fields
#3 write
#4 request prepare
#5 send
#6 set wallpaper

function actions(sd mod,sd arg1,sd arg2)
    data lat_zone#1
    data lat_degrees#1
    data lat_minutes#1
    data lg_zone#1
    data lg_degrees#1
    data lg_minutes#1
    data off_zone#1
    data off_hours#1
    data twilight_start#1
    data sun_rise#1
    data sun_set#1
    data twilight_end#1
    data uri#1
    data skipvalue#1
    const ptr_skipvalue^skipvalue

    data p_lat_zone^lat_zone
    data p_lg_zone^lg_zone
    data p_off_zone^off_zone

    data ptr_allok%ptr_allok
#initialize default values
if mod==0
    str default_coord="1"
    set lat_zone 0
    set lat_degrees default_coord
    set lat_minutes default_coord
    set lg_zone 1
    set lg_degrees default_coord
    set lg_minutes default_coord
    set off_zone 1
    set off_hours default_coord
    str ts_def="morning.png"
    set twilight_start ts_def
    str sr_def="day.png"
    set sun_rise sr_def
    str ss_def="evening.png"
    set sun_set ss_def
    str te_def="night.png"
    set twilight_end te_def
    str uri_def="http://oa.netau.net/sun.php"
    set uri uri_def
    set skipvalue 0
    return 1
endif
#read values from file
if mod==1
    sd mem
    sd size
    set mem arg1
    set size arg2
    sd mem_sz^mem
    sd err
    import "get_mem_int_advance" get_mem_int_advance
    import "get_str_advance" get_str_advance
    const size_of_degrees_lat=2+1
    const size_of_degrees_long=3+1
    const size_of_minutes=2+1
    const size_of_hours=2+1
    data minutes_size=size_of_minutes

    #lat
    setcall err get_mem_int_advance(p_lat_zone,mem_sz)
    if err!=(noerror)
        return 0
    endif

    chars lat_degrees_chars#size_of_degrees_lat
    data lat_degrees_size=size_of_degrees_lat
    str lat_degrees_str^lat_degrees_chars
    setcall err get_str_advance(lat_degrees_str,lat_degrees_size,mem_sz)
    if err!=(noerror)
        return 0
    endif
    set lat_degrees lat_degrees_str

    chars lat_minutes_chars#size_of_minutes
    str lat_minutes_str^lat_minutes_chars
    setcall err get_str_advance(lat_minutes_str,minutes_size,mem_sz)
    if err!=(noerror)
        return 0
    endif
    set lat_minutes lat_minutes_str

    #long
    setcall err get_mem_int_advance(p_lg_zone,mem_sz)
    if err!=(noerror)
        return 0
    endif

    chars lg_degrees_chars#size_of_degrees_long
    data lg_degrees_size=size_of_degrees_long
    str lg_degrees_str^lg_degrees_chars
    setcall err get_str_advance(lg_degrees_str,lg_degrees_size,mem_sz)
    if err!=(noerror)
        return 0
    endif
    set lg_degrees lg_degrees_str

    chars lg_minutes_chars#size_of_minutes
    str lg_minutes_str^lg_minutes_chars
    setcall err get_str_advance(lg_minutes_str,minutes_size,mem_sz)
    if err!=(noerror)
        return 0
    endif
    set lg_minutes lg_minutes_str

    #timezone
    setcall err get_mem_int_advance(p_off_zone,mem_sz)
    if err!=(noerror)
        return 0
    endif

    chars off_hours_chars#size_of_hours
    str off_hours_str^off_hours_chars
    data size_of_hours=size_of_hours
    setcall err get_str_advance(off_hours_str,size_of_hours,mem_sz)
    if err!=(noerror)
        return 0
    endif
    set off_hours off_hours_str

    #wallpapers
    chars morning_chars#MAX_PATH
    str morning_str^morning_chars
    setcall err get_str_advance(morning_str,(MAX_PATH),mem_sz)
    if err!=(noerror)
        return 0
    endif
    set twilight_start morning_str
    #
    chars day_chars#MAX_PATH
    str day_str^day_chars
    setcall err get_str_advance(day_str,(MAX_PATH),mem_sz)
    if err!=(noerror)
        return 0
    endif
    set sun_rise day_str
    #
    chars evening_chars#MAX_PATH
    str evening_str^evening_chars
    setcall err get_str_advance(evening_str,(MAX_PATH),mem_sz)
    if err!=(noerror)
        return 0
    endif
    set sun_set evening_str
    #
    chars night_chars#MAX_PATH
    str night_str^night_chars
    setcall err get_str_advance(night_str,(MAX_PATH),mem_sz)
    if err!=(noerror)
        return 0
    endif
    set twilight_end night_str
    ##
    const MAX_URI=512
    chars uri_chars#MAX_URI
    str uri_str^uri_chars
    setcall err get_str_advance(uri_str,(MAX_URI),mem_sz)
    if err!=(noerror)
        return 0
    endif
    set uri uri_str
    #skipvalue
    data p_skipvalue^skipvalue
    setcall err get_mem_int_advance(p_skipvalue,mem_sz)
    if err!=(noerror)
        return 0
    endif

    #read ok
    set ptr_allok# 1
    return 1
endif
#values to fields
if mod==2
    sd vbox
    sd dialog
    set vbox arg1
    set dialog arg2
##icon
    import "setwndicon" setwndicon
    ss icon
    setcall icon geticon()
    call setwndicon(dialog,icon)
##body
    import "tablefield_cells" tablefield_cells
    data rows=2
    data cols=5

    str degrees="Degrees"
    str mn="Minutes"

    data lat_radio#1
    data lat_radio_n#1
    data lat_radio_s#1

    data lat_d#1
    data lat_m#1

    data lg_radio#1
    data lg_radio_n#1
    data lg_radio_s#1

    data lg_d#1
    data lg_m#1

    data off_radio#1
    data off_radio_e#1
    data off_radio_w#1

    data off_h#1

    sd lat_t
    sd latp1
    sd latp2
    sd latp3
    sd latp4
    sd lg_t
    sd lgp1
    sd lgp2
    sd lgp3
    sd lgp4
    sd ptr_cells^lat_t


    import "_gtk_radio_button_new_with_label" gtk_radio_button_new_with_label
    import "_gtk_radio_button_get_group" gtk_radio_button_get_group
    import "_gtk_entry_new" gtk_entry_new

###Latitude
    str lat_text="Latitude"
    str lat_N="North"
    str lat_S="South"

    setcall lat_t labelfield_left_prepare(lat_text)

    setcall lat_radio_n gtk_radio_button_new_with_label(0,lat_N)
    setcall lat_radio gtk_radio_button_get_group(lat_radio_n)
    setcall latp1 bordered(lat_radio_n)
    setcall lat_radio_s gtk_radio_button_new_with_label(lat_radio,lat_S)
    setcall latp2 bordered(lat_radio_s)

    setcall lat_d gtk_entry_new()
    setcall latp3 containered(lat_d,degrees)
    setcall lat_m gtk_entry_new()
    setcall latp4 containered(lat_m,mn)

###Longitude
    str lg_text="Longitude"
    str lg_E="East"
    str lg_W="West"

    setcall lg_t labelfield_left_prepare(lg_text)

    setcall lg_radio_n gtk_radio_button_new_with_label(0,lg_E)
    setcall lgp1 bordered(lg_radio_n)
    setcall lg_radio gtk_radio_button_get_group(lg_radio_n)
    setcall lg_radio_s gtk_radio_button_new_with_label(lg_radio,lg_W)
    setcall lgp2 bordered(lg_radio_s)

    setcall lg_d gtk_entry_new()
    setcall lgp3 containered(lg_d,degrees)
    setcall lg_m gtk_entry_new()
    setcall lgp4 containered(lg_m,mn)

    sd table
    setcall table tablefield_cells(vbox,rows,cols,ptr_cells)

###Timezone
    import "_gtk_table_resize" gtk_table_resize
    #is 4 because gtk_table_new(0..=>1,add 2 more=>3, then 4
    call gtk_table_resize(table,4,cols)

    str off="Timezone"
    str off_e="East of GM"
    str off_w="West of GM"
    str off_H="Hours"

    import "_gtk_table_attach_defaults" gtk_table_attach_defaults
    sd wd
    setcall wd labelfield_left_prepare(off)
    call gtk_table_attach_defaults(table,wd,0,1,3,4)

    setcall off_radio_e gtk_radio_button_new_with_label(0,off_e)
    setcall wd bordered(off_radio_e)
    call gtk_table_attach_defaults(table,wd,1,2,3,4)
    setcall off_radio gtk_radio_button_get_group(off_radio_e)

    setcall off_radio_w gtk_radio_button_new_with_label(off_radio,off_w)
    setcall wd bordered(off_radio_w)
    call gtk_table_attach_defaults(table,wd,2,3,3,4)

    setcall off_h gtk_entry_new()
    setcall wd containered(off_h,off_H)
    call gtk_table_attach_defaults(table,wd,3,4,3,4)
#
    import "_gtk_table_set_col_spacings" gtk_table_set_col_spacings
    call gtk_table_set_col_spacings(table,10)

    import "_gtk_hseparator_new" gtk_hseparator_new
    setcall wd gtk_hseparator_new()
    call gtk_container_add(vbox,wd)
#
###wallpapers and script
    str ctwlightstart_text="Civil twiligth start wallpaper"
    str sunrise_text="Sunrise wallpaper"
    str sunset_text="Sunset wallpaper"
    str ctwlightend_text="Civil twiligth end wallpaper"
    str scripturi_text="Get change times from php script uri"

    data ctwls#1
    data ctwlightstart#1
    data ctwls_b#1
    data sr#1
    data sunrise#1
    data sunrise_b#1
    data ss#1
    data sunset#1
    data sunset_b#1
    data ctwle#1
    data ctwlightend#1
    data ctwle_b#1
    data su#1
    data scripturi#1
    sd ptr_cls^ctwls

    data click^file_sel_clicked
    import "_g_signal_connect_data" g_signal_connect_data
    import "_gtk_button_new_with_label" gtk_button_new_with_label
    str lab="Browse"
    str cl="clicked"

    setcall ctwls labelfield_left_prepare(ctwlightstart_text)
    setcall ctwlightstart gtk_entry_new()
    setcall ctwls_b gtk_button_new_with_label(lab)
    call g_signal_connect_data(ctwls_b,cl,click,ctwlightstart,0,0)

    setcall sr labelfield_left_prepare(sunrise_text)
    setcall sunrise gtk_entry_new()
    setcall sunrise_b gtk_button_new_with_label(lab)
    call g_signal_connect_data(sunrise_b,cl,click,sunrise,0,0)

    setcall ss labelfield_left_prepare(sunset_text)
    setcall sunset gtk_entry_new()
    setcall sunset_b gtk_button_new_with_label(lab)
    call g_signal_connect_data(sunset_b,cl,click,sunset,0,0)

    setcall ctwle labelfield_left_prepare(ctwlightend_text)
    setcall ctwlightend gtk_entry_new()
    setcall ctwle_b gtk_button_new_with_label(lab)
    call g_signal_connect_data(ctwle_b,cl,click,ctwlightend,0,0)

    sd entriestable
    setcall entriestable tablefield_cells(vbox,4,3,ptr_cls)

    setcall su labelfield_left_prepare(scripturi_text)
    setcall scripturi gtk_entry_new()
    call gtk_table_attach_defaults(entriestable,su,0,1,5,6)
    call gtk_table_attach_defaults(entriestable,scripturi,1,2,5,6)

    setcall wd gtk_hseparator_new()
    call gtk_container_add(vbox,wd)

    #skip settings
    sd hbox
    setcall hbox gtk_hbox_new(0,0)

    data skip_radio_yes#1
    data skip_radio_no#1

    str y="Yes"
    str n="No"

    str skip_text="Skip settings when program starts: "
    setcall wd labelfield_left_prepare(skip_text)
    call pack_default(hbox,wd)

    setcall skip_radio_yes gtk_radio_button_new_with_label(0,y)
    setcall wd gtk_radio_button_get_group(skip_radio_yes)
    setcall skip_radio_no gtk_radio_button_new_with_label(wd,n)
    call pack_default(hbox,skip_radio_yes)
    call pack_default(hbox,skip_radio_no)

    call gtk_container_add(vbox,hbox)

    setcall wd gtk_hseparator_new()
    call gtk_container_add(vbox,wd)

##set values at the fields
    import "_gtk_toggle_button_set_active" gtk_toggle_button_set_active

    #
    if lat_zone==0
        call gtk_toggle_button_set_active(lat_radio_n,1)
    else
        call gtk_toggle_button_set_active(lat_radio_s,1)
    endelse
    call gtk_entry_set_text(lat_d,lat_degrees)
    call gtk_entry_set_text(lat_m,lat_minutes)
    #
    if lg_zone==0
        call gtk_toggle_button_set_active(lg_radio_n,1)
    else
        call gtk_toggle_button_set_active(lg_radio_s,1)
    endelse
    call gtk_entry_set_text(lg_d,lg_degrees)
    call gtk_entry_set_text(lg_m,lg_minutes)
    #
    if off_zone==0
        call gtk_toggle_button_set_active(off_radio_e,1)
    else
        call gtk_toggle_button_set_active(off_radio_w,1)
    endelse
    call gtk_entry_set_text(off_h,off_hours)
    #
    call gtk_entry_set_text(ctwlightstart,twilight_start)
    call gtk_entry_set_text(sunrise,sun_rise)
    call gtk_entry_set_text(sunset,sun_set)
    call gtk_entry_set_text(ctwlightend,twilight_end)
    call gtk_entry_set_text(scripturi,uri)
    #
    if skipvalue==0
        call gtk_toggle_button_set_active(skip_radio_no,1)
    else
        call gtk_toggle_button_set_active(skip_radio_yes,1)
    endelse
    #
    return 1
endif
#write to file for storing causes
if mod==3
    sd file
    set file arg1
    import "_gtk_toggle_button_get_active" gtk_toggle_button_get_active
    import "file_write" file_write
    sd bool
    import "_gtk_entry_get_text" gtk_entry_get_text
    sd text
    import "slen" slen
    sd text_size

    #lat
    setcall bool gtk_toggle_button_get_active(lat_radio_n)
    if bool==1
        set lat_zone 0
    else
        set lat_zone 1
    endelse
    setcall err file_write(p_lat_zone,4,file)
    if err!=(noerror)
        return 0
    endif

    setcall text gtk_entry_get_text(lat_d)
    setcall text_size slen(text)
    inc text_size
    setcall err file_write(text,text_size,file)
    if err!=(noerror)
        return 0
    endif

    setcall text gtk_entry_get_text(lat_m)
    setcall text_size slen(text)
    inc text_size
    setcall err file_write(text,text_size,file)
    if err!=(noerror)
        return 0
    endif
    #long
    setcall bool gtk_toggle_button_get_active(lg_radio_n)
    if bool==1
        set lg_zone 0
    else
        set lg_zone 1
    endelse
    setcall err file_write(p_lg_zone,4,file)
    if err!=(noerror)
        return 0
    endif

    setcall text gtk_entry_get_text(lg_d)
    setcall text_size slen(text)
    inc text_size
    setcall err file_write(text,text_size,file)
    if err!=(noerror)
        return 0
    endif

    setcall text gtk_entry_get_text(lg_m)
    setcall text_size slen(text)
    inc text_size
    setcall err file_write(text,text_size,file)
    if err!=(noerror)
        return 0
    endif
    #off
    setcall bool gtk_toggle_button_get_active(off_radio_e)
    if bool==1
        set off_zone 0
    else
        set off_zone 1
    endelse
    setcall err file_write(p_off_zone,4,file)
    if err!=(noerror)
        return 0
    endif

    setcall text gtk_entry_get_text(off_h)
    setcall text_size slen(text)
    inc text_size
    setcall err file_write(text,text_size,file)
    if err!=(noerror)
        return 0
    endif

    #wallpapers
    setcall text gtk_entry_get_text(ctwlightstart)
    setcall text_size slen(text)
    inc text_size
    setcall err file_write(text,text_size,file)
    if err!=(noerror)
        return 0
    endif

    setcall text gtk_entry_get_text(sunrise)
    setcall text_size slen(text)
    inc text_size
    setcall err file_write(text,text_size,file)
    if err!=(noerror)
        return 0
    endif
    setcall text gtk_entry_get_text(sunset)
    setcall text_size slen(text)
    inc text_size
    setcall err file_write(text,text_size,file)
    if err!=(noerror)
        return 0
    endif
    setcall text gtk_entry_get_text(ctwlightend)
    setcall text_size slen(text)
    inc text_size
    setcall err file_write(text,text_size,file)
    if err!=(noerror)
        return 0
    endif
    #script
    setcall text gtk_entry_get_text(scripturi)
    setcall text_size slen(text)
    inc text_size
    setcall err file_write(text,text_size,file)
    if err!=(noerror)
        return 0
    endif
    #skip settings
    setcall bool gtk_toggle_button_get_active(skip_radio_no)
    if bool==1
        set skipvalue 0
    else
        set skipvalue 1
    endelse
    setcall err file_write(p_skipvalue,4,file)
    if err!=(noerror)
        return 0
    endif

    #write ok
    set ptr_allok# 1
    return 1
endif
if mod==4
    #prepare get times
    chars senddata#MAX_URI+dword_null+1+2+2+1+2+2+1+2+50
    str sends^senddata
    str sendformat="%s?lat=%s%s.%u&long=%s%s.%u&offset=%s%s"
    str negative="-"
    str positive=""
    import "strtoint_positive" strtoint_positive
    import "strtoint_positive_minutes_to_decimal" strtoint_positive_minutes_to_decimal
    sd number
    sd ptr_out^number

    #get the values:...,verify degrees to be positive, and convert positive minutes to zecimals
    #uri from data uri
    #lat sign
    ss lat_sign
    set lat_sign positive
    if lat_zone==1
        set lat_sign negative
    endif
    #lat degrees check
    setcall bool strtoint_positive(lat_degrees,ptr_out)
    if bool==0
        return 0
    endif
    #lat minutes check and convert
    setcall bool strtoint_positive_minutes_to_decimal(lat_minutes,ptr_out)
    if bool==0
        return 0
    endif
    sd decimal_lat
    set decimal_lat number
    #long sign
    ss lg_sign
    set lg_sign positive
    if lg_zone==0
        set lg_sign negative
    endif
    #long degrees check
    setcall bool strtoint_positive(lg_degrees,ptr_out)
    if bool==0
        return 0
    endif
    #long minutes check and convert
    setcall bool strtoint_positive_minutes_to_decimal(lg_minutes,ptr_out)
    if bool==0
        return 0
    endif
    sd decimal_lg
    set decimal_lg number
    #offset sign
    ss off_sign
    set off_sign positive
    if off_zone==0
        set off_sign negative
    endif
    #offset hours check
    setcall bool strtoint_positive(off_hours,ptr_out)
    if bool==0
        return 0
    endif

    #make the request
    import "_sprintf" sprintf
    call sprintf(sends,sendformat,uri,lat_sign,lat_degrees,decimal_lat,lg_sign,lg_degrees,decimal_lg,off_sign,off_hours)

    set ptr_allok# 1
    return 1
endif
if mod==5
#bool return in this case
#call
    import "uri_get_content_forward" uri_get_content_forward
    import "wallpapers" wallpapers
    data w^wallpapers

    setcall err uri_get_content_forward(sends,w)
    if err!=(noerror)
        return 0
    endif

    set ptr_allok# 1
    #all times are ok
    return 1
endif
#if mod==6
#set wallpaper
    sd index
    set index arg1

    sd wallpaperlocation
    if index==0
        set wallpaperlocation twilight_end
    elseif index==1
        set wallpaperlocation twilight_start
    elseif index==2
        set wallpaperlocation sun_rise
    else
        set wallpaperlocation sun_set
    endelse

    import "_SystemParametersInfoA@16" SystemParametersInfo

    import "texter_warning" texter_warning

    setcall bool SystemParametersInfo((SPI_SETDESKWALLPAPER),0,wallpaperlocation,(SPIF_UPDATEINIFILE))
    if bool==0
        str cannotsetwallpaper="Could not set a wallpaper"
        call texter_warning(cannotsetwallpaper)
        return cannotsetwallpaper
    endif

    return 1
endfunction

function datafile()
    str data_f="files/values.data"
    return data_f
endfunction

function reads_wrap(sd mem,sd size)
    call actions(1,mem,size)
endfunction
function reads()
    import "file_get_content_forward" file_get_content_forward
    data f^reads_wrap
    sd path
    setcall path datafile()
    call file_get_content_forward(path,f)
endfunction

function writes_wrap(sd file)
    call actions(3,file)
endfunction
function writes()
    import "file_write_forward" file_write_forward
    data f^writes_wrap
    sd path
    setcall path datafile()
    call file_write_forward(path,f)
endfunction

#bool
function prelaunch_calls()
    data ptr_allok%ptr_allok

    set ptr_allok# 0
    call actions(4)
    if ptr_allok#==0
        return 0
    endif

    set ptr_allok# 0
    call actions(5)
    return ptr_allok#
endfunction

#bool
function prelaunch()
    data ptr_allok%ptr_allok

    set ptr_allok# 0
    call writes()
    if ptr_allok#==0
        return 0
    endif

    #read action has size verifications
    set ptr_allok# 0
    call reads()
    if ptr_allok#==0
        return 0
    endif

    sd bool
    setcall bool prelaunch_calls()
    return bool
endfunction

function getskipvalue()
    data p%ptr_skipvalue
    return p#
endfunction
