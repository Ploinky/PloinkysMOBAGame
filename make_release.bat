rmdir /s /q Release
mkdir Release
mkdir Release/x64/Client

:: web-client
xcopy /s web-client\build Release\x64\Client\dist\

:: Launcher
xcopy Launcher\Build\Release\x64\Launcher.exe Release\x64\Client\Launcher.exe*
xcopy Launcher\Build\Release\x64\Launcher.tlb Release\x64\Client\Launcher.tlb*
xcopy Launcher\Build\Release\x64\WebView2Loader.dll Release\x64\Client\WebView2Loader.dll*

:: Client
xcopy Client\Build\Release\x64\Client.exe Release\x64\Client\Client.exe*