#include <Python.h>
#include "sims/riichi.hpp"
#include "core/io/yaml.hpp"

static PyObject *py_test(PyObject *self, PyObject *args)
{
    std::minstd_rand rng{10};
    mj::sim::RiichiState state("m123p456s789w22d11", mj::k_West);
    state.hand.flags =
        (mj::scoring::f_NormalPlay & mj::scoring::f_ClosedHandMask) |
        mj::scoring::f_Riichi1 | mj::scoring::f_EastP | mj::scoring::f_WestS;
    state.furiten = false;
    // run the simulation
    auto [a, b, c, d] = mj::sim::riichi(state, 8u, 100000u / 8);

    auto amax = *std::max_element(a.begin(), a.end());
    if (amax > 0)
    {
        for (auto [idx, count] : mj::enumerate(a))
        {
            std::cout << std::setw(2) << idx << ": ";
            for ([[maybe_unused]] auto i : mj::range(20 * count / amax))
                std::cout << "*";
            std::cout << "\n";
        }
    }
    std::cout << "\nScores\n";
    auto max_for_map = std::max_element(b.begin(), b.end(), [](auto &a, auto &b) {
        return a.second < b.second;
    })->second;
    for (auto [score, count] : b)
    {
        std::cout << std::setw(5) << score << ": ";
        for ([[maybe_unused]] auto i : mj::range(20 * count / max_for_map))
            std::cout << "*";
        std::cout << "\n";
    }
    return Py_None;
}

// build
static PyMethodDef methods[] = {{"test", py_test, METH_VARARGS, "test"},
                                {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {PyModuleDef_HEAD_INIT, "test", NULL, -1,
                                    methods};

PyMODINIT_FUNC PyInit_test(void) { return PyModule_Create(&module); }