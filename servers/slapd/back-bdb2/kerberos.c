/* kerberos.c - bdb2 backend kerberos bind routines */
/* $OpenLDAP$ */

#include "portable.h"

#ifdef HAVE_KERBEROS

#include <stdio.h>

#include <ac/krb.h>
#include <ac/socket.h>
#include <ac/string.h>

#include "slap.h"
#include "back-bdb2.h"

#define LDAP_KRB_PRINCIPAL	"ldapserver"

extern char		*ldap_srvtab;
extern Attribute	*attr_find();

bdb2i_krbv4_ldap_auth(
    BackendDB		*be,
    struct berval	*cred,
    AUTH_DAT		*ad
)
{
	KTEXT_ST        k;
	KTEXT           ktxt = &k;
	char            instance[INST_SZ];
	int             err;

	Debug( LDAP_DEBUG_TRACE, "=> kerberosv4_ldap_auth\n", 0, 0, 0 );

	SAFEMEMCPY( ktxt->dat, cred->bv_val, cred->bv_len );
	ktxt->length = cred->bv_len;

	strcpy( instance, "*" );
	if ( (err = krb_rd_req( ktxt, LDAP_KRB_PRINCIPAL, instance, 0L, ad,
	    ldap_srvtab )) != KSUCCESS ) {
		Debug( LDAP_DEBUG_ANY, "krb_rd_req failed (%s)\n",
		    krb_err_txt[err], 0, 0 );
		return( LDAP_INVALID_CREDENTIALS );
	}

	return( LDAP_SUCCESS );
}

#endif /* kerberos */
