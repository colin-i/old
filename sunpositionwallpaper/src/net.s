



format elfobj

include "./constants.h"

import "_soup_session_sync_new" soup_session_sync_new
import "_soup_message_new" soup_message_new
import "_soup_session_send_message" soup_session_send_message

function getSessionMessageBody(data sessionMsg,data ptrmsgmem,data ptrmsgsize)
#    GObject             parent
#All the fields in the GObject structure are private to the GObject implementation and should never be accessed directly.
#    const char         *method;

#    guint               status_code;
#    char               *reason_phrase;

#    SoupMessageBody    *request_body;
#    SoupMessageHeaders *request_headers;

#    SoupMessageBody    *response_body;
#    SoupMessageHeaders *response_headers;
    data offset=3+5*4
    add sessionMsg offset

    data response_body#1
    set response_body sessionMsg#


#        const char *data;
#        goffset     length;  (gint64)
    set ptrmsgmem# response_body#
    data valuesize=4
    data greatest=8
    add response_body valuesize
    import "system_variables_alignment_pad" system_variables_alignment_pad
    addcall response_body system_variables_alignment_pad(valuesize,greatest)
    set ptrmsgsize# response_body#
endfunction

#err
function uri_get_content(str uri,data ptrmsg,data ptrmsgmem,data ptrmsgsize)
    data session#1
    setcall session soup_session_sync_new()

    str get="GET"
    setcall ptrmsg# soup_message_new(get,uri)

    data ok=HTTP_STATUS_OK
    data status#1
    setcall status soup_session_send_message(session,ptrmsg#)
    if status!=ok
        str urierr="Error status code: "
        import "strvaluedisp" strvaluedisp
        data su=stringUinteger
        call strvaluedisp(urierr,status,su)
        return urierr
    endif

    import "_g_object_unref" g_object_unref
    call g_object_unref(session)

    call getSessionMessageBody(ptrmsg#,ptrmsgmem,ptrmsgsize)

    data noerr=noerror
    return noerr
endfunction


#e/forward
function uri_get_content_forward_data(ss uri,sd forward,sd data)
#                        forward body and size
    data err#1
    data noerr=noerror
    sd msg
    sd body
    sd size
    sd ptrmsg^msg
    sd ptrbody^body
    sd ptrsize^size
    setcall err uri_get_content(uri,ptrmsg,ptrbody,ptrsize)
    if err!=noerr
        return err
    endif
    setcall err forward(body,size,data)
    call g_object_unref(msg)
    return err
endfunction

#e
function uri_get_content_forward(ss uri,sd forward)
    data null=0
    sd err
    setcall err uri_get_content_forward_data(uri,forward,null)
    return err
endfunction
