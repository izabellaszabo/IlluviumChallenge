# IlluviumChallenge By Izabella Szabo

The game can be played in editor in both Standalone Netmode and Play as Client netmode.

You can set how many creatures to spawn in the GameMode blueprint variable: NumberOfCreaturesToSpawn. 
Set to 10 by default

Note: I realise that in PlayAsClient mode the server and client results do not match up perfectly. 
They are deterministic in their own netmode, but the server desynches from the client.
The start locations do match up, but I realised that the order of the actors on the level is different, 
I assume they get their TimeSteps in different orders and so they desynch from server as time goes on.
This would require some more debugging to find the exact issue, I've run out of time for now :) 
