/*
 * Note: this file originally auto-generated by mib2c using
 *        $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "../../include/channelControl/channelNumber.h"
#include "../../include/mibParameters.h"
#include "../../include/handler.h"

/** 
 * \brief Initializes the channelNumber module
 */
void
init_channelNumber(void)
{
    const oid channelNumber_oid[] = { 1,3,6,1,4,1,35990,3,1,3,2 };

  DEBUGMSGTL(("channelNumber", "Initializing"));

    netsnmp_register_instance(
        netsnmp_create_handler_registration("channelNumber", handle_channelNumber,
                               channelNumber_oid, OID_LENGTH(channelNumber_oid),
                               HANDLER_CAN_RONLY
        ));
}

/** 
 * \brief calls appropriate handler for this parameter
 * \param handler the specific handler for this item
 * \param reqinfo the SNMP request
 * \param reuests the resuest information
 * \param mib_parameter the parameter of the MIB
 * \return SNMP_ERR_NOERROR or approriate code error
 */
int
handle_channelNumber(netsnmp_mib_handler *handler,
                          netsnmp_handler_registration *reginfo,
                          netsnmp_agent_request_info   *reqinfo,
                          netsnmp_request_info         *requests)
{
	return handle_ROinteger(handler, reginfo, reqinfo, requests, &channelNumber );
}
