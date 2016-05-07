#include <Python.h>
#include "numpy/arrayobject.h"
#include <stdlib.h>
#include <iostream>
#include "networkEvaluator.h"

networkEvaluator::networkEvaluator(){
	setenv("PYTHONPATH",".",1);
	Py_Initialize();
	import_array();
	PyObject *evalModule = PyImport_Import(PyString_FromString((char*) "networkEvaluator"));
	PyObject *evalType = PyObject_GetAttrString(evalModule, "networkEvaluator");
	this->evaluator = PyObject_CallObject(evalType, NULL);
}

//takes a flattened representation of a boardstate in a boolean C array consisting
//of an array of white stones, followed by black stones
//outputs the scores from a neural network evaluation over the whole board
void networkEvaluator::evaluate(bool* state, int toPlay, double* scores){
	const int n_state_dims = 3;
	npy_intp state_dims[3] = {2,13,13};
	PyObject *npState = PyArray_SimpleNewFromData(
		n_state_dims, 
		state_dims, 
		NPY_BOOL, 
		reinterpret_cast<void*>(state)
	);
	PyObject *npToPlay = PyInt_FromLong(toPlay);
	PyObject *result = PyObject_CallMethodObjArgs(
		this->evaluator, 
		PyUnicode_FromString("evaluate"), 
		npState,
		npToPlay,
		NULL
	);
	double *outputScores = reinterpret_cast<double*>(PyArray_DATA(result));
	for(int i=0; i<13*13; i++){
		scores[i] = outputScores[i];
	}
}