import argparse
import random
from program import Program
import sys

def print_game(game):
	for move in game:
		print(move, end=' ')
	print()

PLAYERS = {0: "white", 1: "black"}


p = argparse.ArgumentParser()
p.add_argument('games', metavar = 'games', type = int, help = 'How many games should be generated.')
p.add_argument('boardsize', metavar = 'boardsize', type = int, help = 'Size of the game board to play on.')
p.add_argument('temperature', metavar = 'temperature', type = int, help = 'Amount of randomness in move selection.')
args = p.parse_args()

wolve = Program("../src/wolve/./wolve 2>/dev/null", False)

wolve.sendCommand("param_wolve temperature "+str(args.temperature))
wolve.sendCommand("boardsize "+str(args.boardsize))

for i in range(args.games):
	wolve.sendCommand("clear_board")
	game = []
	while(True):
		move = wolve.sendCommand("genmove black").strip()
		if(str(move) != "resign"):
			game.append(move)
		else:
			print_game(game)
			sys.stdout.flush()
			break
		move = wolve.sendCommand("genmove white").strip()
		if(str(move) != "resign"):
			game.append(move)
		else:
			print_game(game)
			sys.stdout.flush()
			break



	
