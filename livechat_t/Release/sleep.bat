:: for bat sleep

::echo %time%

:: sleep 1 second
::ping 127.0.0.1 -n 2 > null
::timeout /T 1 /NOBREAK
::choice /t 1 /d y /n >nul 

start /wait sleep.vbs

::echo %time%

exit