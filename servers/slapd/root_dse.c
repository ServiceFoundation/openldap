/* $OpenLDAP$ */
/* root_dse.c - Provides the ROOT DSA-Specific Entry
 *
 * Copyright 1999 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted only
 * as authorized by the OpenLDAP Public License.  A copy of this
 * license is available at http://www.OpenLDAP.org/license.html or
 * in file LICENSE in the top-level directory of the distribution.
 */

#include "portable.h"

#include <stdio.h>
#include <ac/string.h>

#include "slap.h"

void
root_dse_info( Connection *conn, Operation *op, char **attrs, int attrsonly )
{
	char buf[BUFSIZ];
	Entry		*e;
	struct berval	val;
	struct berval	*vals[2];
	int		i, j;

	vals[0] = &val;
	vals[1] = NULL;

	e = (Entry *) ch_calloc( 1, sizeof(Entry) );

	e->e_attrs = NULL;
	e->e_dn = ch_strdup( LDAP_ROOT_DSE );
	e->e_ndn = ch_strdup( LDAP_ROOT_DSE );
	(void) dn_normalize( e->e_ndn );
	e->e_private = NULL;

	for ( i = 0; i < nbackends; i++ ) {
		for ( j = 0; backends[i].be_suffix[j] != NULL; j++ ) {
			val.bv_val = backends[i].be_suffix[j];
			val.bv_len = strlen( val.bv_val );
			attr_merge( e, "namingContexts", vals );
		}
	}

#ifdef SLAPD_SCHEMA_NOT_COMPAT
	/* we shouldn't publish subentries as naming contexts */
#else
#if defined( SLAPD_MONITOR_DN )
	val.bv_val = SLAPD_MONITOR_DN;
	val.bv_len = strlen( val.bv_val );
	attr_merge( e, "namingContexts", vals );
	/* subschemasubentry is added by send_search_entry() */
#endif

#if defined( SLAPD_CONFIG_DN )
	val.bv_val = SLAPD_CONFIG_DN;
	val.bv_len = strlen( val.bv_val );
	attr_merge( e, "namingContexts", vals );
#endif

#if defined( SLAPD_SCHEMA_DN )
	val.bv_val = SLAPD_SCHEMA_DN;
	val.bv_len = strlen( val.bv_val );
	attr_merge( e, "namingContexts", vals );
#endif
#endif

	/* altServer unsupported */

	/* supportedControl */
	for ( i=0; supportedControls[i] != NULL; i++ ) {
		val.bv_val = supportedControls[i];
		val.bv_len = strlen( val.bv_val );
		attr_merge( e, "supportedControl", vals );
	}

	/* supportedExtension */
	for ( i=0; (val.bv_val = get_supported_extop(i)) != NULL; i++ ) {
		val.bv_len = strlen( val.bv_val );
		attr_merge( e, "supportedExtension", vals );
	}

	/* supportedLDAPVersion */
	for ( i=LDAP_VERSION_MIN; i<=LDAP_VERSION_MAX; i++ ) {
		sprintf(buf,"%d",i);
		val.bv_val = buf;
		val.bv_len = strlen( val.bv_val );
		attr_merge( e, "supportedLDAPVersion", vals );
	}

	/* supportedSASLMechanism */
	if( supportedSASLMechanisms != NULL ) {
		for ( i=0; supportedSASLMechanisms[i] != NULL; i++ ) {
			val.bv_val = supportedSASLMechanisms[i];
			val.bv_len = strlen( val.bv_val );
			attr_merge( e, "supportedSASLMechanisms", vals );
		}
	}

#ifdef SLAPD_ACI_ENABLED
	/* supportedACIMechanisms */
	for ( i=0; (val.bv_val = get_supported_acimech(i)) != NULL; i++ ) {
		val.bv_len = strlen( val.bv_val );
		attr_merge( e, "supportedACIMechanisms", vals );
	}
#endif

	if ( default_referral != NULL ) {
		attr_merge( e, "ref", default_referral );
	}

	val.bv_val = "top";
	val.bv_len = sizeof("top")-1;
	attr_merge( e, "objectClass", vals );

	val.bv_val = "LDAProotDSE";
	val.bv_len = sizeof("LDAProotDSE")-1;
	attr_merge( e, "objectClass", vals );

	val.bv_val = "extensibleObject";
	val.bv_len = sizeof("extensibleObject")-1;
	attr_merge( e, "objectClass", vals );

	send_search_entry( &backends[0], conn, op,
		e, attrs, attrsonly, NULL );
	send_search_result( conn, op, LDAP_SUCCESS,
		NULL, NULL, NULL, NULL, 1 );

	entry_free( e );
}

