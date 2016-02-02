/*
 * Note: this file originally auto-generated by mib2c using
 *        $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "../include/deviceNatoStockNumber.h"
#include "../include/handler.h"
#include "../include/mibParameters.h"




/** Initializes the deviceNatoStockNumber module */
void
init_deviceNatoStockNumber(void)
{
    const oid deviceNatoStockNumber_oid[] = { 1,3,6,1,4,1,35990,3,1,1,13 };

  DEBUGMSGTL(("deviceNatoStockNumber", "Initializing\n"));

    netsnmp_register_read_only_instance(
                                        netsnmp_create_handler_registration("deviceNatoStockNumber", handle_deviceNatoStockNumber,
                                        deviceNatoStockNumber_oid, OID_LENGTH(deviceNatoStockNumber_oid),
                                        HANDLER_CAN_RONLY ));
}

int
handle_deviceNatoStockNumber(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
    return handle_ROstring32(handler, reginfo, reqinfo, requests, "deviceNatoStockNumber" ,  deviceInfo.parameters[num_DeviceNato]._value.string_val);
}
