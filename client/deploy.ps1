New-Item -ItemType Directory -Force -Path Release

Copy-Item -Path x64/Release/Game.exe Release/
Copy-Item -Path x64/Release/steam_api64.dll Release/

Set-Location -Path x64/Debug

PakUtil Generic ../../Release/Generic.pak

New-Item -ItemType Directory -Force -Path ../../Release/Maps
PakUtil Maps/map1 ../../Release/Maps/Map1.pak

New-Item -ItemType Directory -Force -Path ../../Release/Buildings
PakUtil Buildings/Tower ../../Release/Buildings/Tower.pak

New-Item -ItemType Directory -Force -Path ../../Release/Persons
PakUtil Persons/ChessPerson ../../Release/Persons/ChessPerson.pak
PakUtil Persons/Minion ../../Release/Persons/Minion.pak
PakUtil Persons/_Generic ../../Release/Persons/_Generic.pak

New-Item -ItemType Directory -Force -Path ../../Release/UI/Buttons
PakUtil UI/Buttons/MenuButton ../../Release/UI/Buttons/MenuButton.pak

PakUtil UI/MoveTo ../../Release/UI/MoveTo.pak

Set-Location -Path ../../x64/Release

PakUtil Shaders ../../Release/Shaders.pak

Set-Location -Path ../..