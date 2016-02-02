/*
 * Licence: GPL
 * Created: Thu, 14 Jan 2016 12:05:31 +0100
 * Main authors:
 *     - hoel <hvasseur@openwide.fr>
 */

/*
 * Note: this file originally auto-generated by mib2c using
 *
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "../include/deviceType.h"
#include "../include/mibParameters.h"
#include "../include/handler.h"


void init_deviceType(void)
{
    static oid      deviceType_oid[] = { 1,3,6,1,4,1,35990,3,1,1,9 };

    /*
     * a debugging statement.  Run the agent with -DdeviceType to see
     * the output of this debugging statement.
     */
    DEBUGMSGTL(("deviceType",
                "Initializing the deviceType module\n"));

    /*
     * the line below registers our variables defined above as
     * accessible and makes it writable.  A read only version of any
     * of these registration would merely call
     * register_read_only_long_instance() instead.  The functions
     * called below should be consistent with your MIB, however.
     *
     * If we wanted a callback when the value was retrieved or set
     * (even though the details of doing this are handled for you),
     * you could change the NULL pointer below to a valid handler
     * function.
     */
    DEBUGMSGTL(("deviceType",
                "Initalizing deviceType scalar integer.  Default value = %d\n",
                deviceInfo.parameters[num_DeviceType]._value.int_val ));

    netsnmp_register_read_only_instance(
        netsnmp_create_handler_registration("deviceType", handle_deviceType,
                               deviceType_oid, OID_LENGTH(deviceType_oid),
                               HANDLER_CAN_RONLY
        ));

    DEBUGMSGTL(("deviceType",
                "Done initalizing deviceType module\n"));
}


int
handle_deviceType(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    return handle_ROinteger(handler, reginfo, reqinfo, requests, "deviceType" , &(deviceInfo.parameters[num_DeviceType]._value.int_val) );
}

