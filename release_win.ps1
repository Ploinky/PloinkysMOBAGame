$OutputFolder = './_dist/win'
$PAKR = './_build/apps/pakr/pakr.exe'

if (Test-Path -Path $OutputFolder) {
    "Output folder [$OutputFolder] exists"
    Remove-Item $OutputFolder/* -Recurse -Force
} else {
    "Creating output folder [$OutputFolder]"
    New-Item -ItemType Directory -Force -Path $OutputFolder
}

Copy-Item "./_out/client/client.exe" -Destination $OutputFolder
Copy-Item "./_out/client/steam_api64.dll" -Destination $OutputFolder
Copy-Item "./_out/client/steam_appid.txt" -Destination $OutputFolder
Copy-Item "./resources/data" -Destination $OutputFolder -Recurse
New-Item -ItemType Directory -Force -Path "$OutputFolder/cfg"
Copy-Item "./_out/client/cfg/settings.cfg" -Destination "$OutputFolder/cfg/"

New-Item -ItemType Directory -Force -Path "$OutputFolder/Persons"
& $PAKR "./resources/assets_raw/persons/chess_person" "$OutputFolder/Persons/ChessPerson.pak" "assets/persons/chess_person"
& $PAKR "./resources/assets_raw/persons/minion" "$OutputFolder/Persons/Minion.pak" "assets/persons/minion"
& $PAKR "./resources/assets_raw/persons/shared" "$OutputFolder/Persons/_Generic.pak" "assets/persons/shared"

New-Item -ItemType Directory -Force -Path "$OutputFolder/Buildings"
& $PAKR "./resources/assets_raw/buildings/tower" "$OutputFolder/Buildings/Tower.pak" "assets/Buildings/Tower"

New-Item -ItemType Directory -Force -Path "$OutputFolder/UI"
New-Item -ItemType Directory -Force -Path "$OutputFolder/UI/Buttons"
& $PAKR "./resources/assets_raw/ui/buttons/menu-button" "$OutputFolder/UI/Buttons/MenuButton.pak" "assets/ui/buttons/menu-button"
& $PAKR "./resources/assets_raw/ui/move-to" "$OutputFolder/UI/MoveTo.pak" "assets/UI/move-to"

New-Item -ItemType Directory -Force -Path "$OutputFolder/Maps"
& $PAKR "./resources/assets_raw/maps/map1" "$OutputFolder/Maps/Map1.pak" "assets/maps/map1"
& $PAKR "./resources/assets_raw/shared" "$OutputFolder/Generic.pak"

New-Item -ItemType Directory -Force -Path "$OutputFolder/characters"
& $PAKR "./resources/assets_raw/characters/stormcaller" "$OutputFolder/characters/stormcaller.pak" "assets/characters/stormcaller"

& $PAKR "./_out/client/shaders" "$OutputFolder/shaders.pak" "Shaders"

& $PAKR "./resources/data" "$OutputFolder/data.pak" "data"