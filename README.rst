======
dubins
======

Important
======

This is a pybind11 version of the original `dubins <https://github.com/AndrewWalker/pydubins>`_ package. It supports python 3.9+ and provides same functionality.

To install the package:

.. code-block:: console

    $ pip install .


Overview
========

This software finds the shortest paths between configurations for the Dubins' car [Dubins57]_, the forward only car-like vehicle with a constrained turning radius. A good description of the equations and basic strategies for doing this are described in section 15.3.1 `"Dubins Curves" <http://planning.cs.uiuc.edu/node821.html>`_ of the book "Planning Algorithms" [LaValle06]_.

The approach used to find paths is based on the algebraic solutions published in [Shkel01]_. However, rather than using angular symmetries to improve performance, the simpler approach to test all possible solutions is used here. 


Example
=======

Discrete Sampling of a Dubin's path at finite step sizes

.. code-block:: python

    import dubins

    q0 = (x0, y0, theta0)
    q1 = (x1, y1, theta1)
    turning_radius = 1.0
    step_size = 0.5

    path = dubins.shortest_path(q0, q1, turning_radius)
    configurations, _ = path.sample_many(step_size)


Acknowledgements
================

This work was completed as part of [Walker11]_. 

* Francis Valentinis
* Royce Smart - who tested early versions of this code while completing his PhD thesis [Smart08]_.

References
==========

.. [Dubins57] Dubins, L. E. (July 1957). "On Curves of Minimal Length with a Constraint on Average Curvature, and with Prescribed Initial and Terminal Positions and Tangents". American Journal of Mathematics 79 (3): 497–516
.. [LaValle06] LaValle, S. M. (2006). "Planning Algorithms". Cambridge University Press
.. [Shkel01] Shkel, A. M. and Lumelsky, V. (2001). "Classification of the Dubins set". Robotics and Autonomous Systems 34 (2001) 179–202
.. [Walker11] Walker, A. (2011). "Hard Real-Time Motion Planning for Autonomous Vehicles", PhD thesis, Swinburne University.
.. [Smart08] Royce, S. (2008). "Evolutionary Control of Autonomous Underwater Vehicles". PhD thesis, RMIT
