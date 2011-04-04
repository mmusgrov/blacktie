# ALLOW JOBS TO BE BACKGROUNDED
set -m

# RUN THE INTEGRATION 1 EXAMPLE
echo "Example: Running integration 1 XATMI"
cd $BLACKTIE_HOME/examples/integration1/xatmi_service/
generate_server -Dservice.names=CREDIT,DEBIT -Dserver.includes="CreditService.c,DebitService.c"
if [ "$?" != "0" ]; then
        exit -1
fi
btadmin startup
if [ "$?" != "0" ]; then
        exit -1
fi
cd $BLACKTIE_HOME/examples/integration1/client/
generate_client -Dclient.includes=client.c 
./client 
if [ "$?" != "0" ]; then
	exit -1
fi
cd $BLACKTIE_HOME/examples/integration1/xatmi_service/
btadmin shutdown
if [ "$?" != "0" ]; then
	exit -1
fi

echo "Build Converted XATMI service"
cd $BLACKTIE_HOME/examples/integration1/ejb
mvn install
if [ "$?" != "0" ]; then
	exit -1
fi
cd $BLACKTIE_HOME/examples/integration1/ejb/ear/
mvn install
if [ "$?" != "0" ]; then
	exit -1
fi
cd $BLACKTIE_HOME/examples/integration1/xatmi_adapter/
mvn install
if [ "$?" != "0" ]; then
	exit -1
fi
cd $BLACKTIE_HOME/examples/integration1/xatmi_adapter/ear/
mvn install
if [ "$?" != "0" ]; then
	exit -1
fi
cd $BLACKTIE_HOME/examples/integration1/client/
generate_client -Dclient.includes=client.c
if [ "$?" != "0" ]; then
	exit -1
fi

echo "Run Converted XATMI service"
cd $BLACKTIE_HOME/examples/integration1/ejb/ear/
mvn jboss:deploy
if [ "$?" != "0" ]; then
	exit -1
fi
cd $BLACKTIE_HOME/examples/integration1/xatmi_adapter/ear/
mvn jboss:deploy
if [ "$?" != "0" ]; then
	exit -1
fi
cd $BLACKTIE_HOME/examples/integration1/client/
sleep 5
./client 
if [ "$?" != "0" ]; then
	exit -1
fi
cd $BLACKTIE_HOME/examples/integration1/xatmi_adapter/ear/
mvn jboss:undeploy
if [ "$?" != "0" ]; then
	exit -1
fi
cd $BLACKTIE_HOME/examples/integration1/ejb/ear/
mvn jboss:undeploy
if [ "$?" != "0" ]; then
	exit -1
fi