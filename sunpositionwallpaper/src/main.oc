



format elfobj

include "constants.h"

function _start(sd *argc,sd *argv)
    import "_gtk_init" gtk_init
    call gtk_init((NULL),(NULL))

    import "movetoScriptfolder" movetoScriptfolder
    call movetoScriptfolder()

    import "actions" actions
    call actions(0)

    data settings_clicked#1
    set settings_clicked 0
    const ptr_settings_clicked^settings_clicked

    data settings_tray#1
    const ptr^settings_tray
    set settings_tray 1
    while settings_tray==1
        #read values from file
        import "reads" reads
        call reads()

        sd skipvalue=0

        if settings_clicked==0
            import "getskipvalue" getskipvalue
            setcall skipvalue getskipvalue()

            data bool#1
            if skipvalue==1
                import "prelaunch_calls" prelaunch_calls
                setcall bool prelaunch_calls()
                if bool==0
                    set skipvalue 0
                endif
            endif
        else
            set settings_clicked 0
        endelse

        if skipvalue==0
            str programname="Sun Position Wallpaper"
            str GTK_STOCK_OK="gtk-ok"
            str GTK_STOCK_EXIT="gtk-quit"

            import "_gtk_dialog_new_with_buttons" gtk_dialog_new_with_buttons
            data dialog#1
            const ptr_dialog^dialog
            setcall dialog gtk_dialog_new_with_buttons(programname,0,(GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),GTK_STOCK_EXIT,(GTK_RESPONSE_CANCEL),GTK_STOCK_OK,(GTK_RESPONSE_OK),0)

            import "_gtk_dialog_get_content_area" gtk_dialog_get_content_area
            sd vbox
            setcall vbox gtk_dialog_get_content_area(dialog)

            call actions(2,vbox,dialog)

            import "_gtk_widget_show_all" gtk_widget_show_all
            call gtk_widget_show_all(dialog)

            import "_gtk_dialog_run" gtk_dialog_run
            sd response
            setcall response gtk_dialog_run(dialog)

            import "_gtk_widget_destroy" gtk_widget_destroy
            if response==(GTK_RESPONSE_OK)
                import "prelaunch" prelaunch
                setcall bool prelaunch()

                call gtk_widget_destroy(dialog)
            else
                set bool 0
                set settings_tray 0
                call gtk_widget_destroy(dialog)
            endelse
        endif

        if bool==1
            import "tray" tray
            sd trayicon
            setcall trayicon tray(programname)
            import "_gtk_main" gtk_main
            call gtk_main()
            import "_g_object_unref" g_object_unref
            call g_object_unref(trayicon)
        endif
    endwhile

    #gtk_init uncloseds
    import "_exit" exit
    call exit(1)
    return 1
endfunction

function getptr_settings_tray()
    data p%ptr
    return p
endfunction

function mainwidget()
    data p%ptr_dialog
    return p#
endfunction

function set_settings_clicked()
    data p%ptr_settings_clicked
    set p# 1
endfunction
