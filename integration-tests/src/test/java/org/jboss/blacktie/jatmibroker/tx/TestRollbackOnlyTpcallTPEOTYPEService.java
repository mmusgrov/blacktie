package org.jboss.blacktie.jatmibroker.tx;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.jboss.blacktie.jatmibroker.xatmi.BlacktieService;
import org.jboss.blacktie.jatmibroker.xatmi.Connection;
import org.jboss.blacktie.jatmibroker.xatmi.ConnectionException;
import org.jboss.blacktie.jatmibroker.xatmi.Response;
import org.jboss.blacktie.jatmibroker.xatmi.TPSVCINFO;
import org.jboss.blacktie.jatmibroker.xatmi.X_COMMON;

public class TestRollbackOnlyTpcallTPEOTYPEService implements BlacktieService {
	private static final Logger log = LogManager
			.getLogger(TestRollbackOnlyTpcallTPEOTYPEService.class);

	public Response tpservice(TPSVCINFO svcinfo) {
		try {
			log.info("test_tpcall_TPEOTYPE_service");
			int len = 60;
			X_COMMON toReturn = (X_COMMON) svcinfo.tpalloc("X_COMMON", "test");
			toReturn.setByteArray("key", "test_tpcall_TPEOTYPE_service"
					.getBytes());
			return new Response(Connection.TPSUCCESS, 0, toReturn, len, 0);
		} catch (ConnectionException e) {
			return new Response(Connection.TPFAIL, 0, null, 0, 0);
		}
	}
}
