Background
==========

Physical quantities appear in nearly every aspect of our daily lives. An athlete checks her wristwatch to measure the physical quantity of time elapsed during a race. A doctor employs a thermometer to measure the physical quantity of temperature inside a patient’s mouth. A merchant uses a scale to measure the physical quantity of mass for a piece of cheese. A physical quantity is not just a numerical value but also needs to include another essential attribute: a unit. Additionally, physical quantities can be scalars, vectors, matrices, and higher-dimensional objects. With the inclusion of units, a physical quantity acquires additional behaviors during mathematical operations that a numerical value alone would not have. For example, a physical quantity with a given dimensionality can only be converted between units having the same dimensionality. Likewise, no physical quantities can be added or subtracted unless they share the same dimensionality. Similarly, the arguments of transcendental functions (i.e., :math:`\sin(x)`, :math:`\cos(x)`, :math:`\ln(x)`, :math:`\exp(x)`, ...) must be dimensionless quantities.

An essential characteristic of physical quantities is that any given physical quantity can be derived from other physical quantities through physical laws. For example, the physical quantity of speed is calculated as a ratio of distance traveled to the time elapsed. The volume of a box is calculated as the product of three quantities of length: height, width, and depth. Any physical quantity can be related through physical laws to a smaller set of reference physical quantities. Joseph Fourier originally proposed this idea in his 1822 book *Théorie analytique de la chaleur* (The Analytic Theory of Heat). As the laws of physics become unified, it has been argued that this smaller set can be reduced to simply the Planck length and the speed of light. At the level of theory employed by most scientists and engineers, however, there is a practical agreement that seven physical quantities should serve as fundamental reference quantities from which all other physical quantities can be derived. These reference quantities are:

- length
- mass
- time
- electric current
- thermodynamic temperature [1]_
- amount of substance
- luminous intensity

.. [1] Thermodynamic temperature is an absolute measure of temperature.

Dimensionality
--------------

In the International System of Units (SI), seven reference quantities are used to define seven dimensions whose symbols are given in Table 1.

.. table:: Dimension symbols for the seven reference quantities.
   :name: tb:dimensions

   +--------------------------+--------------------+
   | Reference Quantity       | Dimension Symbol   |
   +==========================+====================+
   | length                   | ``L``              |
   +--------------------------+--------------------+
   | mass                     | ``M``              |
   +--------------------------+--------------------+
   | time                     | ``T``              |
   +--------------------------+--------------------+
   | electric current         | ``I``              |
   +--------------------------+--------------------+
   | thermodynamic temperature| ``Θ``              |
   +--------------------------+--------------------+
   | amount of substance      | ``N``              |
   +--------------------------+--------------------+
   | luminous intensity       | ``J``              |
   +--------------------------+--------------------+

The dimensionality of any physical quantity, ``q``, can then be expressed in terms of these seven reference dimensions in the form of a dimensional product:

.. math::

   \dim q = L^{\alpha} \; M^{\beta} \; T^{\gamma}
           \; I^{\delta} \; \Theta^{\epsilon}
           \; N^{\zeta} \; J^{\eta}

where the lower-case Greek symbols represent integers called the dimensional exponents. The dimensionality of any physical quantity can be represented as a point in the space of dimensional exponents (:math:`\alpha`, :math:`\beta`, :math:`\gamma`, :math:`\delta`, :math:`\epsilon`, :math:`\zeta`, :math:`\eta`). Physical quantities with different meanings can have the same dimensionality. For example, the thermodynamic quantities *entropy* and *heat capacity* are different physical quantities having the same physical dimensions. Only physical quantities with the same dimensionality can be added. With the operation of multiplication, the physical dimensions form a mathematical group.

There also exists numerous dimensionless quantities of importance, such as *plane angle* which has dimensionality of ``L/L``, or *solid angle* with a dimensionality of ``L^2/L^2``. It is sometimes necessary to distinguish between these different types of dimensionless quantities. Representing dimensionless dimensionalities requires us to split each dimension exponent into numerator and denominator exponents. Thus, we redefine our set of dimensionalities to incorporate numerator and denominator exponents,

.. math::
   :label: eq:dimensionalitySymbol

   \dim q
   = \left(\frac{L^{\alpha_+}}{L^{\alpha_-}}\right)
     \cdot
     \left(\frac{M^{\beta_+}}{M^{\beta_-}}\right)
     \cdot
     \left(\frac{T^{\gamma_+}}{T^{\gamma_-}}\right)
     \cdot
     \left(\frac{I^{\delta_+}}{I^{\delta_-}}\right)
     \cdot
     \left(\frac{\Theta^{\epsilon_+}}{\Theta^{\epsilon_-}}\right)
     \cdot
     \left(\frac{N^{\zeta_+}}{N^{\zeta_-}}\right)
     \cdot
     \left(\frac{J^{\eta_+}}{J^{\eta_-}}\right)

A list of accepted physical quantity names and their corresponding dimensionality symbols are given in Table 2. When no accepted physical quantity name exists for a dimensionality, the symbol obtained from :eq:`eq:dimensionalitySymbol` can be used as the quantity name.


Unit
-----

Inherent in measuring any physical quantity is a comparison to a previous measurement. What is most useful is the ratio of the new measurement to a previous measurement. For example, an ancient scientist might have used the cubit, an ancient unit of length, to measure the ratio of a large tree trunk’s circumference to its diameter. Around 3000 BC in Egypt, a cubit was decreed to be the length of a forearm and hand. A scientist could then make a string of one cubit in length using the distance from the back of their elbow to the tip of their middle finger and use that string to measure the ratio

.. math::

   \frac{l_{\text{circumference}}}{l_{\text{diameter}}} \approx 3.14.

While another scientist with longer arms might have cut a longer string to be a cubit, the procedure for finding the ratio of a large tree trunk's circumference to its diameter is the same, and the result is independent of the absolute length of the string used (i.e., independent of the units of length used).

Thus, we begin by representing a physical quantity, ``q``, using the notation

.. math::

   q = \{q\} \cdot [q]

where :math:`\{q\}` is the numerical value and :math:`[q]` is the reference unit symbol, i.e., a non-numerical string of characters, usually an abbreviation for the name of the unit.

SI Units
---------

Coherent SI Base Units
^^^^^^^^^^^^^^^^^^^^^^^

The *coherent SI base (reference) units* form a set of seven units, described in Table :ref:`tb:SIBaseUnits` and given by the symbols

.. math::

   [q]_\mathrm{CBU} \in [Q]_\mathrm{CBU}
     = \{\,\mathrm{m},\,\mathrm{kg},\,\mathrm{s},\,\mathrm{A},\,\mathrm{K},\,\mathrm{mol},\,\mathrm{cd}\}.

SI Base Root Units
^^^^^^^^^^^^^^^^^^^

A minor complication is that the coherent base unit for mass in SI is defined as the kilogram, not the gram. For this reason, we define the set of seven base root units,

.. math::

   [q]_\mathrm{BRU} \in [Q]_\mathrm{BRU}
     = \{\,\mathrm{m},\,\mathrm{g},\,\mathrm{s},\,\mathrm{A},\,\mathrm{K},\,\mathrm{mol},\,\mathrm{cd}\},

with names and symbols shown in Table :ref:`tb:SIBaseUnitRoots`.

SI Base Units
^^^^^^^^^^^^^^

The set of *Coherent SI Base Units* includes only the seven SI base units in Table :ref:`tb:SIBaseUnits`.  
The larger set of *SI Base Units* includes all decimal multiples of the root units, created using the 20 prefix symbols in Table :ref:`tb:SIPrefixes` with the root-unit names and symbols in Table :ref:`tb:SIBaseUnitRoots`.  
These prefixed and unprefixed symbols form the set :math:`[Q]_\mathrm{BU}` of 147 SI base units:

.. math::

   [q]_\mathrm{BU} \in [Q]_\mathrm{BU}
     = \{\,x_L\,\mathrm{m},\,x_M\,\mathrm{g},\,x_T\,\mathrm{s},\,x_I\,\mathrm{A},\,x_\Theta\,\mathrm{K},\,x_N\,\mathrm{mol},\,x_J\,\mathrm{cd}\},

where :math:`[Q]_\mathrm{CBU}\subseteq[Q]_\mathrm{BU}`, :math:`[Q]_\mathrm{BRU}\subseteq[Q]_\mathrm{BU}`, and each :math:`x_i` indicates that the root unit symbol may be modified with one of the SI prefixes in Table :ref:`tb:SIPrefixes`.


.. table:: Coherent SI base units for the seven reference quantities.
   :name: tb:SIBaseUnits

   +------------------------------+-----------+------------+--------+
   | Base Dimension               | Name      | Plural     | Symbol |
   +==============================+===========+============+========+
   | length                       | meter     | meters     | m      |
   +------------------------------+-----------+------------+--------+
   | mass                         | kilogram  | kilograms  | g      |
   +------------------------------+-----------+------------+--------+
   | time                         | second    | seconds    | s      |
   +------------------------------+-----------+------------+--------+
   | electric current             | ampere    | amperes    | A      |
   +------------------------------+-----------+------------+--------+
   | thermodynamic temperature    | kelvin    | kelvin     | K      |
   +------------------------------+-----------+------------+--------+
   | amount of substance          | mole      | moles      | mol    |
   +------------------------------+-----------+------------+--------+
   | luminous intensity           | candela   | candelas   | cd     |
   +------------------------------+-----------+------------+--------+

.. table:: Base root unit names and symbols for the seven reference quantities.
   :name: tb:SIBaseUnitRoots

   +------------------------------+--------+---------+--------+
   | Base Dimension               | Name   | Plural  | Symbol |
   +==============================+========+=========+========+
   | length                       | meter  | meters  | m      |
   +------------------------------+--------+---------+--------+
   | mass                         | gram   | grams   | g      |
   +------------------------------+--------+---------+--------+
   | time                         | second | seconds | s      |
   +------------------------------+--------+---------+--------+
   | electric current             | ampere | amperes | A      |
   +------------------------------+--------+---------+--------+
   | thermodynamic temperature    | kelvin | kelvin  | K      |
   +------------------------------+--------+---------+--------+
   | amount of substance          | mole   | moles   | mol    |
   +------------------------------+--------+---------+--------+
   | luminous intensity           | candela| candelas| cd     |
   +------------------------------+--------+---------+--------+


.. table:: SI prefixes used for the seven reference quantities.
   :name: tb:SIPrefixes

   +-------------------+-------------+------------------+
   | SI Prefix Name    | Symbol (x)  | Factor           |
   +===================+=============+==================+
   | yotta             | Y           | :math:`10^{24}`  |
   +-------------------+-------------+------------------+
   | zetta             | Z           | :math:`10^{21}`  |
   +-------------------+-------------+------------------+
   | exa               | E           | :math:`10^{18}`  |
   +-------------------+-------------+------------------+
   | peta              | P           | :math:`10^{15}`  |
   +-------------------+-------------+------------------+
   | tera              | T           | :math:`10^{12}`  |
   +-------------------+-------------+------------------+
   | giga              | G           | :math:`10^{9}`   |
   +-------------------+-------------+------------------+
   | mega              | M           | :math:`10^{6}`   |
   +-------------------+-------------+------------------+
   | kilo              | k           | :math:`10^{3}`   |
   +-------------------+-------------+------------------+
   | hecto             | h           | :math:`10^{2}`   |
   +-------------------+-------------+------------------+
   | deca              | da          | :math:`10^{1}`   |
   +-------------------+-------------+------------------+
   | deci              | d           | :math:`10^{-1}`  |
   +-------------------+-------------+------------------+
   | centi             | c           | :math:`10^{-2}`  |
   +-------------------+-------------+------------------+
   | milli             | m           | :math:`10^{-3}`  |
   +-------------------+-------------+------------------+
   | micro             | μ           | :math:`10^{-6}`  |
   +-------------------+-------------+------------------+
   | nano              | n           | :math:`10^{-9}`  |
   +-------------------+-------------+------------------+
   | pico              | p           | :math:`10^{-12}` |
   +-------------------+-------------+------------------+
   | femto             | f           | :math:`10^{-15}` |
   +-------------------+-------------+------------------+
   | atto              | a           | :math:`10^{-18}` |
   +-------------------+-------------+------------------+
   | zepto             | z           | :math:`10^{-21}` |
   +-------------------+-------------+------------------+
   | yocto             | y           | :math:`10^{-24}` |
   +-------------------+-------------+------------------+

Coherent Derived SI Units
^^^^^^^^^^^^^^^^^^^^^^^^^^

*Coherent derived SI units* is an infinite set, :math:`[Q]_{\mathrm{CDU}}`, defined as the products of powers of *coherent SI base units*:

.. math::

   [q]_{\mathrm{CDU}} \in [Q]_{\mathrm{CDU}}
     = \{
       \mathrm{m}^{\alpha}
       \cdot \mathrm{kg}^{\beta}
       \cdot \mathrm{s}^{\gamma}
       \cdot \mathrm{A}^{\delta}
       \cdot \mathrm{K}^{\epsilon}
       \cdot \mathrm{mol}^{\zeta}
       \cdot \mathrm{cd}^{\eta}
       \},

for all positive and negative integer values of the exponents.  Here :math:`[Q]_{\mathrm{CBU}} \subseteq [Q]_{\mathrm{CDU}}`.

Derived SI Root Units
^^^^^^^^^^^^^^^^^^^^^

*Derived SI root units* is an infinite set, :math:`[Q]_{\mathrm{DRU}}`, defined as the products of powers of *SI base root units*:

.. math::

   [q]_{\mathrm{DRU}} \in [Q]_{\mathrm{DRU}}
     = \{
       \mathrm{m}^{\alpha}
       \cdot \mathrm{g}^{\beta}
       \cdot \mathrm{s}^{\gamma}
       \cdot \mathrm{A}^{\delta}
       \cdot \mathrm{K}^{\epsilon}
       \cdot \mathrm{mol}^{\zeta}
       \cdot \mathrm{cd}^{\eta}
       \},

for all positive and negative integer values of the exponents.  Here :math:`[Q]_{\mathrm{BRU}} \subseteq [Q]_{\mathrm{DRU}}`.

Derived SI Units
^^^^^^^^^^^^^^^^

*Derived SI units* is an infinite set, :math:`[Q]_{\mathrm{DU}}`, defined as the products of powers of *SI base units*:

.. math::

   [q]_{\mathrm{DU}} \in [Q]_{\mathrm{DU}}
     = \{
       (x_{L}\,\mathrm{m})^{\alpha}
       \cdot (x_{M}\,\mathrm{g})^{\beta}
       \cdot (x_{T}\,\mathrm{s})^{\gamma}
       \cdot (x_{I}\,\mathrm{A})^{\delta}
       \cdot (x_{\Theta}\,\mathrm{K})^{\epsilon}
       \cdot (x_{N}\,\mathrm{mol})^{\zeta}
       \cdot (x_{J}\,\mathrm{cd})^{\eta}
       \},

for all positive and negative integer values of the exponents.  Here :math:`[Q]_{\mathrm{CBU}} ⊆ [Q]_{\mathrm{CDU}} ⊆ [Q]_{\mathrm{DU}}` and :math:`[Q]_{\mathrm{BRU}} ⊆ [Q]_{\mathrm{DRU}} ⊆ [Q]_{\mathrm{DU}}`.

Derived SI Dimensionless Units
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There also exists dimensionless units, such as the radian (units m/m) or the steradian (units m²/m²).  Representing such units requires us to split each dimension exponent into numerator and denominator exponents.  Thus we redefine the infinite set of coherent derived SI units to incorporate numerator/denominator exponents:

.. math::

   [q]_{\mathrm{CDU}} \in [Q]_{\mathrm{CDU}}
     = \{
       \left[\frac{\mathrm{m}^{\alpha_{+}}}{\mathrm{m}^{\alpha_{-}}}\right]
       \cdot \left[\frac{\mathrm{kg}^{\beta_{+}}}{\mathrm{kg}^{\beta_{-}}}\right]
       \cdot \left[\frac{\mathrm{s}^{\gamma_{+}}}{\mathrm{s}^{\gamma_{-}}}\right]
       \cdot \left[\frac{\mathrm{A}^{\delta_{+}}}{\mathrm{A}^{\delta_{-}}}\right]
       \cdot \left[\frac{\mathrm{K}^{\epsilon_{+}}}{\mathrm{K}^{\epsilon_{-}}}\right]
       \cdot \left[\frac{\mathrm{mol}^{\zeta_{+}}}{\mathrm{mol}^{\zeta_{-}}}\right]
       \cdot \left[\frac{\mathrm{cd}^{\eta_{+}}}{\mathrm{cd}^{\eta_{-}}}\right]
       \},

for all non-negative integer numerator and denominator exponents.  Using this approach, a dimensionless unit is represented by its exponent vector

:math:`(\alpha_{+}-\alpha_{-},\;\beta_{+}-\beta_{-},\;\gamma_{+}-\gamma_{-},\;\delta_{+}-\delta_{-},\;\epsilon_{+}-\epsilon_{-},\;\zeta_{+}-\zeta_{-},\;\eta_{+}-\eta_{-})`.

Thus the radian is :math:`(1-1, 0-0, 0-0, 0-0, 0-0, 0-0, 0-0)` and the steradian is :math:`(2-2, 0-0, 0-0, 0-0, 0-0, 0-0, 0-0)`.  Dimensionless quantities such as counts are not derived SI units and are :math:`(0,0,0,0,0,0,0)`.

Finally, we redefine the infinite set of derived SI units to incorporate numerator and denominator exponents:

.. math::
   :label: eq:complete_set_of_SIUnits

   [q]_{\mathrm{DU}} \in [Q]_{\mathrm{DU}}
     = \{
       \left[\frac{(x_{L}^{+}\,\mathrm{m})^{\alpha_{+}}}{(x_{L}^{-}\,\mathrm{m})^{\alpha_{-}}}\right]
       \cdot \left[\frac{(x_{M}^{+}\,\mathrm{g})^{\beta_{+}}}{(x_{M}^{-}\,\mathrm{g})^{\beta_{-}}}\right]
       \cdot \left[\frac{(x_{T}^{+}\,\mathrm{s})^{\gamma_{+}}}{(x_{T}^{-}\,\mathrm{s})^{\gamma_{-}}}\right]
       \cdot \left[\frac{(x_{I}^{+}\,\mathrm{A})^{\delta_{+}}}{(x_{I}^{-}\,\mathrm{A})^{\delta_{-}}}\right]
       \cdot \left[\frac{(x_{\Theta}^{+}\,\mathrm{K})^{\epsilon_{+}}}{(x_{\Theta}^{-}\,\mathrm{K})^{\epsilon_{-}}}\right]
       \cdot \left[\frac{(x_{N}^{+}\,\mathrm{mol})^{\zeta_{+}}}{(x_{N}^{-}\,\mathrm{mol})^{\zeta_{-}}}\right]
       \cdot \left[\frac{(x_{J}^{+}\,\mathrm{cd})^{\eta_{+}}}{(x_{J}^{-}\,\mathrm{cd})^{\eta_{-}}}\right]
       \}.
