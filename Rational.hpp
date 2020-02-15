#pragma once

//from ‚é‚Üƒ‰ƒCƒuƒ‰ƒŠ
//https://lumakernel.github.io/ecasdqina/misc/Rational

#include <iostream>
#include <utility>
#include <exception>
template <class Integer = long long>
struct Rational {
	static Integer gcd(Integer a, Integer b) {
		if (a < 0) a = -a;
		if (b < 0) b = -b;
		Integer res = 1;
		while (a != b) {
			if (a < b) std::swap(a, b);
			if (b == 0) break;
			if (!(a % 2) && !(b % 2))
				res *= 2, a /= 2, b /= 2;
			else if (!(a % 2))
				a /= 2;
			else if (!(b % 2))
				b /= 2;
			else
				a = (a - b) / 2;
		}
		return res * a;
	}
	Rational(Integer numer = 0) : numer(numer), denom(1) {}
	Rational(Integer numer, Integer denom) : numer(numer), denom(denom) { reduce(); }
	Rational(String str) {
		auto list = str.split(U'/');
		if (list.size() != 2) {
			throw std::runtime_error("Parse error in class \"Rational\".");
		}
		auto parsed = list.map([](auto&& s) {return ParseIntOpt<int>(s); });
		if (!parsed[0] || !parsed[1] || parsed[1] == 0) {
			throw std::runtime_error("Parse error in class \"Rational\".");
		}
		numer = *parsed[0];
		denom = *parsed[1];
		reduce();
	}
private:
	Integer numer, denom;
	void reduce() {
		if (denom == 0) {
			throw std::domain_error("Division by zero in class \"Rational\".");
		}
		Integer g = gcd(numer, denom);
		numer /= g, denom /= g;
		if (denom < 0) numer = -numer, denom = -denom;
	}
	Rational(Integer numer, Integer denom, int) : numer(numer), denom(denom) {}

public:
	template < class T >
	explicit operator T() {
		return T(T(numer) / T(denom));
	}
	Rational operator+() const { return *this; }
	Rational operator-() const { return Rational(-numer, denom, 0); }
	// Rational <arithmetic operator> Rational {{{
	Rational operator+(const Rational& a) const {
		return Rational(a.numer * denom + numer * a.denom, denom * a.denom);
	}
	Rational operator-(const Rational& a) const { return *this + -a; }
	Rational operator*(const Rational& a) const {
		return Rational(numer * a.numer, denom * a.denom);
	}
	Rational operator/(const Rational& a) const {
		return Rational(numer * a.denom, denom * a.numer);
	}
	Rational& operator+=(const Rational& a) {
		*this = *this + a;
		return *this;
	}
	Rational& operator-=(const Rational& a) {
		*this = *this - a;
		return *this;
	}
	Rational& operator*=(const Rational& a) {
		*this = *this * a;
		return *this;
	}
	Rational& operator/=(const Rational& a) {
		*this = *this / a;
		return *this;
	}
	// }}}
	// Rational <arithmetic operator> Integer {{{
	Rational operator+(Integer a) const { return *this + Rational(a); }
	Rational operator-(Integer a) const { return *this + Rational(-a); }
	Rational operator*(Integer a) const { return *this * Rational(a); }
	Rational operator/(Integer a) const { return *this * Rational(1, a, 0); }
	Rational& operator+=(Integer a) {
		*this = *this + a;
		return *this;
	}
	Rational& operator-=(Integer a) {
		*this = *this - a;
		return *this;
	}
	Rational& operator*=(Integer a) {
		*this = *this * a;
		return *this;
	}
	Rational& operator/=(Integer a) {
		*this = *this / a;
		return *this;
	}
	// }}}
	Rational inverse() const {
		assert(numer != 0);
		return Rational(denom, numer);
	}
	/// Rational <comparison operator> Rational {{{
	bool operator==(const Rational& a) const {
		return numer == a.numer && denom == a.denom;
	}
	bool operator<(const Rational& a) const { return numer * a.denom < a.numer * denom; }
	bool operator!=(const Rational& a) const { return std::rel_ops::operator!=(*this, a); }
	bool operator>(const Rational& a) const { return std::rel_ops::operator>(*this, a); }
	bool operator<=(const Rational& a) const { return std::rel_ops::operator<=(*this, a); }
	bool operator>=(const Rational& a) const { return std::rel_ops::operator>=(*this, a); }
	// }}}
	/// Rational <comparison operator> Integer {{{
	bool operator==(Integer a) const { return *this == Rational(a); }
	bool operator<(Integer a) const { return *this < Rational(a); }
	bool operator!=(Integer a) const { return *this != Rational(a); }
	bool operator>(Integer a) const { return *this > Rational(a); }
	bool operator<=(Integer a) const { return *this <= Rational(a); }
	bool operator>=(Integer a) const { return *this >= Rational(a); }
	// }}}
	/*
	friend ostream& operator<<(ostream& os, const Rational& a) {
		os << a.numer << "/" << a.denom;
		return os;
	}
	*/
	String parse() const {
		return Format(numer, U"/", denom);
	}

	Integer bunsi() const { return numer; }
	Integer bunbo() const { return denom; }

	//mod 1
	Rational loop() const {
		return Rational((numer % denom + denom) % denom, denom);
	}

	//gauss([x])
	Integer gauss() const {
		return (*this - loop()).numer;
	}

	//FŒ^‚Å‚Ì–ß‚è’l
	template<class F = double> F real() const {
		return F(numer) / F(denom);
	}

	//abs
	Rational abs() {
		return Rational(std::abs(numer), denom);
	}

	bool operator<(const double& a) const { return real() < a; }
	bool operator!=(const double& a) const { return std::rel_ops::operator!=(*this, a); }
	bool operator>(const double& a) const { return std::rel_ops::operator>(*this, a); }
	bool operator<=(const double& a) const { return std::rel_ops::operator<=(*this, a); }
	bool operator>=(const double& a) const { return std::rel_ops::operator>=(*this, a); }
};

using Quot = Rational<>;
