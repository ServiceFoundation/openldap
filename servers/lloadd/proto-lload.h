/* $OpenLDAP$ */
/* This work is part of OpenLDAP Software <http://www.openldap.org/>.
 *
 * Copyright 1998-2015 The OpenLDAP Foundation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by the OpenLDAP
 * Public License.
 *
 * A copy of this license is available in the file LICENSE in the
 * top-level directory of the distribution or, alternatively, at
 * <http://www.OpenLDAP.org/license.html>.
 */
/* Portions Copyright (c) 1995 Regents of the University of Michigan.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of Michigan at Ann Arbor. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 */

#ifndef PROTO_LLOAD_H
#define PROTO_LLOAD_H

#include <ldap_cdefs.h>
#include "ldap_pvt.h"

#include <event2/event.h>

LDAP_BEGIN_DECL

struct config_args_s;  /* config.h */
struct config_reply_s; /* config.h */

/*
 * backend.c
 */

LDAP_SLAPD_F (void) backend_connect( evutil_socket_t s, short what, void *arg );
LDAP_SLAPD_F (void *) backend_connect_task( void *ctx, void *arg );
LDAP_SLAPD_F (void) backend_retry( LloadBackend *b );
LDAP_SLAPD_F (LloadConnection *) backend_select( LloadOperation *op );
LDAP_SLAPD_F (void) backends_destroy( void );

/*
 * bind.c
 */
LDAP_SLAPD_F (int) request_bind( LloadConnection *c, LloadOperation *op );
LDAP_SLAPD_F (int) handle_bind_response( LloadOperation *op, BerElement *ber );
LDAP_SLAPD_F (int) handle_vc_bind_response( LloadOperation *op, BerElement *ber );

/*
 * client.c
 */
LDAP_SLAPD_F (int) request_abandon( LloadConnection *c, LloadOperation *op );
LDAP_SLAPD_F (int) request_process( LloadConnection *c, LloadOperation *op );
LDAP_SLAPD_F (int) handle_one_request( LloadConnection *c );
LDAP_SLAPD_F (void) client_tls_handshake_cb( evutil_socket_t s, short what, void *arg );
LDAP_SLAPD_F (LloadConnection *) client_init( ber_socket_t s, LloadListener *url, const char *peername, struct event_base *base, int use_tls );
LDAP_SLAPD_F (void) client_reset( LloadConnection *c );
LDAP_SLAPD_F (void) client_destroy( LloadConnection *c );
LDAP_SLAPD_F (void) clients_destroy( void );

/*
 * config.c
 */
LDAP_SLAPD_F (int) lload_read_config( const char *fname, const char *dir );
LDAP_SLAPD_F (void) lload_config_destroy( void );
LDAP_SLAPD_F (int) verb_to_mask( const char *word, slap_verbmasks *v );
LDAP_SLAPD_F (int) lload_tls_get_config( LDAP *ld, int opt, char **val );
LDAP_SLAPD_F (void) lload_bindconf_tls_defaults( slap_bindconf *bc );
LDAP_SLAPD_F (int) lload_backend_parse( const char *word, LloadBackend *b );
LDAP_SLAPD_F (int) lload_bindconf_parse( const char *word, slap_bindconf *bc );
LDAP_SLAPD_F (int) lload_bindconf_unparse( slap_bindconf *bc, struct berval *bv );
LDAP_SLAPD_F (int) lload_bindconf_tls_set( slap_bindconf *bc, LDAP *ld );
LDAP_SLAPD_F (void) lload_bindconf_free( slap_bindconf *bc );

/*
 * connection.c
 */
LDAP_SLAPD_V (ldap_pvt_thread_mutex_t) clients_mutex;
LDAP_SLAPD_F (void) connection_write_cb( evutil_socket_t s, short what, void *arg );
LDAP_SLAPD_F (void) connection_read_cb( evutil_socket_t s, short what, void *arg );
LDAP_SLAPD_F (LloadConnection *) lload_connection_init( ber_socket_t s, const char *peername, int use_tls );
LDAP_SLAPD_F (void) connection_destroy( LloadConnection *c );

/*
 * daemon.c
 */
LDAP_SLAPD_F (int) lloadd_daemon_init( const char *urls );
LDAP_SLAPD_F (int) lloadd_daemon_destroy( void );
LDAP_SLAPD_F (int) lloadd_daemon( struct event_base *daemon_base );
LDAP_SLAPD_F (LloadListener **) lloadd_get_listeners( void );
LDAP_SLAPD_F (void) listeners_reactivate( void );
LDAP_SLAPD_F (struct event_base *) lload_get_base( ber_socket_t s );
LDAP_SLAPD_V (int) lload_daemon_threads;
LDAP_SLAPD_V (int) lload_daemon_mask;

LDAP_SLAPD_F (void) lload_sig_shutdown( evutil_socket_t sig, short what, void *arg );

LDAP_SLAPD_V (struct evdns_base *) dnsbase;
LDAP_SLAPD_V (volatile sig_atomic_t) slapd_shutdown;
LDAP_SLAPD_V (int) lloadd_inited;

LDAP_SLAPD_V (struct event *) lload_timeout_event;

LDAP_SLAPD_V (LDAP *) lload_tls_backend_ld;
LDAP_SLAPD_V (LDAP *) lload_tls_ld;
LDAP_SLAPD_V (void *) lload_tls_ctx;

/*
 * extended.c
 */
LDAP_SLAPD_V (Avlnode *) lload_exop_handlers;
LDAP_SLAPD_F (int) exop_handler_cmp( const void *l, const void *r );
LDAP_SLAPD_F (int) request_extended( LloadConnection *c, LloadOperation *op );
LDAP_SLAPD_F (int) lload_exop_init( void );

/*
 * init.c
 */
LDAP_SLAPD_F (int) lload_init( int mode, const char *name );
LDAP_SLAPD_F (int) lload_destroy( void );

/*
 * libevent_support.c
 */
LDAP_SLAPD_F (int) lload_libevent_init( void );
LDAP_SLAPD_F (void) lload_libevent_destroy( void );

/*
 * operation.c
 */
LDAP_SLAPD_F (const char *) lload_msgtype2str( ber_tag_t tag );
LDAP_SLAPD_F (int) operation_upstream_cmp( const void *l, const void *r );
LDAP_SLAPD_F (int) operation_client_cmp( const void *l, const void *r );
LDAP_SLAPD_F (LloadOperation *) operation_init( LloadConnection *c, BerElement *ber );
LDAP_SLAPD_F (int) operation_send_abandon( LloadOperation *op );
LDAP_SLAPD_F (void) operation_abandon( LloadOperation *op );
LDAP_SLAPD_F (void) operation_send_reject( LloadOperation *op, int result, const char *msg, int send_anyway );
LDAP_SLAPD_F (int) operation_send_reject_locked( LloadOperation *op, int result, const char *msg, int send_anyway );
LDAP_SLAPD_F (void) operation_lost_upstream( LloadOperation *op );
LDAP_SLAPD_F (void) operation_destroy_from_client( LloadOperation *op );
LDAP_SLAPD_F (void) operation_destroy_from_upstream( LloadOperation *op );
LDAP_SLAPD_F (void) operations_timeout( evutil_socket_t s, short what, void *arg );

/*
 * upstream.c
 */
LDAP_SLAPD_F (int) forward_final_response( LloadOperation *op, BerElement *ber );
LDAP_SLAPD_F (int) forward_response( LloadOperation *op, BerElement *ber );
LDAP_SLAPD_F (LloadConnection *) upstream_init( ber_socket_t s, LloadBackend *b );
LDAP_SLAPD_F (void) upstream_destroy( LloadConnection *c );

LDAP_SLAPD_V (ber_len_t) sockbuf_max_incoming_client;
LDAP_SLAPD_V (ber_len_t) sockbuf_max_incoming_upstream;
LDAP_SLAPD_V (int) lload_conn_max_pdus_per_cycle;

LDAP_SLAPD_V (lload_features_t) lload_features;

LDAP_SLAPD_V (slap_mask_t) global_allows;
LDAP_SLAPD_V (slap_mask_t) global_disallows;

LDAP_SLAPD_V (const char) Versionstr[];

LDAP_SLAPD_V (int) global_gentlehup;
LDAP_SLAPD_V (int) global_idletimeout;

LDAP_SLAPD_V (struct timeval *) lload_timeout_api;
LDAP_SLAPD_V (struct timeval *) lload_timeout_net;
LDAP_SLAPD_V (struct timeval *) lload_write_timeout;

LDAP_SLAPD_V (char *) global_host;
LDAP_SLAPD_V (int) lber_debug;
LDAP_SLAPD_V (int) ldap_syslog;

LDAP_END_DECL

#endif /* PROTO_LLOAD_H */
