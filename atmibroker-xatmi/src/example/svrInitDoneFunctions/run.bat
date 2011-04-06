@echo off

echo "Example: Running svrInitDoneFunctions"

cd %BLACKTIE_HOME%\examples\xatmi\svrInitDoneFunctions

rem RUN THE FOOAPP SERVER
call generate_server -Dserver.includes=BarService.c,SvrInitDone.c
IF %ERRORLEVEL% NEQ 0 exit -1
set BLACKTIE_CONFIGURATION=win32
call btadmin startup
IF %ERRORLEVEL% NEQ 0 exit -1
set BLACKTIE_CONFIGURATION=

call btadmin shutdown
IF %ERRORLEVEL% NEQ 0 exit -1