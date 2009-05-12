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
#include "BaseTest.h"
#include "XATMITestSuite.h"

#include "xatmi.h"
#include "malloc.h"
#include "TestTPTypes.h"

void TestTPTypes::setUp() {
	userlogc((char*) "TestTPTypes::setUp");
	BaseTest::setUp();
	// Do local work
	m_allocated = NULL;
}

void TestTPTypes::tearDown() {
	userlogc((char*) "TestTPTypes::setUp");
	// Do local work
	if (m_allocated != NULL) {
		::tpfree(m_allocated);
	}

	BaseTest::tearDown();
}

void TestTPTypes::test_tptypes_x_octet() {
	userlogc((char*) "test_tptypes_x_octet");
	m_allocated = ::tpalloc((char*) "X_OCTET", NULL, 10);
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* type = (char*) malloc(8);
	char* subtype = (char*) malloc(16);
	int toTest = ::tptypes(m_allocated, type, subtype);
	CPPUNIT_ASSERT(strcmp(type, "X_OCTET") == 0);
	CPPUNIT_ASSERT(strcmp(subtype, "") == 0);
	free(type);
	free(subtype);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == 10);
}

void TestTPTypes::test_tptypes_x_common() {
	userlogc((char*) "test_tptypes_x_common");
	m_allocated = ::tpalloc((char*) "X_COMMON", (char*) "deposit", sizeof(DEPOSIT));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* type = (char*) malloc(8);
	char* subtype = (char*) malloc(16);
	int toTest = ::tptypes(m_allocated, type, subtype);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == 1024);
	CPPUNIT_ASSERT(strncmp(type, "X_COMMON", 8) == 0);
	CPPUNIT_ASSERT(strcmp(subtype, "deposit") == 0);
	free(type);
	free(subtype);
}

void TestTPTypes::test_tptypes_x_common_bigdata() {
	userlogc((char*) "test_tptypes_x_common_bigdata");
	m_allocated = ::tpalloc((char*) "X_COMMON", (char*) "deposit", sizeof(BIGDATA));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* type = (char*) malloc(8);
	char* subtype = (char*) malloc(16);
	int toTest = ::tptypes(m_allocated, type, subtype);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == sizeof(BIGDATA));
	CPPUNIT_ASSERT(strncmp(type, "X_COMMON", 8) == 0);
	CPPUNIT_ASSERT(strcmp(subtype, "deposit") == 0);
	free(type);
	free(subtype);
}

void TestTPTypes::test_tptypes_x_c_type() {
	userlogc((char*) "test_tptypes_x_c_type");
	m_allocated = ::tpalloc((char*) "X_C_TYPE", (char*) "acct_info", sizeof(ACCT_INFO));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* type = (char*) malloc(8);
	char* subtype = (char*) malloc(16);
	int toTest = ::tptypes(m_allocated, type, subtype);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == 1024);
	CPPUNIT_ASSERT(strncmp(type, "X_C_TYPE", 8) == 0);
	CPPUNIT_ASSERT(strcmp(subtype, "acct_info") == 0);
	free(type);
	free(subtype);
}

void TestTPTypes::test_tptypes_x_c_type_bigdata() {
	userlogc((char*) "test_tptypes_x_c_type_bigdata");
	m_allocated = ::tpalloc((char*) "X_C_TYPE", (char*) "acct_info", sizeof(BIGDATA));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* type = (char*) malloc(8);
	char* subtype = (char*) malloc(16);
	int toTest = ::tptypes(m_allocated, type, subtype);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == sizeof(BIGDATA));
	CPPUNIT_ASSERT(strncmp(type, "X_C_TYPE", 8) == 0);
	CPPUNIT_ASSERT(strcmp(subtype, "acct_info") == 0);
	free(type);
	free(subtype);
}

// 8.2
void TestTPTypes::test_tptypes_unallocated() {
	userlogc((char*) "test_tptypes_unallocated");
	char* type = (char*) malloc(8);
	char* subtype = (char*) malloc(16);
	int toTest = ::tptypes((char*) "test", type, subtype);
	CPPUNIT_ASSERT(tperrno== TPEINVAL);
	CPPUNIT_ASSERT(toTest == -1);
	free(type);
	free(subtype);
}

void TestTPTypes::test_tptypes_null_ptr() {
	userlogc((char*) "test_tptypes_null_ptr");
	char* type = (char*) malloc(8);
	char* subtype = (char*) malloc(16);
	int toTest = ::tptypes(NULL, type, subtype);
	CPPUNIT_ASSERT(tperrno== TPEINVAL);
	CPPUNIT_ASSERT(toTest == -1);
	free(type);
	free(subtype);
}

void TestTPTypes::test_tptypes_null_type() {
	userlogc((char*) "test_tptypes_null_type");
	m_allocated = ::tpalloc((char*) "X_COMMON", (char*) "deposit", sizeof(BIGDATA));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* subtype = (char*) malloc(16);
	int toTest = ::tptypes(m_allocated, NULL, subtype);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == sizeof(BIGDATA));
	CPPUNIT_ASSERT(strcmp(subtype, "deposit") == 0);
	free(subtype);
}

void TestTPTypes::test_tptypes_null_subtype() {
	userlogc((char*) "test_tptypes_null_subtype");
	m_allocated = ::tpalloc((char*) "X_COMMON", (char*) "deposit", sizeof(BIGDATA));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* type = (char*) malloc(8);
	int toTest = ::tptypes(m_allocated, type, NULL);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == sizeof(BIGDATA));
	CPPUNIT_ASSERT(strncmp(type, "X_COMMON", 8) == 0);
	free(type);
}

void TestTPTypes::test_tptypes_max_type() {
	userlogc((char*) "test_tptypes_max_type");
	m_allocated = ::tpalloc((char*) "X_COMMON", (char*) "deposit", sizeof(BIGDATA));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* type = (char*) malloc(8);
	int toTest = ::tptypes(m_allocated, type, NULL);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == sizeof(BIGDATA));
	CPPUNIT_ASSERT(strncmp(type, "X_COMMON", 8) == 0);
	free(type);
}

void TestTPTypes::test_tptypes_max_subtype() {
	userlogc((char*) "test_tptypes_max_subtype");
	m_allocated = ::tpalloc((char*) "X_COMMON", (char*) "1234567890123456", sizeof(BIGDATA));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* subtype = (char*) malloc(16);
	int toTest = ::tptypes(m_allocated, NULL, subtype);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == sizeof(BIGDATA));
	CPPUNIT_ASSERT(strncmp(subtype, "1234567890123456", 16) == 0);
	free(subtype);
}

void TestTPTypes::test_tptypes_small_type() { // cannot be tested as we can't find how big the memory is
	userlogc((char*) "test_tptypes_small_type");
	m_allocated = ::tpalloc((char*) "X_COMMON", (char*) "deposit", sizeof(BIGDATA));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* type = (char*) malloc(7);
	int toTest = ::tptypes(m_allocated, type, NULL);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == sizeof(BIGDATA));
	CPPUNIT_ASSERT(strncmp(type, "X_COMMON", 8) == 0);
	CPPUNIT_ASSERT(strncmp(type, "X_COMMO", 7) == 0);
	free(type);
}

void TestTPTypes::test_tptypes_small_subtype() { // cannot be tested as we can't find how big the memory is
	userlogc((char*) "test_tptypes_small_subtype");
	m_allocated = ::tpalloc((char*) "X_COMMON", (char*) "1234567890123456", sizeof(BIGDATA));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* subtype = (char*) malloc(15);
	int toTest = ::tptypes(m_allocated, NULL, subtype);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == sizeof(BIGDATA));
	CPPUNIT_ASSERT(strncmp(subtype, "1234567890123456", 16) == 0);
	CPPUNIT_ASSERT(strncmp(subtype, "123456789012345", 15) == 0);
	free(subtype);
}

void TestTPTypes::test_tptypes_large_type() {
	userlogc((char*) "test_tptypes_large_type");

	m_allocated = ::tpalloc((char*) "X_COMMON", (char*) "deposit", sizeof(BIGDATA));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* type = (char*) malloc(9);
	int toTest = ::tptypes(m_allocated, type, NULL);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == sizeof(BIGDATA));
	CPPUNIT_ASSERT(strncmp(type, "X_COMMON", 8) == 0);
	free(type);
}

void TestTPTypes::test_tptypes_large_subtype() {
	userlogc((char*) "test_tptypes_large_subtype");
	m_allocated = ::tpalloc((char*) "X_COMMON", (char*) "1234567890123456", sizeof(BIGDATA));
	CPPUNIT_ASSERT(m_allocated != NULL);

	char* subtype = (char*) malloc(17);
	int toTest = ::tptypes(m_allocated, NULL, subtype);
	CPPUNIT_ASSERT(tperrno == 0);
	CPPUNIT_ASSERT(toTest == sizeof(BIGDATA));
	CPPUNIT_ASSERT(strncmp(subtype, "1234567890123456", 16) == 0);
	free(subtype);
}
