/*
 * Note: this file originally auto-generated by mib2c using
 *        $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "../include/deviceReset.h"
#include "../include/mibParameters.h"
#include "../include/handler.h"


/** Initializes the deviceReset module */
void
init_deviceReset(void)
{
    const oid deviceReset_oid[] = { 1,3,6,1,4,1,35990,3,1,1,15 };

  DEBUGMSGTL(("deviceReset", "Initializing\n"));

    netsnmp_register_instance(
        netsnmp_create_handler_registration("deviceReset", handle_deviceReset,
                               deviceReset_oid, OID_LENGTH(deviceReset_oid),
                               HANDLER_CAN_RWRITE
        ));
}

int
handle_deviceReset(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
        return handle_RWinteger(handler, reginfo, reqinfo, requests, "deviceReset" ,&(deviceInfo.parameters[num_DeviceReset]._value.int_val));
}
