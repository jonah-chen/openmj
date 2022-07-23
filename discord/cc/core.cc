#include "core/mahjong/constants.hpp"
#include <Python.h>

static PyObject *ping(PyObject *self, PyObject *args)
{
    return Py_BuildValue("s", "pong");
}

static PyObject *help(PyObject *self, PyObject *args)
{
    static std::string help_str;
    if (help_str.empty())
    {
        std::ifstream ifs("assets/help.txt");
        if (!ifs)
            return NULL;
        ifs.seekg(0, std::ios::end);
        help_str.reserve(ifs.tellg());
        ifs.seekg(0, std::ios::beg);
        help_str.assign((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    }

    return Py_BuildValue("s", help_str.c_str());
}

// name, fn_ptr, args_type, docstring
static PyMethodDef methods[] = {{"ping", ping, METH_VARARGS, "ping the server"},
                                {"help", help, METH_VARARGS, "help function"},
                                {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {PyModuleDef_HEAD_INIT, "core", NULL, -1,
                                    methods};

PyMODINIT_FUNC PyInit_core(void) { return PyModule_Create(&module); }
