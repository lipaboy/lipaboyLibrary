#ifndef FIXED_PRECISION_NUMBER_H
#define FIXED_PRECISION_NUMBER_H

#include "common_interfaces/comparable.h"
#include "common_interfaces/either_comparable.h"
#include "common_interfaces/algebra.h"
#include "extra_tools/maths_tools.h"

#include <typeinfo>

namespace lipaboy_lib {
	
	// TODO: overload operator<< for ostream

	template <typename T, typename IntegerPrecisionType, 
		IntegerPrecisionType fraction, IntegerPrecisionType dozenPower>
	class FixedPrecisionNumberBase : 
        public EitherComparable<T, FixedPrecisionNumberBase<T, IntegerPrecisionType, fraction, dozenPower> >,
        public ComparatorExtender<FixedPrecisionNumberBase<T, IntegerPrecisionType, fraction, dozenPower> >,
		public OperationAlgebra<T, FixedPrecisionNumberBase<T, IntegerPrecisionType, fraction, dozenPower> >,
        public SelfOperationAlgebra<T, FixedPrecisionNumberBase<T, IntegerPrecisionType, fraction, dozenPower> >
	{
    public:
        using ValueType = std::remove_reference_t<T>;
	public:
		explicit
			FixedPrecisionNumberBase(T _number = T()) noexcept
				: number(_number) {}

        bool operator<(const T& val) const noexcept { return (getNumber() < val
			- static_cast<T>(epsilonPrecision())); }
//        bool operator<=(const T& val) const noexcept { return (getNumber() <= val
//			+ static_cast<T>(epsilonPrecision())); }
        bool operator==(const ValueType& val) const noexcept {
            return (getNumber() >= val - static_cast<ValueType>(epsilonPrecision()))
                && (getNumber() <= val + static_cast<ValueType>(epsilonPrecision()));
		}

        bool operator<(const FixedPrecisionNumberBase& obj) const noexcept {
            return (*this) < obj.getNumber(); }
        bool operator==(const FixedPrecisionNumberBase& obj) const noexcept {
            return (*this) == obj.getNumber(); }

		void setNumber(T const & val) noexcept { number = val; }
        T const & getNumber() const noexcept { return number; }
		FixedPrecisionNumberBase const & operator= (T const & val) noexcept { setNumber(val); return *this; }

		static constexpr ValueType epsilon() {
			return static_cast<ValueType>(fraction * powDozen<ValueType, IntegerPrecisionType>(dozenPower));
		}

		//friend std::ostream& operator<< (std::ostream& o, FixedPrecisionNumber const & number);

	public:
		constexpr ValueType epsilonPrecision() const { return fraction * powDozen<T, IntegerPrecisionType>(dozenPower); }
	private:
		T number;
	};


	// Number of fixed integer precision
	template <typename T, int fraction, int dozenPower>
	using FixedPrecisionNumber = FixedPrecisionNumberBase<T, int, fraction, dozenPower>;

	template <typename T, typename IntegerPrecisionType,
		IntegerPrecisionType fraction, IntegerPrecisionType dozenPower>
	inline std::ostream& operator<< (std::ostream& o, 
		FixedPrecisionNumberBase<T, IntegerPrecisionType, fraction, dozenPower> const & number) {
		return o << number.getNumber();
	}

}

#endif //FIXED_PRECISION_NUMBER_H
