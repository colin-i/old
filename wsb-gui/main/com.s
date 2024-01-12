


format elfobj

include "../include/wsb.h"


function img_folder()
    str i="img"
    return i
endfunction
function data_folder()
    str d="data"
    return d
endfunction


function inits(sd window)
    call set_app_icon(window)

    #updates
    import "updates" updates
    call updates()
endfunction

#pixbuf/0
function pixbuf_from_file(ss filename)
    import "gdk_pixbuf_new_from_file" gdk_pixbuf_new_from_file
    sd pixbuf
    sd gerror=0
    setcall pixbuf gdk_pixbuf_new_from_file(filename,#gerror)
    if pixbuf==0
        call gerrtoerr(gerror)
    endif
    return pixbuf
endfunction

function gerrtoerr(sd gerror)
    import "g_error_free" g_error_free
    #typedef guint32 GQuark;
    #GQuark       domain
    #gint         code
    #gchar       *message
    sd message
    set message gerror
    add message (2*DWORD)
    call texter(message#)

    call g_error_free(gerror)
endfunction


function texter(ss msg)
    import "printf" printf
    call printf(msg)
    chars nl={0xa,0}
    call printf(#nl)
endfunction

function pack_tight(sd box,sd wid)
    import "gtk_box_pack_start" gtk_box_pack_start
    call gtk_box_pack_start(box,wid,0,0,0)
endfunction

import "mainvbox" mainvbox
import "mainwindow_label" mainwindow_label

import "gtk_vbox_new" gtk_vbox_new
import "gtk_hbox_new" gtk_hbox_new
import "gtk_container_add" gtk_container_add
function mainwindow(sd window)
    sd vbox
    setcall vbox gtk_vbox_new(0,0)
    call gtk_container_add(window,vbox)
    sd b
    setcall b mainvbox()
    set b# vbox

    #title
    sd label
    setcall label mainwindow_label()
    str name="Web Site Builder"
    setcall label# label_left(name)
    call pack_tight(vbox,label#)

    #updates
    import "update_info" update_info
    sd info
    setcall info update_info()
    if info#==(TRUE)
        import "update_text" update_text
        sd text
        setcall text update_text(0)
        sd label_info
        setcall label_info label_left(text)
        call pack_tight(vbox,label_info)
    endif

    #buttons
    sd hbox
    setcall hbox gtk_hbox_new(0,0)
    call pack_tight(vbox,hbox)

    chars new="New Site"
    data *new_fn^new_site
    chars *new_i="Create a new site in a selected folder"
    #
    import "open_last" open_last
    chars *open="Open Last"
    data *open_fn^open_last
    chars *open_i="Open last site project"
    #
    chars *options="Options"
    data *^options
    chars *options_i="Set the preferences"
    #
    data *=0

    call add_buttons(hbox,#new)
endfunction

function connect_signal_data(sd widget,ss signal,sd fn,sd data)
    import "g_signal_connect_data" g_signal_connect_data
    call g_signal_connect_data(widget,signal,fn,data,0,0)
endfunction
import "strlen" strlen
import "gtk_button_new_with_label" gtk_button_new_with_label
function add_buttons(sd box,sd packs)
    ss title
    sd fn
    ss tooltip
    #
    while packs#!=0
        set title packs
        addcall packs strlen(packs)
        inc packs
        set fn packs#
        add packs (DWORD)
        set tooltip packs
        addcall packs strlen(packs)
        inc packs

        sd button
        setcall button gtk_button_new_with_label(title)
        #
        str clicked="clicked"
        call connect_signal_data(button,clicked,fn,0)
        #
        import "gtk_widget_set_tooltip_markup" gtk_widget_set_tooltip_markup
        call gtk_widget_set_tooltip_markup(button,tooltip)
        #
        call pack_tight(box,button)
    endwhile
endfunction

#####

function new_site()
    str title="New Site"
    data i^new_site_init
    data s^new_site_set
    call dialog_modal_def_action(title,i,s,200,200)
endfunction
function new_site_init(sd vbox)
    import "gtk_table_new" gtk_table_new
    import "gtk_table_attach_defaults" gtk_table_attach_defaults
    import "gtk_table_attach" gtk_table_attach
    import "gtk_entry_new" gtk_entry_new
    sd label
    sd entry
    sd pointer
    sd table
    setcall table gtk_table_new(2,3,(FALSE))
    #
    ss name="Project Name"
    setcall label label_left(name)
    call gtk_table_attach_defaults(table,label,0,1,0,1)
    ##
    setcall entry gtk_entry_new()
    call gtk_table_attach_defaults(table,entry,1,2,0,1)
    setcall pointer new_site_name()
    set pointer# entry
    #
    ss folder="Folder"
    setcall label label_left(folder)
    call gtk_table_attach_defaults(table,label,0,1,1,2)
    ##
    setcall entry gtk_entry_new()
    call gtk_table_attach_defaults(table,entry,1,2,1,2)
    setcall pointer new_site_folder()
    set pointer# entry
    ##
    sd sel_button
    ss sel="Select"
    setcall sel_button gtk_button_new_with_label(sel)
    call gtk_table_attach(table,sel_button,2,3,1,2,(GTK_SHRINK),(GTK_SHRINK),0,0)
    str sg="clicked"
    data fn^new_site_sel_click
    call connect_signal_data(sel_button,sg,fn,0)
    #
    call gtk_container_add(vbox,table)
endfunction
import "gtk_entry_get_text" gtk_entry_get_text
import "sprintf" sprintf
function new_site_set()
    import "proj_work_with_all_projects" proj_work_with_all_projects
    call proj_work_with_all_projects()
    sd name
    sd folder
    setcall name new_site_name()
    setcall folder new_site_folder()
    ss folder_string
    ss name_string
    setcall folder_string gtk_entry_get_text(folder#)
    setcall name_string gtk_entry_get_text(name#)
    #write to remember the last opened project
    sd info#2
    set info folder_string
    sd projname
    set projname #info
    add projname (DWORD)
    set projname# name_string
    import "write_last_site_opened" write_last_site_opened
    call write_last_site_opened(#info)
    #get project filename
    import "proj_set" proj_set
    ss filename
    setcall filename proj_set(folder_string,name_string)
    if filename==0
        return (void)
    endif
    import "file_open_overwrite" file_open_overwrite
    import "fclose" fclose
    sd file
    setcall file file_open_overwrite(filename)
    if file==0
        return (void)
    endif
    import "proj_root" proj_root
    import "proj_default_index" proj_default_index
    call proj_root((value_set),name_string,file)
    call proj_default_index(file)
    call fclose(file)
    #read and display the project
    import "file_get_content_forward_data" file_get_content_forward_data
    import "proj_read_file" proj_read_file
    call file_get_content_forward_data(filename,proj_read_file,name_string)
endfunction

function new_site_name()
    data name#1
    return #name
endfunction
function new_site_folder()
    data folder#1
    return #folder
endfunction
import "gtk_entry_set_text" gtk_entry_set_text
function new_site_sel_click()
    ss folder
    setcall folder filechooser_folder()
    if folder!=0
        sd folder_entry
        setcall folder_entry new_site_folder()
        call gtk_entry_set_text(folder_entry#,folder)
        import "g_free" g_free
        call g_free(folder)
    endif
endfunction

######

#0/foldername, must be freed
function filechooser_folder()
    ss filename
    str sel_folder="Select Folder"
    setcall filename filechooser(sel_folder,(GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER))
    return filename
endfunction
import "gtk_file_chooser_dialog_new" gtk_file_chooser_dialog_new
import "gtk_dialog_run" gtk_dialog_run
import "gtk_widget_destroy" gtk_widget_destroy
function filechooser(ss title,sd action)
    str GTK_STOCK_CANCEL="gtk-cancel"
    str select="Select"
    sd dialog
    setcall dialog gtk_file_chooser_dialog_new(title,0,action,GTK_STOCK_CANCEL,(GTK_RESPONSE_CANCEL),select,(GTK_RESPONSE_ACCEPT),0)
    sd resp
    setcall resp gtk_dialog_run(dialog)
    ss filename=0
    if resp==(GTK_RESPONSE_ACCEPT)
        import "gtk_file_chooser_get_filename_utf8" gtk_file_chooser_get_filename_utf8
        setcall filename gtk_file_chooser_get_filename_utf8(dialog)
    endif
    call gtk_widget_destroy(dialog)
    return filename
endfunction
import "_get_errno" get_errno
function errno_disp(ss text)
    sd er
    call get_errno(#er)
    chars info#300
    str f="%s. Errno: %i"
    call sprintf(#info,f,text,er)
    call texter(#info)
endfunction

#mem/0
function memalloc(sd sz)
    import "malloc" malloc
    sd mem
    SetCall mem malloc(sz)
    If mem==0
        str newmem="Malloc failed"
        call texter(newmem)
    endif
    return mem
EndFunction

function dialog_modal_def_action(ss title,sd forward_init,sd forward_set,sd width,sd height)
    str GTK_STOCK_CANCEL="gtk-cancel"
    str GTK_STOCK_OK="gtk-ok"

    import "gtk_dialog_new_with_buttons" gtk_dialog_new_with_buttons
    sd dialog
    setcall dialog gtk_dialog_new_with_buttons(title,0,(GTK_DIALOG_MODAL),GTK_STOCK_CANCEL,(GTK_RESPONSE_CANCEL),GTK_STOCK_OK,(GTK_RESPONSE_OK),0)

    import "gtk_window_set_default_size" gtk_window_set_default_size
    call gtk_window_set_default_size(dialog,width,height)

    import "gtk_dialog_get_content_area" gtk_dialog_get_content_area
    sd vbox
    setcall vbox gtk_dialog_get_content_area(dialog)
    call forward_init(vbox)

    import "gtk_widget_show_all" gtk_widget_show_all
    call gtk_widget_show_all(dialog)
    sd resp
    setcall resp gtk_dialog_run(dialog)
    if resp==(GTK_RESPONSE_OK)
        call forward_set()
    endif
    call gtk_widget_destroy(dialog)
endfunction

#######label
function label_left(ss text)
    sd widget
    ss GTK_TYPE_LABEL
    str label="label"
    str x="xalign"
    data left=0

    import "gtk_label_get_type" gtk_label_get_type
    import "gtk_widget_new" gtk_widget_new
    setcall GTK_TYPE_LABEL gtk_label_get_type()
    setcall widget gtk_widget_new(GTK_TYPE_LABEL,label,text,x,left,left,0)
    return widget
endfunction

#bool
function set_script_folder()
    import "GetModuleFileNameA" GetModuleFileName
    ss mem
    setcall mem Scriptfullpath()
    sd size
    setcall size GetModuleFileName(0,mem,(MAX_PATH))
    if size==0
        call texter("GetModuleFileName Error.")
        return (FALSE)
    endif
    ss cursor
    setcall cursor endoffolders(mem)
    set cursor# 0
    sd namestart
    setcall namestart Scriptfullpath_namestart()
    set namestart# cursor
    return (TRUE)
endfunction

#mem
function Scriptfullpath()
    chars mem#MAX_PATH
    chars *safe_chars_to_add#safe_chars_to_add
                    #GetModuleFileName
                    #Windows XP:  The string is truncated to nSize characters and is not null-terminated
                    #not this case, can't be greater than max_path
                    #end with 0 for fun
    chars *safe_end=0
    return #mem
endfunction
function Scriptfullpath_get()
    sd namestart
    setcall namestart Scriptfullpath_namestart()
    ss name_start
    set name_start namestart#
    set name_start# 0
    ss path
    setcall path Scriptfullpath()
    return path
endfunction
function Scriptfullpath_namestart()
    str namestart#1
    return #namestart
endfunction


#('c:\folder\file.txt' will be pointer starting at 'file.txt')
Function endoffolders(ss path)
    sd sz
    setcall sz strlen(path)
    ss cursor
    set cursor path
    add cursor sz
    sd i=0
    while i<sz
        dec cursor
        sd bool
        setcall bool filepathdelims(cursor)
        if bool==(TRUE)
            inc cursor
            return cursor
        endif
        inc i
    endwhile
    return path
EndFunction
#true if match or false
Function filepathdelims(ss str)
        Chars bslash="\\"
        Chars slash="/"
        If str#==bslash
                Return (TRUE)
        EndIf
        If str#==slash
                Return (TRUE)
        EndIf
        Return (FALSE)
EndFunction
#bool
import "_chdir" chdir
function dirch(ss value)
    sd change
    setcall change chdir(value)
    if change!=0
        str chdirerr="Change dir error"
        call texter(chdirerr)
        return (FALSE)
    endif
    return (TRUE)
endfunction
#path
function get_folder_fname(ss folder,ss fname)
    chars idata#50
    str iform="%s/%s"
    call sprintf(#idata,iform,folder,fname)
    sd allname
    setcall allname get_scriptfolder_path(#idata)
    return allname
endfunction
#path
function get_scriptfolder_path(ss path)
    ss scriptfolder
    setcall scriptfolder Scriptfullpath_get()
    #
    import "strcat" strcat
    call strcat(scriptfolder,path)
    return scriptfolder
endfunction

function set_app_icon(sd window)
    str icon="1616.bmp"
    ss img
    setcall img img_folder()
    ss ic
    setcall ic get_folder_fname(img,icon)
    #
    sd pixbuf
    setcall pixbuf pixbuf_from_file(ic)
    if pixbuf!=0
        import "gtk_window_set_icon" gtk_window_set_icon
        import "g_object_unref" g_object_unref
        call gtk_window_set_icon(window,pixbuf)
        call g_object_unref(pixbuf)
    endif
endfunction

#bool
function slen_s(ss string,sd size,sd p_out)
    sd i=0
    while i!=size
        if string#==0
            set p_out# i
            return (TRUE)
        endif
        inc string
        inc i
    endwhile
    return (FALSE)
endfunction

function getoneax(sd x)
    return x
endfunction

function options()
    sd dialog
    setcall dialog gtk_dialog_new_with_buttons("Options",0,(GTK_DIALOG_MODAL),"gtk-ok",(GTK_RESPONSE_OK),"gtk-cancel",(GTK_RESPONSE_CANCEL),0)

    sd vbox
    setcall vbox gtk_dialog_get_content_area(dialog)

    import "gtk_check_button_new_with_label" gtk_check_button_new_with_label
    import "update_pref_get" update_pref_get
    import "update_pref_set" update_pref_set
    sd bool
    sd updates_option
    setcall updates_option gtk_check_button_new_with_label("Check for updates")
    setcall bool update_pref_get()
    if bool==(TRUE)
        import "gtk_toggle_button_set_active" gtk_toggle_button_set_active
        call gtk_toggle_button_set_active(updates_option,bool)
    endif
    call gtk_container_add(vbox,updates_option)

    call gtk_widget_show_all(updates_option)
    sd resp
    setcall resp gtk_dialog_run(dialog)
    if resp==(GTK_RESPONSE_OK)
        import "gtk_toggle_button_get_active" gtk_toggle_button_get_active
        setcall bool gtk_toggle_button_get_active(updates_option)
        call update_pref_set(bool)
    endif
    call gtk_widget_destroy(dialog)
endfunction


#bool
function dialog_message_format(ss format)
    sd len
    setcall len strlen(format)
    sd nr_of_args=0
    sd count_2=0
    chars escape="%"
    ss cursor
    set cursor format
    while len!=0
        if count_2==0
            if cursor#==escape
                inc count_2
            endif
        else
            if cursor#!=escape
                inc nr_of_args
            endif
            set count_2 0
        endelse
        inc cursor
        dec len
    endwhile
    const max_args=30
    if nr_of_args>(max_args)
        call texter("Too many arguments at message")
        return (FALSE)
    endif
    import "gtk_message_dialog_new" gtk_message_dialog_new
    sd useful_value
    sd *parent=0
    sd *flags=GTK_DIALOG_MODAL
    sd *type=GTK_MESSAGE_OTHER
    sd *buttons=GTK_BUTTONS_YES_NO
    sd dialog_format
    sd args#max_args

    set dialog_format format
    sd i=0
    sd args_in^format
    add args_in (DWORD)
    sd args_message^args
    while i<nr_of_args
        set args_message# args_in#
        add args_in (DWORD)
        add args_message (DWORD)
        inc i
    endwhile
    #
    set useful_value gtk_message_dialog_new
    call getoneax(#useful_value)
    #mov esp eax
    hex 0x8b,esp*to_regopcode|mod_reg|eax
    #mov eax [esp]
    hex 0x8b,eax*to_regopcode|mod_main|R_M_sib,esp*to_regopcode|mod_main|esp
    #add esp 4
    const add_REG_OPCODE=0
    hex 0x81,add_REG_OPCODE*to_regopcode|mod_reg|esp,4,0,0,0
    #call [eax]
    const call_reg=2*to_regopcode
    hex 0xff,mod_reg|call_reg|eax
    #sub esp 4
    const sub_REG_OPCODE=5
    hex 0x81,sub_REG_OPCODE*to_regopcode|mod_reg|esp,4,0,0,0
    #mov [esp] eax
    hex 0x89,eax*to_regopcode|mod_main|R_M_sib,esp*to_regopcode|mod_main|esp
    #
    sd widget
    set widget useful_value
    sd bool
    sd resp
    setcall resp gtk_dialog_run(widget)
    if resp==(GTK_RESPONSE_YES)
        set bool (TRUE)
    else
        set bool (FALSE)
    endelse
    call gtk_widget_destroy(widget)
    return bool
endfunction
#0/len
function get_len_with_null_max_path(ss mem,sd size)
    sd len=0
    ss cursor
    set cursor mem
    while len<size
        inc len
        if cursor#==0
            if len>(MAX_PATH)
                call texter("path too long at root folder read")
                return 0
            endif
            return len
        endif
        inc cursor
    endwhile
    return 0
endfunction
