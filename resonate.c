/* 
 * Signal resonator Python module
 * Copyright (C) 2012   <andreas@jansson.me.uk>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <Python.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

static PyObject *resonate(PyObject *self, PyObject *args, PyObject *keywds)
{
  static char *kwlist[] = {"signal", "sr", "freqs", "damping",
                           "rms_window", "return_resons", NULL};

  PyObject *pysignal;
  int sr;
  PyObject *pyfreqs;
  double damping;
  int rms_window;
  int return_resons;
  PyObject *item;
  double *freqs;
  int nfreqs;
  int siglen;
  PyObject *signal;
  PyObject **resons;
  PyObject *resons_list;
  double twopisquared;
  double dt;
  double sample;
  double *pos, *prev_pos, *vel, *prev_vel;
  double normpos;
  double *norm;
  double *freq2_sr;
  int i, t;
  double max_rms;
  double *rms_moving;
  PyObject **rms;
  PyObject *rms_list;
  double rms_val;

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "OiOdi|i", kwlist,
                                  &pysignal, &sr, &pyfreqs, &damping,
                                  &rms_window, &return_resons)) {
    return NULL;
  }

  if(!PySequence_Check(pyfreqs)) {
    PyErr_SetString(PyExc_TypeError, "freqs should be a sequence of floats");
    return NULL;
  }

  nfreqs = PySequence_Size(pyfreqs);
  freqs = alloca(nfreqs * sizeof(double));

  twopisquared = pow(2.0 * M_PI, 2.0);
  freq2_sr = alloca(nfreqs * sizeof(double));

  if(return_resons)
    resons = alloca(nfreqs * sizeof(PyObject *));

  signal = PySequence_Fast(pysignal, "expected sequence");
  siglen = PySequence_Size(signal);

  pos = alloca(nfreqs * sizeof(double));
  prev_pos = alloca(nfreqs * sizeof(double));
  vel = alloca(nfreqs * sizeof(double));
  prev_vel = alloca(nfreqs * sizeof(double));
  norm = alloca(nfreqs * sizeof(double));

  rms_moving = alloca(nfreqs * sizeof(double));
  rms = alloca(nfreqs * sizeof(double));

  for(i = 0; i < nfreqs; i ++) {
    item = PySequence_GetItem(pyfreqs, i);
    if(!PyFloat_Check(item)) {
      Py_DECREF(item);
      PyErr_SetString(PyExc_TypeError, "expected sequence of floats");
      return NULL;
    }
    freqs[i] = PyFloat_AsDouble(item);
    Py_DECREF(item);

    if(return_resons)
      resons[i] = PyList_New(siglen);

    freq2_sr[i] = pow(freqs[i], 2) / (double)sr;

    pos[i] = 0;
    vel[i] = 0;
    norm[i] = pow(freqs[i], 3);

    rms_moving[i] = 0;
    rms[i] = PyList_New(siglen / rms_window);
  }

  // algorithm loosely based on https://instruct1.cit.cornell.edu/courses/bionb441/FinalProjects/f2006/knr9/Final%20Project/KevinRohmannBioNB441FinalProject.htm

  max_rms = 0;
  dt = 1.0 / (double)sr;

  for(t = 0; t < siglen; t ++) {
    item = PySequence_Fast_GET_ITEM(signal, t);
    sample = PyFloat_AsDouble(item);
    
    for(i = 0; i < nfreqs; i ++) {
      prev_pos[i] = pos[i];
      prev_vel[i] = vel[i];
      pos[i] = prev_pos[i] + dt * prev_vel[i];
      normpos = pos[i] * norm[i];
      vel[i] = sample + prev_vel[i] - freq2_sr[i] * (twopisquared * prev_pos[i] + damping * prev_vel[i]);

      if(return_resons) {
        item = PyFloat_FromDouble(normpos);
        PyList_SET_ITEM(resons[i], t, item);
      }

      rms_moving[i] += pow(normpos, 2.0);
    }

    if((t + 1) % rms_window == 0) {
      for(i = 0; i < nfreqs; i ++) {
        rms_val = sqrt(rms_moving[i] / (double)rms_window);
        rms_moving[i] = 0;
        item = PyFloat_FromDouble(rms_val);
        PyList_SET_ITEM(rms[i], t / rms_window, item);
        if(rms_val > max_rms)
          max_rms = rms_val;
      }
    }
  }
  Py_DECREF(signal);

  if(return_resons)
    resons_list = PyList_New(nfreqs);
  rms_list = PyList_New(nfreqs);
  for(i = 0; i < nfreqs; i ++) {
    if(return_resons)
      PyList_SET_ITEM(resons_list, i, resons[i]);
    PyList_SET_ITEM(rms_list, i, rms[i]);
  }

  if(return_resons)
    return Py_BuildValue("(O,O,d)", resons_list, rms_list, max_rms);
  return Py_BuildValue("(O,d)", rms_list, max_rms);
}

static PyMethodDef functions[] = {
    {"resonate", (PyCFunction)resonate, METH_VARARGS | METH_KEYWORDS,
     "Resonate a signal."},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initresonate(void)
{
    Py_InitModule(
        "resonate", functions
        );
}
