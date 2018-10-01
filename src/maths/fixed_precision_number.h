#ifndef FIXED_PRECISION_NUMBER_H
#define FIXED_PRECISION_NUMBER_H

#include "common_interfaces/comparable.h"
#include "common_interfaces/either_comparable.h"
#include "common_interfaces/algebra.h"
#include "extra_tools/maths_tools.h"

#include <typeinfo>

namespace lipaboy_lib {

	template <typename T, typename IntegerPrecisionType, 
		IntegerPrecisionType fraction, IntegerPrecisionType dozenPower>
	class FixedPrecisionNumberBase : 
        public ComparatorExtender<FixedPrecisionNumberBase<T, IntegerPrecisionType, fraction, dozenPower> >,
        public EitherComparable<T, FixedPrecisionNumberBase<T, IntegerPrecisionType, fraction, dozenPower> >,
        public Algebra<T, FixedPrecisionNumberBase<T, IntegerPrecisionType, fraction, dozenPower> >,
		public NumberSettable<T>
	{
    public:
        using ValueType = std::remove_reference_t<T>;
	public:
		explicit
			FixedPrecisionNumberBase(T _number = T()) noexcept
				: number(_number) {}

        bool operator<(const T& val) const noexcept { return (getNumber() < val - epsilon()); }
        bool operator<=(const T& val) const noexcept { return (getNumber() <= val
			+ static_cast<T>(fraction * powDozen<T, IntegerPrecisionType>(dozenPower))); }
        bool operator==(const ValueType& val) const noexcept {
            return (getNumber() >= val - epsilon())
					&& (getNumber() <= val + epsilon());
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

	private:
		T number;
	};


	template <typename T, int fraction, int dozenPower>
	using FixedPrecisionNumber = FixedPrecisionNumberBase<T, int, fraction, dozenPower>;

}

#endif //FIXED_PRECISION_NUMBER_H
