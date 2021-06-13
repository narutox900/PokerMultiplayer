# PokerMultiplayer

Poker Multiplayer game made in C++ and C#, designed to run in Windows (but require Linux to run the server).

# Run the project

First, we need to run the server by running the file `server` in the folder `bin` in a Linux system (we may use WSL for this).

Then, we can run the game client in a Windows device by running `poker multiplayer game.exe` inside `GameBinFiles` folder. We need to know the IP of the host (the server) in order to connect to the server since the server IP is variable and not always localhost.

# Structure

`server` folder holds all the needed files for the server. Inside it lie the main server, the game server and other wrapper classes used inside.

The binary file needed is in the `bin` folder (created after using make file).

The `client` file here is only for debug purpose, not the real client for the game.

Source files for the client game are in `Client Socket DLL` and `poker multiplayer game` folders.


# Compile server

Use the following command in the root folder:
```
make
```

Require installing protobuf for C++ according to this link: https://github.com/protocolbuffers/protobuf/blob/master/src/README.md


# Compile client
To compile client files, we need Visual Studio and Unity installed.

We need to import the project in `Client Socket DLL` into Visual Studio and the project in `poker multiplayer game` into Unity. Then build both those projects.