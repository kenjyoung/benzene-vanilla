#include "networkEvaluator.h"
#include <iostream>

int main(){
	networkEvaluator eval;
	const int board_size = 13*13;
	bool state[2*board_size];
	double scores[board_size];
		for(int i=0; i<2*board_size; i++){
		state[i]=false;
	}
	//0=white 1=black
	for(int i=0; i<1000; i++){
			eval.evaluate(state, 0, scores);
			for(int i=0; i<board_size; i++){
				std::cout<<scores[i]<<" ";
			}
		std::cout<<std::endl;
	}
}