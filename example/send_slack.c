#include "../cslack.h"

#if 0
example:
https://hooks.slack.com/services/XXXXXXXXXX/YYYYYYYYYY/ZZZZZZZZZZZZ
  
char* url_a = "XXXXXXXXXX";
char* url_b = "YYYYYYYYYY";
char* url_c = "ZZZZZZZZZZ";

slack channel = "#webtest";
username = "webhookbot";
icon = ":ghost:";
#endif

int main (int argc, char** argv){

    Msg *msg;
    Url *url;

    setMsg(msg,"#webtest","webhookbot",":ghost:");
    setUrl(url,"XXXXXXXXXX","YYYYYYYYYY","ZZZZZZZZZZZZ");

    slack_msg_request (url, msg, "Hello from C code :)");

    return 0;
}
