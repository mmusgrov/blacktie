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
#include <cppunit/extensions/HelperMacros.h>
extern "C" {
#include "AtmiBrokerServerControl.h"
#include "AtmiBrokerClientControl.h"
}

#include "ace/OS_NS_stdlib.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_string.h"
#include "xatmi.h"
#include "userlogc.h"
#include "TestAdmin.h"

void TestAdmin::setUp() {
	userlogc((char*) "TestAdmin::setUp");

	char* argv[] = {(char*)"./server", (char*)"-i", (char*)"1", (char*)"foo"};
	int argc = sizeof(argv)/sizeof(char*);

	int initted = serverinit(argc, argv);
	// Check that there is no error on server setup
	CPPUNIT_ASSERT(initted != -1);
	CPPUNIT_ASSERT(tperrno == 0);

}

void TestAdmin::tearDown() {
	userlogc((char*) "TestAdmin::tearDown");
	serverdone();

	clientdone();
	CPPUNIT_ASSERT(tperrno == 0);
}

void TestAdmin::testServerdone() {
	long  sendlen = strlen("serverdone") + 1;
	char* sendbuf = tpalloc((char*) "X_OCTET", NULL, sendlen);
	strcpy(sendbuf, "serverdone");

	char* recvbuf = tpalloc((char*) "X_OCTET", NULL, 1);
	long  recvlen = 1;

	int cd = ::tpcall((char*) "foo_ADMIN_1", (char *) sendbuf, sendlen, (char**)&recvbuf, &recvlen, TPNOTRAN);
	CPPUNIT_ASSERT(cd == 0);
	CPPUNIT_ASSERT(tperrno == 0);
}

void TestAdmin::testAdvertised() {
	long  sendlen = strlen("advertise,BAR,") + 1;
	char* sendbuf = tpalloc((char*) "X_OCTET", NULL, sendlen);
	strcpy(sendbuf, "advertise,BAR,");

	char* recvbuf = tpalloc((char*) "X_OCTET", NULL, 1);
	long  recvlen = 1;

	int cd = ::tpcall((char*) "foo_ADMIN_1", (char *) sendbuf, sendlen, (char**)&recvbuf, &recvlen, TPNOTRAN);
	CPPUNIT_ASSERT(cd == 0);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(recvlen == 1);
	CPPUNIT_ASSERT(recvbuf[0] == '1');
}

void TestAdmin::testUnadvertiseUnknowService() {
	long  sendlen = strlen("unadvertise,UNKNOW,") + 1;
	char* sendbuf = tpalloc((char*) "X_OCTET", NULL, sendlen);
	strcpy(sendbuf, "unadvertise,UNKNOW,");

	char* recvbuf = tpalloc((char*) "X_OCTET", NULL, 1);
	long  recvlen = 1;
	int   cd;

	cd = ::tpcall((char*) "foo_ADMIN_1", (char *) sendbuf, sendlen, (char**)&recvbuf, &recvlen, TPNOTRAN);
	CPPUNIT_ASSERT(cd == 0);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(recvlen == 1);
	CPPUNIT_ASSERT(recvbuf[0] == '0');
}

void TestAdmin::testUnadvertised() {
	long  sendlen = strlen("unadvertise,BAR,") + 1;
	char* sendbuf = tpalloc((char*) "X_OCTET", NULL, sendlen);
	strcpy(sendbuf, "unadvertise,BAR,");

	char* recvbuf = tpalloc((char*) "X_OCTET", NULL, 1);
	long  recvlen = 1;
	int   cd;

	userlogc((char*) "TestAdmin::testUnadvertised tpacall BAR before unadvertise");
	cd = ::tpacall((char*) "BAR", (char *) sendbuf, sendlen, TPNOREPLY);
	CPPUNIT_ASSERT(cd == 0);
	CPPUNIT_ASSERT(tperrno == 0);
	
	cd = ::tpcall((char*) "foo_ADMIN_1", (char *) sendbuf, sendlen, (char**)&recvbuf, &recvlen, TPNOTRAN);
	CPPUNIT_ASSERT(cd == 0);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(recvlen == 1);
	CPPUNIT_ASSERT(recvbuf[0] == '1');

	userlogc((char*) "TestAdmin::testUnadvertised tpacall BAR after unadvertise");
	cd = ::tpacall((char*) "BAR", (char *) sendbuf, sendlen, TPNOREPLY);
	CPPUNIT_ASSERT(cd != 0);
	CPPUNIT_ASSERT(tperrno != 0);
}
