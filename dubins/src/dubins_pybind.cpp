#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <stdexcept>
#include <iostream>
#include "dubins.hpp"

namespace py = pybind11;
using namespace dubins;

class _DubinsPath
{
public:
    _DubinsPath() {}
    _DubinsPath(const DubinsPath &ppth) : ppth(ppth) {}
    _DubinsPath(const _DubinsPath &path) : ppth(path.ppth) {}

    static _DubinsPath shortest_path(const std::array<double, 3> &q0, const std::array<double, 3> &q1, double rho)
    {
        _DubinsPath path;
        int code = dubins_shortest_path(path.ppth, q0, q1, rho);
        if (code != 0)
        {
            throw std::runtime_error("Path did not initialize correctly");
        }
        return path;
    }

    static _DubinsPath path(const std::array<double, 3> &q0, const std::array<double, 3> &q1, double rho, int word)
    {
        _DubinsPath path;
        int code = dubins_path(path.ppth, q0, q1, rho, static_cast<DubinsPathType>(word));
        if (code != 0)
        {
            throw std::runtime_error("Path did not initialize correctly");
        }
        return path;
    }

    std::tuple<double, double, double> path_endpoint()
    {
        std::array<double, 3> q0;
        int code = dubins_path_endpoint(ppth, q0);
        if (code != 0)
        {
            throw std::runtime_error("endpoint not found");
        }
        return std::make_tuple(q0[0], q0[1], q0[2]);
    }

    /* Identify the total length of the path */
    double path_length()
    {
        return dubins_path_length(ppth);
    }

    /* Identify the length of the i-th segment within the path */
    double segment_length(int i)
    {
        return dubins_segment_length(ppth, i);
    }

    /* Identify the normalized length of the i-th segment within the path */
    double segment_length_normalized(int i)
    {
        return dubins_segment_length_normalized(ppth, i);
    }

    /* Identify the type of path which applies */
    int path_type()
    {
        return dubins_path_type(ppth);
    }

    /* Sample the path */
    std::tuple<double, double, double> sample(double t)
    {
        std::array<double, 3> q0;
        int code = dubins_path_sample(ppth, t, q0);
        if (code != 0)
        {
            throw std::runtime_error("sample not found");
        }
        return std::make_tuple(q0[0], q0[1], q0[2]);
    }

    /* Sample the path */
    std::tuple<std::vector<std::array<double, 3>>, std::vector<double>> sample_many(double step_size)
    {
        std::vector<std::array<double, 3>> qs;
        std::vector<double> ts;

        // struct CallbackData
        // {
        //     std::vector<std::array<double, 3>> &qs;
        //     std::vector<double> &ts;
        // };
        // CallbackData data{qs, ts};

        // DubinsPathSamplingCallback callback = [](std::array<double, 3>& q, double t, void* user_data) -> int
        // {
        //     auto data = static_cast<CallbackData*>(user_data);
        //     data->qs.push_back(q);
        //     data->ts.push_back(t);
        //     return 0;
        // };

        // dubins_path_sample_many(ppth, step_size, callback, &data);
        // return std::make_tuple(qs, ts);

        DubinsPathSamplingCallback callback = [&](std::array<double, 3> &q, double t, void *) -> int
        {
            qs.push_back(q);
            ts.push_back(t);
            return 0;
        };

        dubins_path_sample_many(ppth, step_size, callback, nullptr);
        return std::make_tuple(std::move(qs), std::move(ts));
    }

    /* Extract a subpath */
    _DubinsPath extract_subpath(double t)
    {
        _DubinsPath newpath;
        int code = dubins_extract_subpath(ppth, t, newpath.ppth);
        if (code != 0)
        {
            throw std::runtime_error("invalid subpath");
        }
        return newpath;
    }

private:
    DubinsPath ppth;
};

PYBIND11_MODULE(dubins, m)
{
    m.doc() = "dubins path library";

    m.attr("LSL") = static_cast<int>(DubinsPathType::LSL);
    m.attr("LSR") = static_cast<int>(DubinsPathType::LSR);
    m.attr("RSL") = static_cast<int>(DubinsPathType::RSL);
    m.attr("RSR") = static_cast<int>(DubinsPathType::RSR);
    m.attr("RLR") = static_cast<int>(DubinsPathType::RLR);
    m.attr("LRL") = static_cast<int>(DubinsPathType::LRL);

    py::class_<_DubinsPath>(m, "_DubinsPath")
        .def(py::init<>())
        .def_static("shortest_path", &_DubinsPath::shortest_path)
        .def("path_endpoint", &_DubinsPath::path_endpoint)
        .def("path_length", &_DubinsPath::path_length, "Identify the total length of the path")
        .def("segment_length", &_DubinsPath::segment_length, "Identify the length of the i-th segment within the path")
        .def("segment_length_normalized", &_DubinsPath::segment_length_normalized, "Identify the normalized length of the i-th segment within the path")
        .def("path_type", &_DubinsPath::path_type, "Identify the type of path which applies")
        .def("sample", &_DubinsPath::sample, "Sample the path")
        .def("sample_many", &_DubinsPath::sample_many, "Sample the entire path")
        .def("extract_subpath", &_DubinsPath::extract_subpath, "Extract a subpath");

    m.def(
        "path_sample", [](std::array<double, 3> q0, std::array<double, 3> q1, double rho, double step_size)
        {
        _DubinsPath path = _DubinsPath::shortest_path(q0, q1, rho);
        return path.sample_many(step_size); },
        R"pbdoc(
        Shortest path between dubins configurations

        Parameters
        ----------
        q0 : array-like
            the initial configuration
        q1 : array-like
            the final configuration
        rho : float
            the turning radius of the vehicle

        Raises
        ------
        RuntimeError
            If the construction of the path fails

        Returns
        -------
        path : DubinsPath 
            The shortest path
    )pbdoc");

    m.def(
        "shortest_path", [](std::array<double, 3> q0, std::array<double, 3> q1, double rho)
        { return _DubinsPath::shortest_path(q0, q1, rho); },
        R"pbdoc(
        Generate points along a Dubin's path sampled at a regular interval 

        Parameters
        ----------
        q0 : array-like
            the initial configuration
        q1 : array-like
            the final configuration
        rho : float
            the turning radius of the vehicle
        step_size : float
            the distance along the path to sample 

        Raises
        ------
        RuntimeError
            If the construction of the path fails

        Returns
        -------
        result : tuple of two lists
            The first element are sampled configurations, the second are distances
    )pbdoc");

    m.def(
        "path", [](std::array<double, 3> q0, std::array<double, 3> q1, double rho, int word)
        {
        _DubinsPath path;
        path.path(q0, q1, rho, word);
        return path; },
        R"pbdoc(
        Find the Dubin's path for one specific word

        Parameters
        ----------
        q0 : array-like
            the initial configuration
        q1 : array-like
            the final configuration
        rho : float
            the turning radius of the vehicle
        word : int
            the control word (LSL, LSR, ...)

        Raises
        ------
        RuntimeError
            If the construction of the path fails

        Returns
        -------
        path : _DubinsPath 
            The path with the specified word (if one exists) or None
    '''
    )pbdoc");

    m.def(
        "norm_path", [m](double alpha, double beta, double delta, int word)
        {
        std::array<double, 3> q0 = {0.0, 0.0, alpha};
        std::array<double, 3> q1 = {delta, 0.0, beta};
        return m.attr("path")(q0, q1, 1.0, word); },
        R"pbdoc(
        Find the Dubin's path for one specific word assuming a normalized (alpha, beta, delta) frame

        Parameters
        ----------
        alpha : float
            the initial orientation 
        beta : flaot
            the final orientation
        delta : float
            the distance between configurations
        word : int
            the control word (LSL, LSR, ...)

        Raises
        ------
        RuntimeError
            If the construction of the path fails

        Returns
        -------
        path : DubinsPath 
            The path with the specified word (if one exists) or None
    )pbdoc");
}