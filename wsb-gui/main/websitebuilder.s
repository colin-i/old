

format elfobj

include "../include/wsb.h"

import "gtk_init" gtk_init
import "gtk_window_new" gtk_window_new
import "gtk_window_set_title" gtk_window_set_title
import "gtk_window_set_default_size" gtk_window_set_default_size
import "gtk_main_quit" gtk_main_quit
import "g_signal_connect_data" g_signal_connect_data
import "gtk_widget_show_all" gtk_widget_show_all
import "gtk_main" gtk_main
#bool
entry main()

#
import "update_info" update_info
sd info
setcall info update_info()
set info# 0
import "set_script_folder" set_script_folder
sd bool
setcall bool set_script_folder()
if bool==(FALSE)
    return (FALSE)
endif
import "proj_hbox" proj_hbox
sd prj_hbox
setcall prj_hbox proj_hbox()
set prj_hbox# 0
#

call gtk_init((NULL),(NULL))

sd window
setcall window gtk_window_new((GTK_WINDOW_TOPLEVEL))

#
import "inits" inits
call inits(window)

str programname="WebSiteBuilder"
call gtk_window_set_title(window,programname)

data width=800
data height=600
call gtk_window_set_default_size(window,width,height)

import "mainwindow" mainwindow
call mainwindow(window)

str ds="destroy"
data q^gtk_main_quit
call g_signal_connect_data(window,ds,q,(NULL),(NULL),(NULL))

import "texter" texter
str go="Ready"
call texter(go)

call gtk_widget_show_all(window)
call gtk_main()

#this never returns with some gtk stuff, do cleans with exit
#return 1
import "exit" exit
call exit((TRUE))


