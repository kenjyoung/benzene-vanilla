import os
from copy import copy, deepcopy
import numpy as np
from inputFormat import *
import theano
from theano import tensor as T
import cPickle

def formatInput(state):
	board = np.reshape(state, (2,13,13))
	ret = new_game(13)
	padding = 2
	for i in range(13):
		for j in range(13):
			if board[white,i,j]:
				play_cell(ret, (i+padding,j+padding), white)
			elif board[black,i,j]:
				play_cell(ret, (i+padding,j+padding), black)
	return ret

class networkEvaluator:
	def __init__(self):
		f = file(os.path.dirname(os.path.realpath(__file__))+"/network.save", 'rb')
		network = cPickle.load(f)
		batch_size = network.batch_size
		f.close()
		input_state = T.tensor3('input_state')
		self.evaluator = theano.function(
			[input_state],
			network.output[0],
			givens={
		        network.input: input_state.dimshuffle('x', 0, 1, 2),
			}
		)

	def evaluate(self, arrayIn, to_play):
		"""
		Compute resistance for all moves in current state.
		"""
		#get equivalent white to play game if black to play
		state = formatInput(arrayIn)
		if(to_play == black):
			state = mirror_game(state)
		played = np.logical_or(state[white,padding:boardsize+padding,padding:boardsize+padding],\
		state[black,padding:boardsize+padding,padding:boardsize+padding]).flatten()
		scores = self.evaluator(state)
		if(to_play == black):
			return np.transpose(np.reshape(scores,(13,13))).flatten()
		else:
			return scores
