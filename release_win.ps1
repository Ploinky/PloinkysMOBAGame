$OutputFolder = './_dist/win64'
$ClientBuildFolder = './_out/win64/release/client'
$PAKR = './_build/win64/release/apps/pakr/pakr.exe'
$VERSION = Get-Content "./_out/win64/release/version.txt"
$STEAMCMD = "C:/steam-sdk/tools/ContentBuilder/builder/steamcmd.exe"

if ($VERSION.Contains("*")) {
    "DIRTY BUILD"
    Exit 1
}

if (-Not (Test-Path -Path $ClientBuildFolder)) {
    "Client build in folder [$ClientBuildFolder] does not exist"
    Exit 1
}

if (Test-Path -Path $OutputFolder) {
    "Output folder [$OutputFolder] exists"
    Remove-Item $OutputFolder/* -Recurse -Force
} else {
    "Creating output folder [$OutputFolder]"
    New-Item -ItemType Directory -Force -Path $OutputFolder
}

Copy-Item "./_out/win64/release/version.txt" -Destination $OutputFolder
Copy-Item "$ClientBuildFolder/client.exe" -Destination $OutputFolder
Copy-Item "$ClientBuildFolder/steam_api64.dll" -Destination $OutputFolder
Copy-Item "$ClientBuildFolder/steam_appid.txt" -Destination $OutputFolder
Copy-Item "./resources/data" -Destination $OutputFolder -Recurse
New-Item -ItemType Directory -Force -Path "$OutputFolder/cfg"
Copy-Item "$ClientBuildFolder/cfg/settings.cfg" -Destination "$OutputFolder/cfg/"

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

& $PAKR "$ClientBuildFolder/shaders" "$OutputFolder/shaders.pak" "Shaders"

& $PAKR "./resources/data" "$OutputFolder/data.pak" "data"

& $STEAMCMD +login curuba2311 +run_app_build -desc "$Version" (Resolve-Path ./tools/steam/app_build_1756910.vdf) +quit