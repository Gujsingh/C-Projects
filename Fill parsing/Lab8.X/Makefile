all: rpg

rpg: Player.c Game.c Lab08_main.c
	gcc -Wall Player.c Game.c Lab08_main.c -o rpg

Player.o: Player.c
	gcc -Wall -c Player.c

Lab08_main.o: Lab08_main.c
	gcc -Wall -c Lab08_main.c

Game.o: Game.c
	gcc -Wall -c Game.c

clean: 
	rm *.o rpg
