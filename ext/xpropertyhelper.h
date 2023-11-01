/*!
    \file xpropertyhelper.h
    \brief This file contains three macros.

    These macros are intended to reduce the amount of boilerplate code the user has to write when using the Qt Frameworks's Q_PROPERTY feature.

    <b>Usage Notes</b>

    Use these macros in the private: section of your header only. They use both public and private keywords and leave the header in the private section.
    Putting these macros directly under the Q_OBJECT macro at the top of the class is acceptable.
    If for some reason you have to use these macros in the public section it is your responsibility to add the public: keyword after the macro.

    <b>Warning!</b>
    Note that these macros are only intended to be used as shown. The parameters are NOT intended to be expressions but simply the type and name.
    The parameters aren't parenthesized in the body of the macro and the entire macro is not parenthesized. Enjoy responsibly.

    <b>Note on Initializers</b>
    If you uses these macros at the top of your class then the member variables they create will be the first ones in your class.
    Therefore, to make tools like lint happy, you should add these variables to your initializer list in the order they are declared; before any variables that come after these macros.
    If you want to fully initialize variables in the order they are declared then even AUTO_PROPERTY created variables will need to be accessed directly.
    There is no harm in doing this. In general I was trying to adhere to the philosophy of the member variables being hidden and only the property being used.

    <b>Example Usages</b>
    \code
        AUTO_PROPERTY(QString, MyProperty)
        BOOL_PROPERTY(MyFlag)
        READONLY_PROPERTY(double, MyValueProp)
    \endcode
 */
#ifndef XPROPERTYHELPER_H
#define XPROPERTYHELPER_H
#include <QObject>

/*!
    \brief creates a typed property with getter, setter and sygnal
    \code
        AUTO_PROPERTY(TYPE, NAME)
    \endcode
    It will create a standard macro of the passed type and name.
    \code
        Q_PROPERTY (TYPE NAME READ getNAME WRITE setName NOTIFY NAMEChanged)
    \endcode
    In addition, it will create a standard getter named getNAME, setter named setNAME(TYPE value) and signal named NAMEChanged(TYPE value)
    It will also create the member data variable.
    The member var is named with an a_ prefix because it is intended to remind the user to NOT use the member variable name directly.
    The user should use the property name instead. If you don't like the a_ prefix change the macro.
*/

#define AUTO_PROPERTY(TYPE, NAME) \
    Q_PROPERTY(TYPE NAME READ get ## NAME WRITE set ## NAME NOTIFY NAME ## Changed ) \
    public: \
       TYPE get ## NAME() const { return a_ ## NAME ; } \
       Q_SLOT void set ## NAME(TYPE value) { \
          if (a_ ## NAME == value)  return; \
          a_ ## NAME = value; \
          emit NAME ## Changed(value); \
        } \
       Q_SIGNAL void NAME ## Changed(TYPE value);\
    private: \
       TYPE a_ ## NAME;

/*!
    \brief creates a bool property with getter, setter and sygnal
    \code
        BOOL_PROPERTY(NAME)
    \endcode
    It will create a standard macro of the name.
    \code
        Q_PROPERTY (bool NAME READ isNAME WRITE setName NOTIFY NAMEChanged)
    \endcode
    In addition, it will create a standard getter named isNAME, setter named setNAME(TYPE value) and signal named NAMEChanged(TYPE value)
    It will also create the member data variable with b_ prefix.
 */
#define BOOL_PROPERTY(NAME) \
    Q_PROPERTY(bool NAME READ is ## NAME WRITE set ## NAME NOTIFY NAME ## Changed ) \
    public: \
       bool is ## NAME() const { return b_ ## NAME ; } \
       Q_SLOT void set ## NAME(bool value) { \
          if (b_ ## NAME == value)  return; \
          b_ ## NAME = value; \
          emit NAME ## Changed(value); \
        } \
       Q_SIGNAL void NAME ## Changed(bool value);\
    private: \
       bool b_ ## NAME;

/*!
    \brief creates a typed readonly property with getter
    \code
        READONLY_PROPERTY(TYPE, NAME)
    \endcode
    will create a standard
    \code
        Q_PROPERTY( TYPE NAME READ NAME CONST)
    \endcode
    It will create the getter and the member variable name. It also creates a private setter function.
    In this macro, the member variable again uses r_ but the user is expected to initialize this variable directly.
    All reads of the variable should be done through the property for consistency.
 */
#define READONLY_PROPERTY(TYPE, NAME) \
    Q_PROPERTY(TYPE NAME READ NAME CONSTANT ) \
    public: \
       TYPE NAME() const { return r_ ## NAME ; } \
    private: \
       void set ## NAME(TYPE value) {r_ ## NAME = value; } \
       TYPE r_ ## NAME;

#endif // XPROPERTYHELPER_H
