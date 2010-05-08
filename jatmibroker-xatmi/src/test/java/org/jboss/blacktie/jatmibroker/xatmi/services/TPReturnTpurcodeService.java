package org.jboss.blacktie.jatmibroker.xatmi.services;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.jboss.blacktie.jatmibroker.xatmi.Buffer;
import org.jboss.blacktie.jatmibroker.xatmi.Connection;
import org.jboss.blacktie.jatmibroker.xatmi.ConnectionException;
import org.jboss.blacktie.jatmibroker.xatmi.Response;
import org.jboss.blacktie.jatmibroker.xatmi.Service;
import org.jboss.blacktie.jatmibroker.xatmi.TPSVCINFO;
import org.jboss.blacktie.jatmibroker.xatmi.TestTPConversation;
import org.jboss.blacktie.jatmibroker.xatmi.X_OCTET;

public class TPReturnTpurcodeService implements Service {
	private static final Logger log = LogManager
			.getLogger(TPReturnTpurcodeService.class);

	public Response tpservice(TPSVCINFO svcinfo) {
		log.info("testtpreturn_service_tpurcode");
		int len = 1;
		try {
			Buffer toReturn = (X_OCTET) svcinfo.getConnection().tpalloc(
					"X_OCTET", null);
			if (TestTPConversation.strcmp(svcinfo.getBuffer(), "24") == 0) {
				return new Response(Connection.TPSUCCESS, 24, toReturn, len, 0);
			} else {
				return new Response(Connection.TPSUCCESS, 77, toReturn, len, 0);
			}
		} catch (ConnectionException e) {
			return new Response(Connection.TPFAIL, 0, null, 0, 0);
		}
	}
}
