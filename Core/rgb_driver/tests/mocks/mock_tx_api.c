#include "mock_tx_api.h"
#include "tx_api.h"


UINT  _txe_event_flags_create(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr, UINT event_control_block_size)
{
    return 0;
}

UINT  _txe_event_flags_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested_flags,
                    UINT get_option, ULONG *actual_flags_ptr, ULONG wait_option)
{
    return 0;
}

UINT  _txe_event_flags_set(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags_to_set, UINT set_option)
{
    return 0;
}

UINT  _txe_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr,
            VOID (*expiration_function)(ULONG id), ULONG expiration_input,
            ULONG initial_ticks, ULONG reschedule_ticks, UINT auto_activate, UINT timer_control_block_size)
{
    return 0;
}

UINT  _txe_timer_deactivate(TX_TIMER *timer_ptr)
{
    return 0;
}

UINT  _txe_timer_change(TX_TIMER *timer_ptr, ULONG initial_ticks, ULONG reschedule_ticks)
{
    return 0;
}

UINT  _txe_timer_activate(TX_TIMER *timer_ptr)
{
    return 0;
}
