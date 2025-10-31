Write-Output "Fetching dependencies..."

Write-Output "Downloading and unpacking GLFW 3.4"
curl -L https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip -o test.zip
New-Item -Force -Path '.' -Name 'temp_glfw' -ItemType "Directory" | Out-Null
tar -xzf test.zip -C temp_glfw/
Copy-Item -R -Force temp_glfw/glfw-3.4.bin.WIN64/include/* vendor/include/
Copy-Item -R -Force temp_glfw/glfw-3.4.bin.WIN64/lib-vc2022/* vendor/lib/
Remove-Item -Force -Recurse temp_glfw
Remove-Item -Force test.zip

