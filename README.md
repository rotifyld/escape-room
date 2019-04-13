# escape-room

Coordinating multiple processes using shared memory and semaphores. 


## Full assignment:

Bajtek celebrates his 16th birthday. Along with his friends he booked building containing several ```escape rooms```. Before they start playing, the ```manager``` - knowing total number of players N (2 <= N <= 1024) - prepares M (1 <= M <= 1024) ```escape rooms``` for Bajtek and his friends. Each ```escape room``` has its capacity and type ```T``` specified. The capacity is the maximum number of players and type T (letter A-Z) describes type of riddles that players have to solve in that room.

Each of players (including Bajtek) has unique identifier ```[1..N]``` and favorite room type, which remains constant. In order to avoid chaos, Bajtek and his friends use list where they put game propositions. Each of propositions contains:
 - required room type
 - information about person who put this proposition
 - information who have to participate in game together with proposition’s author - we can specify directly other players’ identifiers and/or request any player preferring room with type T (which may differ from required room type).

When proposed game finishes, proposition is removed from the list. Proposed game can be started when all following conditions are met:
 - there is a free room with requested type and enough capacity (proposition’s author and all invited players can enter it)
 - proposition’s author currently is not playing any game
 - players specified in proposition directly (by providing their identifiers) currently are not playing any game

for each requested player preferring type T, we can choose matching player (obviously different than proposition’s author and any of players specified by providing identifier) who is currently not playing any game.

At the beginning players enter the building. After everyone comes, every player put his/her game proposition (if has any) at the end of the list.

Next, player checks if there is any proposition that can be played. If there is such one, he/she informs participating players about room and other participating players’ identifiers. If there is more than one proposition that can be started, we select first one (in order they have been added to list). When we choose players by their type, we may dismiss possible impact on other propositions. However, we should ensure that available rooms are used effectively (by choosing the smallest of rooms fitting proposition’s requirements).

Game starts after all participating players enter the room. Players may leave the room individually and after that participate in another game. Game ends when the last player leave the room - then, the room may be used for next game and player who proposed this game can put another game proposition on the list (if he/she currently participates in other game, he/she is able to add proposition after leaving the other room).

If there are no more games in which the player could participate (that is, all players have no more propositions and there is no proposition that could require the player), the player leaves building and notifies manager about his/her identifier and number of games played.

manager waits until all players leave building. After the last participant exits, he closes the building (releasing all resources) and finishes work.

## Task

Write implementation of processes ```manager``` and ```player``` in C, using shared memory and semaphores for passing data and synchronization. You have to avoid deadlock and starvation (unless such situation is implied by assignment description). All allocated resources have to be released after processes exit. The lack of releasing resources will be punished with penalty points.

Process ```manager``` in first line of its standard input gets numbers ```N``` and ```M```. Next ```M``` lines describe available rooms: room type and capacity. After reading input and performing initialization, ```manager``` runs ```N``` processes ```player```, passing player’s identifier as the first command argument. ```manager``` writes on standard output information about leaving players. Example of ```manager```’s standard input:
```
10 4
A 3
B 6
C 8
A 2
```
Example of standard output message - player leaves building:
```
Player 10 left after 5 game(s).
```
```player``` is run with one argument specifying its unique identifier. Input data for ```k```th player is stored in file ```player-k.in```. All messages of ```k```-th player have to be written to file ```player-k.out```. First line of input contains identifier of favorite room type. Next lines describe game propositions. Each line contains (separated by a space): required room type and identifiers and/or types of required players. You can assume that input format is correct, however resulting proposition may be not playable. In such case ```player``` has to write an error message - for example if ```manager``` with input specified above runs ```player``` 1 with input:

```player-1.in```:
```
A
Z 2
```
We should write
```
Invalid game "Z 2"
```
In this case proposition is invalid, because there is no room with type ```Z```.
```
Moreover, process player writes messages:
```
when proposition can be played and participating players get notified:
```
Game defined by 10 is going to start: room 10, players (2, 3, 4, 10)
```
about entering room, along with information about author of game proposition and missing participants
```
Entered room 10, game defined by 10, waiting for players (2, 3, 4)
```
indicating that ```player``` left room

Left room 10
