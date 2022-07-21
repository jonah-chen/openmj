#include "core/io/yaml.hpp"
#include "sims/riichi.hpp"
#include <Python.h>

constexpr mj::Dir dir_from_char(char c)
{
    switch (c)
    {
    case 'N':
        return mj::k_North;
    case 'E':
        return mj::k_East;
    case 'S':
        return mj::k_South;
    case 'W':
        return mj::k_West;
    default:
        return mj::k_ErrDir;
    }
}
static PyObject *ping(PyObject *self, PyObject *args)
{
    return Py_BuildValue("s", "pong");
}

static PyObject *py_test(PyObject *self, PyObject *args)
{
    // parse args to one string
    char *hand_str;
    if (!PyArg_ParseTuple(args, "s", &hand_str))
        return NULL;

    std::minstd_rand rng{10};
    mj::Dir direction = dir_from_char(hand_str[0]);
    if (direction == mj::k_ErrDir)
        return Py_BuildValue("s", "Invalid direction");
    mj::sim::RiichiState state(hand_str + 1, direction);
    if (state.hand.tenpai().empty())
        return Py_BuildValue("s", "Sorry, your hand is in NO_TEN");

    if (state.hand.ec == mj::Hand::ec_InvalidString)
        return Py_BuildValue("s", "Invalid hand string");
    if (state.hand.ec == mj::Hand::ec_InvalidSize)
        return Py_BuildValue("s", "Invalid hand size");

    std::stringstream ss;
    ss << "```\n";
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
            ss << std::setw(2) << idx << ": ";
            for ([[maybe_unused]] auto i : mj::range(20 * count / amax))
                ss << "*";
            ss << "\n";
        }
    }
    ss << "\nScores\n";
    auto max_for_map =
        std::max_element(b.begin(), b.end(), [](auto &a, auto &b) {
            return a.second < b.second;
        })->second;
    for (auto [score, count] : b)
    {
        ss << std::setw(5) << score << ": ";
        for ([[maybe_unused]] auto i : mj::range(20 * count / max_for_map))
            ss << "*";
        ss << "\n";
    }
    ss << "```\n";
    return Py_BuildValue("s", ss.str().c_str());
}

// name, fn_ptr, args_type, docstring
static PyMethodDef methods[] = {{"ping", ping, METH_VARARGS, "ping"},
                                {"test", py_test, METH_VARARGS, "test"},
                                {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {PyModuleDef_HEAD_INIT, "test", NULL, -1,
                                    methods};

PyMODINIT_FUNC PyInit_test(void) { return PyModule_Create(&module); }