/*
 * JBoss, Home of Professional Open Source
 * Copyright 2008, Red Hat, Inc., and others contributors as indicated
 * by the @authors tag. All rights reserved.
 * See the copyright.txt in the distribution for a
 * full listing of individual contributors.
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU Lesser General Public License, v. 2.1.
 * This program is distributed in the hope that it will be useful, but WITHOUT A
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License,
 * v.2.1 along with this distribution; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */
#include <stdlib.h>

#include "xatmi.h"
#include "btxatmi.h"
#include "userlogc.h"
#include "string.h"

void BAR(TPSVCINFO * svcinfo) {
	char* buffer;
	int sendlen;
	int rc;
	msg_opts_t mopts;
	
	userlogc((char*) "bar called  - svc: %s data %s len: %d flags: %d", svcinfo->name, svcinfo->data, svcinfo->len, svcinfo->flags);

	mopts.priority = 0;
	mopts.ttl = 0;
	rc = btenqueue((char*) "TestOne", &mopts, svcinfo->data, svcinfo->len, 0);

	sendlen = 15;
	buffer = tpalloc("X_OCTET", 0, sendlen);
	(void) sprintf(buffer, (char*) "Enqueue RC: %d", rc);
	tpreturn(TPSUCCESS, 0, buffer, sendlen, 0);
}