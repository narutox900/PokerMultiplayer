# PokerMultiplayer
Poker Multiplayer game made in C++ and C#

# Compile file
```
make
```

Need to install protobuf for C++ according to this link: https://github.com/protocolbuffers/protobuf/blob/master/src/README.md

# Structure

`server` folder holds all the needed files for the server. Inside it lie the main server, the game server and other wrapper classes used inside

The binary file needed is in the bin folder (created after using make file)
The `client` file here is only for debug purpose, not the real client for the game