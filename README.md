# PloinkysMOBAGame

Multiplayer Online Battle Arena game. Does some of the things that games like League of Legends, DotA or HOTS do, except much worse.

This project contains the code for the client application, the game server, the code that is shared between the two, and all assets as well as tools. Basically you should be able to add some third party dependencies (enet, GLFW), compile and run!

![Screenshot of PloinkysMOBAGame gameplay](/img/screen.png)

## Client

Game client written in C++ using Direct3D 11, Direct2D, DirectWrite, XAudio2 and enet.

## Server

Game server written in C++ using enet.

## Common

Code shared between client and server, for example some physics/maths and networking code.

## Tools

Scripts used in the build process by some or all of the projecs.

## Vendor

Third party libraries and code go here.

## Assets

3D models, textures, sounds ...


# Mathematical conventions

- right handed coordinate system
- x right, y up, z backward
- column-vectors
- row-major
- NDCx,y in [-1, 1] NDCz in [0, 1]
- counter-clockwise (right-hand-rules) rotation
- camera viewspace forward is z-