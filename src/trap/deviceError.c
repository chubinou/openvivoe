/*
 * Note: this file originally auto-generated by mib2c using
 *        $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "trap/deviceError.h"

extern const oid snmptrap_oid[];
extern const size_t snmptrap_oid_len;

/**
 * \brief send a TRAP message with the content of deviceUserDesc or deviceMode if a unexpected error occurs 
 * \return SNMP_ERR_NOERROR
 */
int
send_deviceError_trap( void )
{
    netsnmp_variable_list  *var_list = NULL;
    const oid deviceError_oid[] = { 1,3,6,1,4,1,35990,3,1,4,1 };
    const oid deviceUserDesc_oid[] = { 1,3,6,1,4,1,35990,3,1,1,10, 0 };
    const oid deviceMode_oid[] = { 1,3,6,1,4,1,35990,3,1,1,14, 0 };

    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        deviceError_oid, sizeof(deviceError_oid));

    /*
     * Add any objects from the trap definition, here deviceUserDesc and deviceMode
     */
    snmp_varlist_add_variable(&var_list,
        deviceUserDesc_oid, OID_LENGTH(deviceUserDesc_oid),
        ASN_OCTET_STR,
        /* Set an appropriate value for deviceUserDesc */
        NULL, 0);
    snmp_varlist_add_variable(&var_list,
        deviceMode_oid, OID_LENGTH(deviceMode_oid),
        ASN_INTEGER,
        /* Set an appropriate value for deviceMode */
        NULL, 0);

    /*
     * Add any extra (optional) objects here, not used for now 
     */

    /*
     * Send the trap to the list of configured destinations
     *  and clean up
     */
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
