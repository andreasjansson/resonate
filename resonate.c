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
  static char *kwlist[] = {"signal", "sr", "freqs", "spring_constant", "damping", "rms_window", NULL};

  PyObject *pysignal;
  int sr;
  PyObject *pyfreqs;
  double spring_constant;
  double damping;
  int rms_window;
  PyObject *item;
  double *freqs;
  int nfreqs;
  int siglen;
  PyObject *signal;
  double *masses;
  PyObject **resons;
  PyObject *resons_list;
  double twopisquared;
  double dt;
  double sample;
  double *pos, *prev_pos, *vel, *prev_vel;
  double normpos;
  double *norm;
  double *sm, *dm;
  int i, t;
  double max_rms;
  double *rms_moving;
  PyObject **rms;
  PyObject *rms_list;
  double rms_val;

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "OiOddi", kwlist,
                                  &pysignal, &sr, &pyfreqs, &spring_constant, &damping, &rms_window)) {
    return NULL;
  }

  if(!PySequence_Check(pyfreqs)) {
    PyErr_SetString(PyExc_TypeError, "expected sequence");
    return NULL;
  }

  nfreqs = PySequence_Size(pyfreqs);
  freqs = alloca(nfreqs * sizeof(double));

  twopisquared = pow(2.0 * M_PI, 2.0);
  spring_constant = spring_constant * twopisquared;
  masses = alloca(nfreqs * sizeof(double));

  resons = alloca(nfreqs * sizeof(PyObject *));

  signal = PySequence_Fast(pysignal, "expected sequence");
  siglen = PySequence_Size(signal);

  // optimisation
  sm = alloca(nfreqs * sizeof(double));
  dm = alloca(nfreqs * sizeof(double));

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

    masses[i] = spring_constant / (pow(freqs[i], 2.0) * twopisquared);

    resons[i] = PyList_New(siglen);

    sm[i] = -(spring_constant / masses[i]);
    dm[i] = (damping / masses[i]);

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
      pos[i] = (prev_pos[i] + dt * prev_vel[i]);;
      normpos = pos[i] * norm[i];
      vel[i] = sample + prev_vel[i] + dt * (sm[i] * prev_pos[i] - dm[i] * prev_vel[i]);
      item = PyFloat_FromDouble(normpos);
      PyList_SET_ITEM(resons[i], t, item);

      rms_moving[i] += fabs(normpos);
    }

    if((t + 1) % rms_window == 0) {
      for(i = 0; i < nfreqs; i ++) {
        rms_val = rms_moving[i] / (double)rms_window;
        rms_moving[i] = 0;
        item = PyFloat_FromDouble(rms_val);
        PyList_SET_ITEM(rms[i], t / rms_window, item);
        if(rms_val > max_rms)
          max_rms = rms_val;
      }
    }
  }
  Py_DECREF(signal);

  resons_list = PyList_New(nfreqs);
  rms_list = PyList_New(nfreqs);
  for(i = 0; i < nfreqs; i ++) {
    PyList_SET_ITEM(resons_list, i, resons[i]);
    PyList_SET_ITEM(rms_list, i, rms[i]);
  }

  return Py_BuildValue("(O,O,d)", resons_list, rms_list, max_rms);
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
