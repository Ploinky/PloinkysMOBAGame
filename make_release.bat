rmdir /s /q Release
mkdir Release
mkdir Release/x64/Client

:: Client
xcopy /s Launcher\Build\Release\x64\Launcher.exe Release\x64\Client\Launcher.exe*
xcopy /s Client\Build\Release\x64\Client.exe Release\x64\Client\Client.exe*
xcopy /s data\client\* Release\x64\Client\
xcopy /s data\shared\* Release\x64\Client\
xcopy /s pmg_version Release\x64\Client\

:: Server
xcopy /s Server\Build\Release\x64\Server.exe Release\x64\Server\Server.exe*
xcopy /s data\server\* Release\x64\Server\
xcopy /s data\shared\* Release\x64\Server\
xcopy /s pmg_version Release\x64\Client\

:: Zip everything up
Compress-Archive Release\x64\Client\* PloinkysMOBAGame.zip