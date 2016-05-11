#include <Python.h>
class networkEvaluator
{
public:
	PyObject* evaluator;
	networkEvaluator();
	void evaluate( bool* state, int toPlay, double* scores);
};

