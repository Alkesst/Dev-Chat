#include "chat.h"

#include <stdbool.h>

bool open_server(struct Server* server, int port);

void close_server(struct Server* server);

void start_polling(struct Server* server);
