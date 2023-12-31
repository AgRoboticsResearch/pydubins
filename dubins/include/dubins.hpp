/*
 * Copyright (c) 2008-2018, Andrew Walker
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef DUBINS_H
#define DUBINS_H

#include <functional>
#include <array>

namespace dubins
{

    enum class DubinsPathType
    {
        LSL = 0,
        LSR = 1,
        RSL = 2,
        RSR = 3,
        RLR = 4,
        LRL = 5
    };

    enum class SegmentType
    {
        L_SEG = 0,
        S_SEG = 1,
        R_SEG = 2
    };

    struct DubinsPath
    {
        std::array<double, 3> qi;    // the initial configuration
        std::array<double, 3> param; // the lengths of the three segments
        double rho;                  // model forward velocity / model angular velocity
        DubinsPathType type;         // the path type described
    };

    constexpr int EDUBOK = 0;        // No error
    constexpr int EDUBCOCONFIGS = 1; // Colocated configurations
    constexpr int EDUBPARAM = 2;     // Path parameterisitation error
    constexpr int EDUBBADRHO = 3;    // the rho value is invalid
    constexpr int EDUBNOPATH = 4;    // no connection between configurations with this word

    /**
     * Callback function for path sampling
     *
     * @note the q parameter is a configuration
     * @note the t parameter is the distance along the path
     * @note the user_data parameter is forwarded from the caller
     * @note return non-zero to denote sampling should be stopped
     */
    // using DubinsPathSamplingCallback = int (*)(std::array<double, 3> &q, double t, void *user_data);
    using DubinsPathSamplingCallback = std::function<int(std::array<double, 3>&, double, void*)>;

    /**
     * Generate a path from an initial configuration to
     * a target configuration, with a specified maximum turning
     * radii
     *
     * A configuration is (x, y, theta), where theta is in radians, with zero
     * along the line x = 0, and counter-clockwise is positive
     *
     * @param path  - the resultant path
     * @param q0    - a configuration specified as an array of x, y, theta
     * @param q1    - a configuration specified as an array of x, y, theta
     * @param rho   - turning radius of the vehicle (forward velocity divided by maximum angular velocity)
     * @return      - non-zero on error
     */
    int dubins_shortest_path(DubinsPath &path,
                             const std::array<double, 3> &q0,
                             const std::array<double, 3> &q1,
                             double rho);

    /**
     * Generate a path with a specified word from an initial configuration to
     * a target configuration, with a specified turning radius
     *
     * @param path     - the resultant path
     * @param q0       - a configuration specified as an array of x, y, theta
     * @param q1       - a configuration specified as an array of x, y, theta
     * @param rho      - turning radius of the vehicle (forward velocity divided by maximum angular velocity)
     * @param pathType - the specific path type to use
     * @return         - non-zero on error
     */
    int dubins_path(DubinsPath &path,
                    const std::array<double, 3> &q0,
                    const std::array<double, 3> &q1,
                    double rho,
                    DubinsPathType pathType);

    /**
     * Calculate the length of an initialised path
     *
     * @param path - the path to find the length of
     */
    double dubins_path_length(const DubinsPath &path);

    /**
     * Return the length of a specific segment in an initialized path
     *
     * @param path - the path to find the length of
     * @param i    - the segment you to get the length of (0-2)
     */
    double dubins_segment_length(const DubinsPath &path, int i);

    /**
     * Return the normalized length of a specific segment in an initialized path
     *
     * @param path - the path to find the length of
     * @param i    - the segment you to get the length of (0-2)
     */
    double dubins_segment_length_normalized(const DubinsPath &path, int i);

    /**
     * Extract an integer that represents which path type was used
     *
     * @param path    - an initialised path
     * @return        - one of LSL, LSR, RSL, RSR, RLR or LRL
     */
    int dubins_path_type(const DubinsPath &path);

    /**
     * Calculate the configuration along the path, using the parameter t
     *
     * @param path - an initialised path
     * @param t    - a length measure, where 0 <= t < dubins_path_length(path)
     * @param q    - the configuration result
     * @returns    - non-zero if 't' is not in the correct range
     */
    int dubins_path_sample(const DubinsPath &path, double t, std::array<double, 3> &q);

    /**
     * Walk along the path at a fixed sampling interval, calling the
     * callback function at each interval
     *
     * The sampling process continues until the whole path is sampled, or the callback returns a non-zero value
     *
     * @param path      - the path to sample
     * @param stepSize  - the distance along the path for subsequent samples
     * @param cb        - the callback function to call for each sample
     * @param user_data - optional information to pass on to the callback
     *
     * @returns - zero on successful completion, or the result of the callback
     */
    int dubins_path_sample_many(const DubinsPath &path,
                                double stepSize,
                                DubinsPathSamplingCallback cb,
                                void *user_data);

    /**
     * Convenience function to identify the endpoint of a path
     *
     * @param path - an initialised path
     * @param q    - the configuration result
     */
    int dubins_path_endpoint(const DubinsPath &path, std::array<double, 3> &q);

    /**
     * Convenience function to extract a subset of a path
     *
     * @param path    - an initialised path
     * @param t       - a length measure, where 0 < t < dubins_path_length(path)
     * @param newpath - the resultant path
     */
    int dubins_extract_subpath(const DubinsPath &path, double t, DubinsPath &newpath);

    // The segment types for each of the Path types
    const SegmentType DIRDATA[][3] = {
        {SegmentType::L_SEG, SegmentType::S_SEG, SegmentType::L_SEG},
        {SegmentType::L_SEG, SegmentType::S_SEG, SegmentType::R_SEG},
        {SegmentType::R_SEG, SegmentType::S_SEG, SegmentType::L_SEG},
        {SegmentType::R_SEG, SegmentType::S_SEG, SegmentType::R_SEG},
        {SegmentType::R_SEG, SegmentType::L_SEG, SegmentType::R_SEG},
        {SegmentType::L_SEG, SegmentType::R_SEG, SegmentType::L_SEG}};

    struct DubinsIntermediateResults
    {
        double alpha;
        double beta;
        double d;
        double sa;
        double sb;
        double ca;
        double cb;
        double c_ab;
        double d_sq;
    };

    int dubins_word(const DubinsIntermediateResults &in,
                    DubinsPathType pathType,
                    std::array<double, 3> &out);

    int dubins_intermediate_results(DubinsIntermediateResults &in,
                                    const std::array<double, 3> &q0,
                                    const std::array<double, 3> &q1,
                                    double rho);

} // namespace dubins

#endif /* DUBINS_H */