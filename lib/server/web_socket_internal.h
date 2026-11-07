#ifndef WEB_SOCKET_INTERNAL_H
#define WEB_SOCKET_INTERNAL_H


void websocket_init();
void websocket_run();

static bool websocket_queue_message(const char* message);
void web_socket_broadcast_text(const String message);

#endif