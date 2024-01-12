
format elfobj

include "../include/wsb.h"
import "strlen" strlen
import "sprintf" sprintf
import "dirch" dirch
#filename/0
function proj_set(ss folder_name,ss name_string)
    #get file name
    sd sz
    setcall sz strlen(name_string)
    str ext=".pro"
    addcall sz strlen(ext)
    if sz>=(MAX_PATH)
        return 0
    endif
    str filename="%s%s"
    chars name_data#MAX_PATH
    str fname^name_data
    call sprintf(fname,filename,name_string,ext)
    #move to project folder
    sd bool
    setcall bool dirch(folder_name)
    if bool!=(TRUE)
        return 0
    endif
    #
    return fname
endfunction

function proj_tree_model()
    data tree_model#1
    return #tree_model
endfunction

import "mainvbox" mainvbox
import "nav_root" nav_root
import "mainwindow_label" mainwindow_label
import "proj_hbox" proj_hbox

const column_text=0

import "gtk_container_add" gtk_container_add
import "gtk_hbox_new" gtk_hbox_new
import "gtk_vbox_new" gtk_vbox_new
import "pack_tight" pack_tight
import "fclose" fclose

function proj_read_file(ss mem,sd size,ss name_string)
    sd vbox
    setcall vbox mainvbox()
    sd hbox
    setcall hbox gtk_hbox_new(0,0)
    call gtk_container_add(vbox#,hbox)
    sd prj_hbox
    setcall prj_hbox proj_hbox()
    set prj_hbox# hbox
    #
    sd nav
    setcall nav gtk_vbox_new(0,0)
    call pack_tight(hbox,nav)
    #
    import "gtk_file_chooser_button_new" gtk_file_chooser_button_new
    sd root_chooser
    setcall root_chooser gtk_file_chooser_button_new("Choose root folder",(GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER))
    call pack_tight(nav,root_chooser)
    sd root
    setcall root nav_root()
    set root# root_chooser
    #tree view
    import "gtk_cell_renderer_text_new" gtk_cell_renderer_text_new
    import "gtk_tree_view_column_new" gtk_tree_view_column_new
    import "gtk_tree_view_column_set_title" gtk_tree_view_column_set_title
    import "gtk_tree_view_column_pack_start" gtk_tree_view_column_pack_start
    import "gtk_tree_view_column_set_attributes" gtk_tree_view_column_set_attributes
    import "gtk_tree_view_new" gtk_tree_view_new
    import "gtk_tree_view_append_column" gtk_tree_view_append_column
    import "gtk_tree_store_new" gtk_tree_store_new
    import "gtk_tree_view_set_model" gtk_tree_view_set_model
    sd cell
    setcall cell gtk_cell_renderer_text_new()
    sd treecolumn
    setcall treecolumn gtk_tree_view_column_new()
    call gtk_tree_view_column_set_title(treecolumn,"Files/Folders")
    call gtk_tree_view_column_pack_start(treecolumn,cell,(TRUE))
    call gtk_tree_view_column_set_attributes(treecolumn,cell,"text",(column_text),0)
    sd treeview
    setcall treeview gtk_tree_view_new()
    call gtk_tree_view_append_column(treeview,treecolumn)
    const G_TYPE_STRING=16*DWORD
    sd treestore
    setcall treestore gtk_tree_store_new(1,(G_TYPE_STRING))
    call gtk_tree_view_set_model(treeview,treestore)
    sd tree_model
    setcall tree_model proj_tree_model()
    set tree_model# treestore
    call pack_tight(nav,treeview)
    #
    import "gtk_label_set_text" gtk_label_set_text
    sd label
    setcall label mainwindow_label()
    call gtk_label_set_text(label#,name_string)
    #
    sd len
    setcall len proj_root((value_get),name_string,mem,size)
    if len==0
        return (void)
    endif
    add mem len
    sub size len
    #
    call proj_tree_add(mem,size)
    #
    import "gtk_widget_show_all" gtk_widget_show_all
    call gtk_widget_show_all(hbox)
endfunction
import "get_len_with_null_max_path" get_len_with_null_max_path
import "texter" texter
function proj_tree_add(ss mem,sd size)
    sd len
    setcall len get_len_with_null_max_path(mem,size)
    import "gtk_tree_store_append" gtk_tree_store_append
    import "gtk_tree_store_set" gtk_tree_store_set
    sd tree_model
    setcall tree_model proj_tree_model()
    chars TreeIter#16
    call gtk_tree_store_append(tree_model#,#TreeIter,0)
    call gtk_tree_store_set(tree_model#,#TreeIter,(column_text),mem,-1)
endfunction

function proj_root(sd procedure,ss name_string,sd data,sd size)
    sd bool
    if procedure==(value_set)
        #make root folder
        import "mkdir_message_on_exist" mkdir_message_on_exist

        setcall bool mkdir_message_on_exist(name_string)
        if bool==(FALSE)
            return (void)
        endif
        #add to .pro
        import "file_write" file_write
        sd sz
        setcall sz strlen(name_string)
        inc sz
        setcall bool file_write(data,name_string,sz)
        if bool==(FALSE)
            return (void)
        endif
    else
    #0/len(+ 0 termination)
        #read root and write to widget
        sd len
        setcall len get_len_with_null_max_path(data,size)
        if len==0
            return 0
        endif
        sd root
        setcall root nav_root()
        import "gtk_file_chooser_set_current_folder_utf8" gtk_file_chooser_set_current_folder_utf8
        setcall bool gtk_file_chooser_set_current_folder_utf8(root#,data)
        if bool!=(TRUE)
            call texter("file chooser error")
            return 0
        endif
        return len
    endelse
endfunction

function proj_default_index(sd pro_file)
    #make main index
    import "file_open_overwrite" file_open_overwrite
    str indx="index.html"
    sd file
    setcall file file_open_overwrite(indx)
    if file==0
        return (void)
    endif
    call fclose(file)
    #add to .pro
    sd sz
    setcall sz strlen(indx)
    inc sz
    sd bool
    setcall bool file_write(pro_file,indx,sz)
    if bool==(FALSE)
        return (void)
    endif
endfunction

function proj_work_with_all_projects()
    sd prj_hbox
    setcall prj_hbox proj_hbox()
    if prj_hbox#!=0
        import "gtk_widget_destroy" gtk_widget_destroy
        call gtk_widget_destroy(prj_hbox#)
        set prj_hbox# 0
    endif
endfunction
