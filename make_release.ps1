Remove-Item -Recurse -Force Release\
New-Item Release\x64\

<# Launcher #>
xcopy /s Launcher\Build\Release\x64\Launcher.exe Release\x64\Launcher\Launcher.exe*

<# Client #>
xcopy /s Client\Build\Release\x64\Client.exe Release\x64\Client\Client.exe*
xcopy /s data\client\* Release\x64\Client\
xcopy /s data\shared\* Release\x64\Client\
xcopy /s pmg_version Release\x64\Client\

<# Server #>
xcopy /s Server\Build\Release\x64\Server.exe Release\x64\Server\Server.exe*
xcopy /s data\server\* Release\x64\Server\
xcopy /s data\shared\* Release\x64\Server\
xcopy /s pmg_version Release\x64\Client\

<# Zip everything up #>
Compress-Archive Release\x64\Launcher\* Release\x64\PloinkysMOBAGameLauncher.zip
Compress-Archive Release\x64\Client\* Release\x64\PloinkysMOBAGame.zip
Compress-Archive Release\x64\Server\* Release\x64\PloinkysMOBAGameServer.zip