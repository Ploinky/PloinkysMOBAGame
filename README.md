# PloinkysMOBAGame

Contains PloinkysMOBAGame client, server and shared code as well as all data needed to run the game and its server. Also includes the launcher and game services.

## Client

Direct3D game engine written in C++.

## Server

Authoritative game server written in c++.

## Shared

Game code shared between server and client, such as physics and networking code, written in c++.

## Launcher

Game launcher and auto updater. Can update itself. Fetches newest game data and binaries whenever an update comes out. Written in C# using WPF.

## GameServices

REST API meant to provide user logins, stats, friendslist and potentially a ~~master~~ main gameserver. Written in C# using ASP.NET.
