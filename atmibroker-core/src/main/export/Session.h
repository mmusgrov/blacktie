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
#ifndef Session_H_
#define Session_H_

#include "Message.h"

class Session {
public:
	virtual ~Session() {
	}
	virtual void setSendTo(const char* replyTo) = 0;
	virtual const char* getReplyTo() = 0;

	virtual MESSAGE receive(long time) = 0;
	virtual bool send(MESSAGE message) = 0;

	virtual int getId() = 0;

	/**
	 * Can this session send
	 */
	void setCanSend(bool canSend) {
		this->canSend = canSend;
	}

	/**
	 * Can this session receive
	 */
	void setCanRecv(bool canRecv) {
		this->canRecv = canRecv;
	}

	/**
	 * Can this session send
	 */
	bool getCanSend() {
		return canSend;
	}

	/**
	 * Can this session receive
	 */
	bool getCanRecv() {
		return canRecv;
	}

	/**
	 * Set the last rcode
	 */
	void setLastEvent(long event) {
		this->lastEvent = event;
	}

	/**
	 * Get the last rcode
	 */
	long getLastEvent() {
		return lastEvent;
	}

	/**
	 * Set the last rcode
	 */
	void setLastRCode(long rcode) {
		this->lastRCode = rcode;
	}

	/**
	 * Get the last rcode
	 */
	long getLastRCode() {
		return lastRCode;
	}
protected:
	bool canSend;
	bool canRecv;
	long lastEvent;
	long lastRCode;
};

#endif
