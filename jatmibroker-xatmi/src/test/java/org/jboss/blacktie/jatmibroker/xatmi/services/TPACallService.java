package org.jboss.blacktie.jatmibroker.xatmi.services;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.jboss.blacktie.jatmibroker.xatmi.Connection;
import org.jboss.blacktie.jatmibroker.xatmi.ConnectionException;
import org.jboss.blacktie.jatmibroker.xatmi.Response;
import org.jboss.blacktie.jatmibroker.xatmi.Service;
import org.jboss.blacktie.jatmibroker.xatmi.TPSVCINFO;
import org.jboss.blacktie.jatmibroker.xatmi.X_OCTET;

public class TPACallService implements Service {
	private static final Logger log = LogManager
			.getLogger(TPACallService.class);

	public Response tpservice(TPSVCINFO svcinfo) throws ConnectionException {
		log.info("testtpacall_service");
		int len = 20;
		X_OCTET toReturn = (X_OCTET) svcinfo.getConnection().tpalloc("X_OCTET",
				null, len);
		toReturn.setByteArray("testtpacall_service".getBytes());
		return new Response(Connection.TPSUCCESS, 0, toReturn, 0);
	}
}
