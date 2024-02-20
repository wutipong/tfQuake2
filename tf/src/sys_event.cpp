#include "sys_event.h"

#include "../../qcommon/qcommon.h"

#include "../../client/keys.h"

bool SYS_global_input_handler(InputActionContext *ctx)
{
    switch (ctx->mDeviceType)
    {
    case INPUT_DEVICE_MOUSE:
        break;
    }

    return false;
}