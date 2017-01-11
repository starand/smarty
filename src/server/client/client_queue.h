#pragma once

#include <lock_queue.h>


class socket_t;

class clients_queue_t : public lock_queue_t < socket_t* >
{
};