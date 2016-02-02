/*
 * Note: this file originally auto-generated by mib2c using
 *        $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "../include/deviceSerialNumber.h"
#include "../include/macro.h"
#include "../include/handler.h"
#include "../include/mibParameters.h"


/** Initializes the deviceSerialNumber module */
void
init_deviceSerialNumber(void)
{
    const oid deviceSerialNumber_oid[] = { 1,3,6,1,4,1,35990,3,1,1,4 };

  DEBUGMSGTL(("deviceSerialNumber", "Initializing\n"));

    netsnmp_register_read_only_instance(
        netsnmp_create_handler_registration("deviceSerialNumber", handle_deviceSerialNumber,
                               deviceSerialNumber_oid, OID_LENGTH(deviceSerialNumber_oid),
                               HANDLER_CAN_RONLY
        ));
}

int
handle_deviceSerialNumber(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    return handle_ROstring32(handler, reginfo, reqinfo, requests, "deviceSerialNumber" , deviceInfo.parameters[num_deviceSN]._value.string_val );
}
