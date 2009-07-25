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

#include <string.h>
#include <exception>

#include "apr_strings.h"

#include "malloc.h"
#include "SessionImpl.h"
#include "CorbaEndpointQueue.h"
#include "StompEndpointQueue.h"
#include "txClient.h"

#include "ThreadLocalStorage.h"
#include "AtmiBrokerEnv.h"

log4cxx::LoggerPtr HybridSessionImpl::logger(log4cxx::Logger::getLogger(
		"HybridSessionImpl"));

HybridSessionImpl::HybridSessionImpl(CORBA_CONNECTION* connection,
		apr_pool_t* pool, int id, char* serviceName) {
	LOG4CXX_TRACE(logger, (char*) "constructor service");
	this->id = id;
	this->corbaConnection = connection;
	serviceInvokation = true;

	stompConnection = NULL;
	std::string timeout = AtmiBrokerEnv::get_instance()->getenv(
			(char*) "RequestTimeout");
	stompConnection
			= HybridConnectionImpl::connect(pool, atoi(timeout.c_str())); // TODO allow the timeout to be specified in configuration
	this->pool = pool;
	// XATMI_SERVICE_NAME_LENGTH is in xatmi.h and therefore not accessible
	int XATMI_SERVICE_NAME_LENGTH = 15;
	this->sendTo = (char*) ::malloc(7 + XATMI_SERVICE_NAME_LENGTH + 1);
	memset(this->sendTo, '\0', 7 + XATMI_SERVICE_NAME_LENGTH + 1);
	strcpy(this->sendTo, "/queue/");
	strncat(this->sendTo, serviceName, XATMI_SERVICE_NAME_LENGTH);

	this->canSend = true;
	this->canRecv = true;

	this->temporaryQueue = new HybridCorbaEndpointQueue(corbaConnection);
	this->replyTo = temporaryQueue->getName();
	LOG4CXX_TRACE(logger, "OK service session created");
}

HybridSessionImpl::HybridSessionImpl(CORBA_CONNECTION* connection, int id,
		const char* temporaryQueueName) {
	LOG4CXX_DEBUG(logger, (char*) "constructor corba");
	this->id = id;
	this->corbaConnection = connection;
	serviceInvokation = false;

	stompConnection = NULL;
	this->sendTo = NULL;

	LOG4CXX_DEBUG(logger, (char*) "EndpointQueue: " << temporaryQueueName);
	CORBA::ORB_ptr orb = (CORBA::ORB_ptr) corbaConnection->orbRef;
	CORBA::Object_var tmp_ref = orb->string_to_object(temporaryQueueName);
	remoteEndpoint = AtmiBroker::EndpointQueue::_narrow(tmp_ref);
	LOG4CXX_DEBUG(logger, (char*) "connected to %s" << temporaryQueueName);

	this->canSend = true;
	this->canRecv = true;

	this->temporaryQueue = new HybridCorbaEndpointQueue(corbaConnection);
	this->replyTo = temporaryQueue->getName();
	LOG4CXX_DEBUG(logger, (char*) "constructor corba done");
}

HybridSessionImpl::~HybridSessionImpl() {
	setSendTo(NULL);
	//delete temporaryQueue;

	if (stompConnection) {
		LOG4CXX_TRACE(logger, (char*) "destroying");
		HybridConnectionImpl::disconnect(stompConnection, pool);
		LOG4CXX_TRACE(logger, (char*) "destroyed");
		stompConnection = NULL;
	}
}

void HybridSessionImpl::setSendTo(const char* destinationName) {
	if (this->sendTo != NULL) {
		if (destinationName == NULL || strcmp(destinationName, this->sendTo)
				!= 0) {
			::free(this->sendTo);
			this->sendTo = NULL;
		}
	}
	//	if (remoteEndpoint) {
	//		remoteEndpoint = NULL;
	//	}
	if (destinationName != NULL && strcmp(destinationName, "") != 0
			&& this->sendTo == NULL) {
		CORBA::ORB_ptr orb = (CORBA::ORB_ptr) corbaConnection->orbRef;
		LOG4CXX_DEBUG(logger, (char*) "EndpointQueue: " << destinationName);
		CORBA::Object_var tmp_ref = orb->string_to_object(destinationName);
		remoteEndpoint = AtmiBroker::EndpointQueue::_narrow(tmp_ref);
		LOG4CXX_DEBUG(logger, (char*) "connected to %s" << destinationName);
		this->sendTo = strdup((char*) destinationName);
	}
}

MESSAGE HybridSessionImpl::receive(long time) {
	MESSAGE message = temporaryQueue->receive(time);
	if (message.replyto != NULL && strcmp(message.replyto, "") != 0) {
		setSendTo(message.replyto);
	} else {
		setSendTo(NULL);
	}
	return message;
}

bool HybridSessionImpl::send(MESSAGE message) {
	LOG4CXX_DEBUG(logger, "HybridSessionImpl::send");

	bool toReturn = false;
	if (serviceInvokation) {
		stomp_frame frame;
		frame.command = (char*) "SEND";
		frame.headers = apr_hash_make(pool);
		apr_hash_set(frame.headers, "destination", APR_HASH_KEY_STRING, sendTo);
		apr_hash_set(frame.headers, "receipt", APR_HASH_KEY_STRING, "send");

		frame.body_length = message.len;
		frame.body = message.data;
		if (message.replyto && strcmp(message.replyto, "") != 0) {
			LOG4CXX_TRACE(logger, "send set messagereplyto: "
					<< message.replyto);
			apr_hash_set(frame.headers, "messagereplyto", APR_HASH_KEY_STRING,
					message.replyto);
		} else {
			LOG4CXX_TRACE(logger, "send not set messagereplyto");
		}
		char * correlationId = apr_itoa(pool, message.correlationId);
		char * flags = apr_itoa(pool, message.flags);
		char * rval = apr_itoa(pool, message.rval);
		char * rcode = apr_itoa(pool, message.rcode);
		apr_hash_set(frame.headers, "messagecorrelationId",
				APR_HASH_KEY_STRING, correlationId);
		LOG4CXX_TRACE(logger, "Set the corrlationId: " << correlationId);
		apr_hash_set(frame.headers, "messageflags", APR_HASH_KEY_STRING, flags);
		apr_hash_set(frame.headers, "messagerval", APR_HASH_KEY_STRING, rval);
		apr_hash_set(frame.headers, "messagercode", APR_HASH_KEY_STRING, rcode);
		char* control = serialize_tx((char*) "ots");
		if (control) {
			LOG4CXX_TRACE(logger, "Sending serialized control: " << control);
			apr_hash_set(frame.headers, "messagecontrol", APR_HASH_KEY_STRING,
					control);
		}

		LOG4CXX_DEBUG(logger, "Send to: " << sendTo << " Command: "
				<< frame.command << " Size: " << frame.body_length);
		apr_status_t rc = stomp_write(stompConnection, &frame, pool);
		if (rc != APR_SUCCESS) {
			LOG4CXX_ERROR(logger, "Could not send frame");
			//setSpecific(TPE_KEY, TSS_TPESYSTEM);
		} else {
			LOG4CXX_TRACE(logger, "Sent frame");
			stomp_frame *framed;
			rc = stomp_read(stompConnection, &framed, pool);
			if (rc != APR_SUCCESS) {
				LOG4CXX_ERROR(logger, "Could not send frame");
				//setSpecific(TPE_KEY, TSS_TPESYSTEM);
			} else if (strcmp(framed->command, (const char*) "ERROR") == 0) {
				LOG4CXX_DEBUG(logger, (char*) "Got an error: " << framed->body);
				//setSpecific(TPE_KEY, TSS_TPENOENT);
			} else if (strcmp(framed->command, (const char*) "RECEIPT") == 0) {
				LOG4CXX_DEBUG(logger, (char*) "SEND RECEIPT: "
						<< (char*) apr_hash_get(framed->headers, "receipt-id",
								APR_HASH_KEY_STRING));
				toReturn = true;
			} else {
				LOG4CXX_ERROR(logger, "Didn't get a receipt: "
						<< framed->command << ", " << framed->body);
			}
			LOG4CXX_DEBUG(logger, "Sent to: " << sendTo << " Command: "
					<< frame.command << " Size: " << frame.body_length);

		}
		serviceInvokation = false;
	} else {
		char* data_togo = new char[message.len];
		LOG4CXX_TRACE(logger, (char*) "allocated");
		memcpy(data_togo, message.data, message.len);
		LOG4CXX_TRACE(logger, (char*) "copied: idata into: data_togo");

		LOG4CXX_DEBUG(logger, (char*) "Sending to remote queue: "
				<< remoteEndpoint);
		AtmiBroker::octetSeq_var aOctetSeq = new AtmiBroker::octetSeq(
				message.len, message.len, (unsigned char*) data_togo, true);
		remoteEndpoint->send(message.replyto, message.rval, message.rcode,
				aOctetSeq, message.len, message.correlationId, message.flags);
		aOctetSeq = NULL;
		LOG4CXX_DEBUG(logger, (char*) "Called back ");

		toReturn = true;
	}
	return toReturn;
}

const char* HybridSessionImpl::getReplyTo() {
	return replyTo;
}

int HybridSessionImpl::getId() {
	return id;
}
