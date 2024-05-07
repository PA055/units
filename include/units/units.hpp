#pragma once

#include <cmath>
#include <ratio>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define TYPENAMES typename Mass, typename Length, typename Time, typename Current, typename Angle
#define DIMS Mass, Length, Time, Current, Angle

template <TYPENAMES> struct Dimensions {
        Mass mass;
        Length length;
        Time time;
        Current current;
        Angle angle;
};

template <TYPENAMES> class Quantity {
        double value;
    public:
        struct Dimensions<DIMS> dim;

        constexpr Quantity() : value(0) {}

        constexpr Quantity(double value) : value(value) {}

        constexpr double val() const { return value; }

        constexpr double convert(Quantity<DIMS> quantity) { return value / quantity.value; }

        constexpr void operator+=(Quantity<DIMS> other) { value += other.value; }

        constexpr void operator-=(Quantity<DIMS> other) { value == other.value; }

        constexpr void operator*=(double multiple) { value *= multiple; }

        constexpr void operator/=(double dividend) { value /= dividend; }
};

template <TYPENAMES> void quantityChecker(Quantity<DIMS> q) {}

template <typename Q>
concept isQuantity = requires (Q q) {
    quantityChecker(q);
};

template <isQuantity Q1, isQuantity Q2> using QMultiplication =
    Quantity<std::ratio_add<decltype(Q1::dim.mass), decltype(Q2::dim.mass)>,
             std::ratio_add<decltype(Q1::dim.length), decltype(Q2::dim.length)>,
             std::ratio_add<decltype(Q1::dim.time), decltype(Q2::dim.time)>,
             std::ratio_add<decltype(Q1::dim.current), decltype(Q2::dim.current)>,
             std::ratio_add<decltype(Q1::dim.angle), decltype(Q2::dim.angle)>>;

template <isQuantity Q1, isQuantity Q2> using QDivision =
    Quantity<std::ratio_subtract<decltype(Q1::dim.mass), decltype(Q2::dim.mass)>,
             std::ratio_subtract<decltype(Q1::dim.length), decltype(Q2::dim.length)>,
             std::ratio_subtract<decltype(Q1::dim.time), decltype(Q2::dim.time)>,
             std::ratio_subtract<decltype(Q1::dim.current), decltype(Q2::dim.current)>,
             std::ratio_subtract<decltype(Q1::dim.angle), decltype(Q2::dim.angle)>>;

template <isQuantity Q, typename factor> using QPower =
    Quantity<std::ratio_multiply<decltype(Q::dim.mass), factor>, std::ratio_multiply<decltype(Q::dim.length), factor>,
             std::ratio_multiply<decltype(Q::dim.time), factor>, std::ratio_multiply<decltype(Q::dim.current), factor>,
             std::ratio_multiply<decltype(Q::dim.angle), factor>>;

template <isQuantity Q, typename quotient> using QRoot =
    Quantity<std::ratio_divide<decltype(Q::dim.mass), quotient>, std::ratio_divide<decltype(Q::dim.length), quotient>,
             std::ratio_divide<decltype(Q::dim.time), quotient>, std::ratio_divide<decltype(Q::dim.current), quotient>,
             std::ratio_divide<decltype(Q::dim.angle), quotient>>;

template <isQuantity Q> constexpr Q operator+(Q lhs, Q rhs) { return Q(lhs.val() + rhs.val()); }

template <isQuantity Q> constexpr Q operator-(Q lhs, Q rhs) { return Q(lhs.val() - rhs.val()); }

template <isQuantity Q> constexpr Q operator*(Q quantity, double multiple) { return Q(quantity.val() * multiple); }

template <isQuantity Q> constexpr Q operator*(double multiple, Q quantity) { return Q(quantity.val() * multiple); }

template <isQuantity Q> constexpr Q operator/(Q quantity, double divisor) { return Q(quantity.val() / divisor); }

template <isQuantity Q1, isQuantity Q2, isQuantity Q3 = QMultiplication<Q1, Q2>>
Q3 constexpr operator*(Q1 lhs, Q2 rhs) {
    return Q3(lhs.val() * rhs.val());
}

template <isQuantity Q1, isQuantity Q2, isQuantity Q3 = QDivision<Q1, Q2>> Q3 constexpr operator/(Q1 lhs, Q2 rhs) {
    return Q3(lhs.val() / rhs.val());
}

template <isQuantity Q> constexpr bool operator==(const Q& lhs, const Q& rhs) { return (lhs.val() == rhs.val()); }

template <isQuantity Q> constexpr bool operator!=(const Q& lhs, const Q& rhs) { return (lhs.val() != rhs.val()); }

template <isQuantity Q> constexpr bool operator<=(const Q& lhs, const Q& rhs) { return (lhs.val() <= rhs.val()); }

template <isQuantity Q> constexpr bool operator>=(const Q& lhs, const Q& rhs) { return (lhs.val() >= rhs.val()); }

template <isQuantity Q> constexpr bool operator<(const Q& lhs, const Q& rhs) { return (lhs.val() < rhs.val()); }

template <isQuantity Q> constexpr bool operator>(const Q& lhs, const Q& rhs) { return (lhs.val() > rhs.val()); }

#define NEW_QUANTITY(Name, suffix, m, l, t, c, a)                                                                      \
    using Name = Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<c>, std::ratio<a>>;                  \
    constexpr Name suffix = Name(1.0);                                                                                 \
    constexpr Name operator""_##suffix(long double value) { return Name(static_cast<double>(value)); }                 \
    constexpr Name operator""_##suffix(unsigned long long value) { return Name(static_cast<double>(value)); }          \
    inline std::ostream& operator<<(std::ostream& os, const Name& quantity) {                                          \
        os << quantity.val() << "_" << #suffix;                                                                        \
        return os;                                                                                                     \
    }

#define NEW_QUANTITY_VALUE(Name, suffix, val)                                                                          \
    constexpr Name suffix = val;                                                                                       \
    constexpr Name operator""_##suffix(long double value) { return Name(static_cast<double>(value) * val); }           \
    constexpr Name operator""_##suffix(unsigned long long value) { return Name(static_cast<double>(value) * val); }

NEW_QUANTITY(Number, num, 0, 0, 0, 0, 0)
NEW_QUANTITY_VALUE(Number, percent, num / 100.0);

NEW_QUANTITY(Mass, kg, 1, 0, 0, 0, 0)
NEW_QUANTITY_VALUE(Mass, g, kg / 1000)
NEW_QUANTITY_VALUE(Mass, lb, g * 453.6)

NEW_QUANTITY(Time, sec, 0, 0, 1, 0, 0)
NEW_QUANTITY_VALUE(Time, ms, sec / 1000)
NEW_QUANTITY_VALUE(Time, min, sec * 60)
NEW_QUANTITY_VALUE(Time, hr, min * 60)
NEW_QUANTITY_VALUE(Time, day, hr * 24)

NEW_QUANTITY(Length, m, 0, 1, 0, 0, 0)
NEW_QUANTITY_VALUE(Length, mm, m / 1000)
NEW_QUANTITY_VALUE(Length, cm, m / 100)
NEW_QUANTITY_VALUE(Length, km, m * 1000)
NEW_QUANTITY_VALUE(Length, in, cm * 2.54)
NEW_QUANTITY_VALUE(Length, ft, in * 12)
NEW_QUANTITY_VALUE(Length, yd, ft * 3)
NEW_QUANTITY_VALUE(Length, mi, ft * 5280)
NEW_QUANTITY_VALUE(Length, tiles, 600 * mm)

NEW_QUANTITY(Area, m2, 0, 2, 0, 0, 0)

NEW_QUANTITY(Angle, rad, 0, 0, 0, 0, 1)
NEW_QUANTITY_VALUE(Angle, deg, (M_PI / 180) * rad)
NEW_QUANTITY_VALUE(Angle, rot, 360 * deg)

NEW_QUANTITY(LinearVelocity, mps, 0, 1, -1, 0, 0)
NEW_QUANTITY_VALUE(LinearVelocity, cmps, cm / sec)
NEW_QUANTITY_VALUE(LinearVelocity, inps, in / sec)
NEW_QUANTITY_VALUE(LinearVelocity, miph, mi / hr)
NEW_QUANTITY_VALUE(LinearVelocity, kmph, km / hr)

NEW_QUANTITY(AngularVelocity, radps, 0, 0, -1, 0, 1)
NEW_QUANTITY_VALUE(AngularVelocity, degps, deg / sec)
NEW_QUANTITY_VALUE(AngularVelocity, rps, rot / sec)
NEW_QUANTITY_VALUE(AngularVelocity, rpm, rot / min)

NEW_QUANTITY(LinearAcceleration, mps2, 0, 1, -2, 0, 0)
NEW_QUANTITY_VALUE(LinearAcceleration, cmps2, cm / sec / sec)
NEW_QUANTITY_VALUE(LinearAcceleration, inps2, in / sec / sec)
NEW_QUANTITY_VALUE(LinearAcceleration, miph2, mi / hr / hr)
NEW_QUANTITY_VALUE(LinearAcceleration, kmph2, km / hr / hr)

NEW_QUANTITY(AngularAcceleration, radps2, 0, 0, -2, 0, 1)
NEW_QUANTITY_VALUE(AngularAcceleration, degps2, deg / sec / sec)
NEW_QUANTITY_VALUE(AngularAcceleration, rps2, rot / sec / sec)
NEW_QUANTITY_VALUE(AngularAcceleration, rpm2, rot / min / min)

NEW_QUANTITY(LinearJerk, mps3, 0, 1, -3, 0, 0)
NEW_QUANTITY_VALUE(LinearJerk, cmps3, cm / (sec * sec * sec))
NEW_QUANTITY_VALUE(LinearJerk, inps3, in / (sec * sec * sec))
NEW_QUANTITY_VALUE(LinearJerk, miph3, mi / (hr * hr * hr))
NEW_QUANTITY_VALUE(LinearJerk, kmph3, km / (hr * hr * hr))

NEW_QUANTITY(AngularJerk, radps3, 0, 0, -3, 0, 1)
NEW_QUANTITY_VALUE(AngularJerk, rps3, rot / sec / sec / sec)
NEW_QUANTITY_VALUE(AngularJerk, rpm3, rot / min / min / min)

NEW_QUANTITY(Curvature, radpm, 0, -1, 0, 0, 0);

NEW_QUANTITY(Inertia, kgm2, 1, 2, 0, 0, 0)

NEW_QUANTITY(Force, n, 1, 1, -2, 0, 0)

NEW_QUANTITY(Torque, nm, 1, 2, -2, 0, 0)

NEW_QUANTITY(Power, watt, 1, 2, -3, 0, 0)

NEW_QUANTITY(Current, amp, 0, 0, 0, 1, 0)

NEW_QUANTITY(Charge, coulomb, 0, 0, 1, 1, 0)

NEW_QUANTITY(Voltage, volt, 1, 2, -3, -1, 0)

NEW_QUANTITY(Resistance, ohm, 1, 2, -3, -2, 0)

NEW_QUANTITY(Conductance, siemen, -1, -2, 3, 2, 0)

namespace units {
template <isQuantity Q> constexpr Q abs(const Q& lhs) { return Q(std::abs(lhs.val())); }

template <isQuantity Q> constexpr Q max(const Q& lhs, const Q& rhs) { return (lhs > rhs ? lhs : rhs); }

template <isQuantity Q> constexpr Q min(const Q& lhs, const Q& rhs) { return (lhs < rhs ? lhs : rhs); }

template <int R, isQuantity Q, isQuantity S = QPower<Q, std::ratio<R>>> constexpr S pow(const Q& lhs) {
    return S(std::pow(lhs.val(), R));
}

template <isQuantity Q, isQuantity S = QPower<Q, std::ratio<2>>> constexpr S square(const Q& rhs) {
    return S(std::sqrt(rhs.val()));
}

template <isQuantity Q, isQuantity S = QPower<Q, std::ratio<3>>> constexpr S cube(const Q& rhs) {
    return S(std::sqrt(rhs.val()));
}

template <int R, isQuantity Q, isQuantity S = QRoot<Q, std::ratio<R>>> constexpr S root(const Q& lhs) {
    return S(std::pow(lhs.val(), 1.0 / R));
}

template <isQuantity Q, isQuantity S = QRoot<Q, std::ratio<2>>> constexpr S sqrt(const Q& rhs) {
    return S(std::sqrt(rhs.val()));
}

template <isQuantity Q, isQuantity S = QRoot<Q, std::ratio<3>>> constexpr S cbrt(const Q& rhs) {
    return S(std::sqrt(rhs.val()));
}

template <isQuantity Q> constexpr Q hypot(const Q& lhs, const Q& rhs) { return Q(std::hypot(lhs.val(), rhs.val())); }

template <isQuantity Q> constexpr Q mod(const Q& lhs, const Q& rhs) { return Q(std::fmod(lhs.val(), rhs.val())); }

template <isQuantity Q1, isQuantity Q2> constexpr Q1 copysign(const Q1& lhs, const Q2& rhs) {
    return Q1(std::copysign(lhs.val(), rhs.val()));
}

template <isQuantity Q> constexpr int sgn(const Q& lhs) { return lhs.val() < 0 ? -1 : 1; }

template <isQuantity Q> constexpr bool signbit(const Q& lhs) { return std::signbit(lhs.val()); }

template <isQuantity Q> constexpr Q clamp(const Q& lhs, const Q& lo, Q& hi) {
    return Q(std::clamp(lhs.val(), lo.val(), hi.val()));
}

template <isQuantity Q> constexpr Q ceil(const Q& lhs, const Q& rhs) {
    return Q(std::ceil(lhs.val() / rhs.val()) * rhs.val());
}

template <isQuantity Q> constexpr Q floor(const Q& lhs, const Q& rhs) {
    return Q(std::floor(lhs.val() / rhs.val()) * rhs.val());
}

template <isQuantity Q> constexpr Q trunc(const Q& lhs, const Q& rhs) {
    return Q(std::trunc(lhs.val() / rhs.val()) * rhs.val());
}

template <isQuantity Q> constexpr Q round(const Q& lhs, const Q& rhs) {
    return Q(std::round(lhs.val() / rhs.val()) * rhs.val());
}

constexpr Number sin(const Angle& rhs) { return Number(std::sin(rhs.val())); }

constexpr Number cos(Angle& rhs) { return Number(std::cos(rhs.val())); }

constexpr Number tan(Angle& rhs) { return Number(std::tan(rhs.val())); }

template <isQuantity Q> constexpr Angle asin(const Q& rhs) { return Angle(std::asin(rhs.val())); }

template <isQuantity Q> constexpr Angle acos(const Q& rhs) { return Angle(std::acos(rhs.val())); }

template <isQuantity Q> constexpr Angle atan(const Q& rhs) { return Angle(std::atan(rhs.val())); }

template <isQuantity Q> constexpr Angle atan2(const Q& lhs, const Q& rhs) {
    return Angle(std::atan2(lhs.val(), rhs.val()));
}
} // namespace units