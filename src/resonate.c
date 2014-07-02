#include <Python.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEBUG(x, ...) sprintf(_debug_string, x, __VA_ARGS__); \
    PyObject* sysmod = PyImport_ImportModuleNoBlock("sys"); \
    PyObject* pystdout = PyObject_GetAttrString(sysmod, "stdout"); \
    PyObject* result = PyObject_CallMethod(pystdout, "write", "s", _debug_string); \
    Py_XDECREF(result); \
    Py_XDECREF(pystdout); \
    Py_XDECREF(sysmod);    
char _debug_string[1000];

static PyObject* resonate(PyObject *self, PyObject *args, PyObject *keywds)
{
    static char *kwlist[] = {"signal", "sr", "freqs", "damping",
                             "rms_window", "return_response", NULL};
    PyObject *pysignal;
    int sr;
    PyObject *pyfreqs;
    double damping;
    int rms_window;
    int return_response;
    int siglen;
    int nfreqs;
    double w;
    double *x, *x1, *x2;
    double *a1, *a2, *b1;
    PyObject *signal;
    PyObject **response;
    PyObject *response_list;
    double *rms_moving;
    PyObject **rms;
    PyObject *rms_list;
    PyObject *out;
    double rms_val;
    PyObject *item;
    double freq;
    long freq_long;
    double sample;
    int i, t;

    if(!PyArg_ParseTupleAndKeywords(args, keywds, "OiOdi|i", kwlist,
                                    &pysignal, &sr, &pyfreqs, &damping,
                                    &rms_window, &return_response)) {
        return NULL;
    }

    if(!PySequence_Check(pyfreqs)) {
        PyErr_SetString(PyExc_TypeError, "freqs: expected sequence of numbers");
        return NULL;
    }

    nfreqs = PySequence_Size(pyfreqs);
    signal = PySequence_Fast(pysignal, "signal: expected sequence of floats");
    siglen = PySequence_Size(signal);

    x = alloca(nfreqs * sizeof(double));
    x1 = alloca(nfreqs * sizeof(double));
    x2 = alloca(nfreqs * sizeof(double));
    a1 = alloca(nfreqs * sizeof(double));
    a2 = alloca(nfreqs * sizeof(double));
    b1 = alloca(nfreqs * sizeof(double));
    rms_moving = alloca(nfreqs * sizeof(double));
    rms = alloca(nfreqs * sizeof(PyObject *));

    if(return_response)
        response = alloca(nfreqs * sizeof(PyObject *));

    for(i = 0; i < nfreqs; i ++) {
        item = PySequence_GetItem(pyfreqs, i);
        if(PyInt_Check(item)) {
            freq_long = PyInt_AsLong(item);
            freq = (double)freq_long;
        }
        else if(PyLong_Check(item)) {
            freq = PyLong_AsDouble(item);
        }
        else if(PyFloat_Check(item)) {
            freq = PyFloat_AsDouble(item);
        }
        else {
            Py_DECREF(item);
            PyErr_SetString(PyExc_TypeError, "freqs: expected sequence of numbers");
            return NULL;
        }

        w = (2 * M_PI * freq) / sr;

        a1[i] = (pow(w, 2) - 2) / (1 + damping);
        a2[i] = (1 - damping) / (1 + damping);
        b1[i] = 1 / (1 + damping);

        x[i] = 0;
        x1[i] = 0;
        x2[i] = 0;

        rms_moving[i] = 0;
        rms[i] = PyList_New(siglen / rms_window);

        if(return_response)
            response[i] = PyList_New(siglen);
    }

    for(t = 0; t < siglen; t ++) {
        item = PySequence_Fast_GET_ITEM(signal, t);
        sample = PyFloat_AsDouble(item);

        for(i = 0; i < nfreqs; i ++) {
            x[i] = b1[i] * sample - a1[i] * x1[i] - a2[i] * x2[i];
            rms_moving[i] += pow(x[i], 2);

            if(return_response) {
                item = PyFloat_FromDouble(x[i]);
                PyList_SET_ITEM(response[i], t, item);
            }
        }

        memcpy(x2, x1, nfreqs * sizeof(double));
        memcpy(x1, x, nfreqs * sizeof(double));

        if((t + 1) % rms_window == 0) {
            for(i = 0; i < nfreqs; i ++) {
                rms_val = sqrt(rms_moving[i] / (double)rms_window);
                rms_moving[i] = 0;
                item = PyFloat_FromDouble(rms_val);
                PyList_SET_ITEM(rms[i], t / rms_window, item);
            }
        }
    }
    Py_DECREF(signal);

    rms_list = PyList_New(nfreqs);
    if(return_response)
        response_list = PyList_New(nfreqs);

    for(i = 0; i < nfreqs; i ++) {
        PyList_SET_ITEM(rms_list, i, rms[i]);
        if(return_response)
            PyList_SET_ITEM(response_list, i, response[i]);
    }

    if(return_response)
        return Py_BuildValue("(O,O)", rms_list, response_list);
    else
        return Py_BuildValue("O", rms_list);
}

static PyMethodDef functions[] = {
    {"resonate", (PyCFunction)resonate, METH_VARARGS | METH_KEYWORDS,
     "Resonate a signal."},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_resonate(void)
{
    Py_InitModule("_resonate", functions);
}
