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

#include "log4cxx/logger.h"
#include "ace/OS_NS_stdlib.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_string.h"
#include "AtmiBrokerServerControl.h"
#include "xatmi.h"
#include "userlog.h"
#include "string.h"

log4cxx::LoggerPtr loggerAtmiBrokerAdmin(log4cxx::Logger::getLogger(
			"AtmiBrokerAdmin"));

void ADMIN(TPSVCINFO* svcinfo) {
	char* toReturn = NULL;
	long  len = 1;
	char* req = svcinfo->data;
	toReturn = tpalloc((char*) "X_OCTET", NULL, len);
	toReturn[0] = '0';

	strtok(req, ",");
	char* svc = strtok(NULL, ",");

	if(strncmp(req, "serverdone", 10) == 0) {
		LOG4CXX_DEBUG(loggerAtmiBrokerAdmin, (char*) "get serverdone command");
		toReturn[0] = '1';
		server_sigint_handler_callback(0);
	} else if(strncmp(req, "advertise", 9) == 0) {
		LOG4CXX_DEBUG(loggerAtmiBrokerAdmin, (char*) "get advertise command");
		if(svc != NULL && strcmp(svc, svcinfo->name) != 0 && advertiseByAdmin(svc) == 0) {
			LOG4CXX_DEBUG(loggerAtmiBrokerAdmin, (char*) "advertise service " << svc << " OK");
			toReturn[0] = '1';
		} else {
			LOG4CXX_WARN(loggerAtmiBrokerAdmin, (char*) "advertise service " << svc << " FAIL");
		}
	} else if(strncmp(req, "unadvertise", 11) == 0) {
		LOG4CXX_DEBUG(loggerAtmiBrokerAdmin, (char*) "get unadvertise command");
		if (svc != NULL && strcmp(svc, svcinfo->name) != 0 && tpunadvertise(svc) == 0) {
			toReturn[0] = '1';
			LOG4CXX_DEBUG(loggerAtmiBrokerAdmin, (char*) "unadvertise service " << svc << " OK");
		} else {
			LOG4CXX_WARN(loggerAtmiBrokerAdmin, (char*) "unadvertise service " << svc << " FAIL");
		}
	} else if(strncmp(req, "status", 6) == 0) {
		LOG4CXX_DEBUG(loggerAtmiBrokerAdmin, (char*) "get status command");
		char* status = NULL;

		len += getServiceStatus(&status, svc) + 1;
		if (len > 1 && status != NULL) {
			toReturn = tprealloc(toReturn, len + 1);
			ACE_OS::memcpy(&toReturn[1], status, len);
			free(status);
			toReturn[0] = '1';
		} else {
			LOG4CXX_WARN(loggerAtmiBrokerAdmin, (char*) "get server status FAIL");
		}
	} else if(strncmp(req, "counter", 7) == 0) {
		LOG4CXX_DEBUG(loggerAtmiBrokerAdmin, (char*) "get counter command");
		long counter = 0;

		if(svc != NULL) {
			toReturn = tprealloc(toReturn, 16);
			counter = getServiceMessageCounter(svc);
			len += ACE_OS::sprintf(&toReturn[1], "%ld", counter);
			toReturn[0] = '1';
		} else {
			LOG4CXX_WARN(loggerAtmiBrokerAdmin, (char*) "get counter FAIL");
		}
	} else if(strncmp(req, "pause", 5) == 0) {
		LOG4CXX_DEBUG(loggerAtmiBrokerAdmin, (char*) "get pause command");
		if(pauseServerByAdmin() == 0) {
			toReturn[0] = '1';
		}
	} else if(strncmp(req, "resume", 6) == 0) {
		LOG4CXX_DEBUG(loggerAtmiBrokerAdmin, (char*) "get resume command");
		if(resumeServerByAdmin() == 0) {
			toReturn[0] = '1';
		}
	}

	userlog(log4cxx::Level::getDebug(), loggerAtmiBrokerAdmin,
			(char*) "service done");
	tpreturn(TPSUCCESS, 0, toReturn, len, 0);
}