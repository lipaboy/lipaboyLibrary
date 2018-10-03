#pragma once

#include "../common_interfaces/containable.h"
#include "../common_interfaces/either_comparable.h"

#include <functional>

namespace lipaboy_lib {

	//TODO: write comparison with Infinity (Infinity == Infinity)

    template <class T>
    class Infinity : public EitherComparable<T, Infinity<T> > {};

    template <class T>
    class PositiveInfinity : public Infinity<T> {
    public:
        bool operator< (const T& number) const { return false; }
        bool operator<= (const T& number) const { return false; }
        bool operator== (const T& number) const { return false; }

	public:
		struct less {
			constexpr bool operator()(T const &, PositiveInfinity const &) const {
				return true;
			}
			constexpr bool operator()(PositiveInfinity const &, T const &) const {
				return false;
			}
		};
    };

    template <class T>
    class NegativeInfinity : public Infinity<T> {
    public:
        bool operator< (const T& number) const { return true; }
        bool operator<= (const T& number) const { return true; }
        bool operator== (const T& number) const { return true; }

	public:
		struct less {
			constexpr bool operator()(T const &, NegativeInfinity const &) const {
				return false;
			}
			constexpr bool operator()(NegativeInfinity const &, T const &) const {
				return true;
			}
		};
    };
	
	//TODO: Add debug function to output result ( if contains then "c in [a, b]" or "c out [a, b]" )
	//TODO: add output to ostream

	template <class T,
		class LeftComparator, 
		class RightComparator,
		class TLeftBorder = T,
		class TRightBorder = T>
    class IntervalBase : 
		public ContainAPIExtender<T, 
			IntervalBase<T, LeftComparator, RightComparator, TLeftBorder, TRightBorder> 
		>
	{
	public:
		IntervalBase(TLeftBorder const & leftBorder, TRightBorder const & rightBorder)
			: leftBorder_(leftBorder), rightBorder_(rightBorder) {}
		bool in(const T& element) const { return contains(element); }
		bool out(const T& element) const { return !in(element); }
		//TODO: think about advantages of these methods. If you include them then will override these ones into PositiveRay
		bool outLeft(const T& element) const { return !isLeftCompare(element); }
		bool outRight(const T& element) const { return !isLeftCompare(element); }

		bool contains(const T& element) const {
			return isLeftCompare(element) && isRightCompare(element);
		}

		TLeftBorder const & left() const { return leftBorder_; }
		TRightBorder const & right() const { return rightBorder_; }

		bool operator==(const IntervalBase& other) const {
			return (leftBorder_ == other.leftBorder)
				&& (rightBorder_ == other.rightBorder);
		}
		bool operator!=(const IntervalBase& other) const { return !((*this) == other); }

		//You can't write method length() because your class Interval will become less general
		//Solution: but you can inherit from Interval class NumericInterval where add this method (and maybe Infinity)

	protected:
		bool isLeftCompare(const T& element) const { return LeftComparator()(leftBorder_, element); }
		bool isRightCompare(const T& element) const { return RightComparator()(element, rightBorder_); }
	protected:
		TLeftBorder leftBorder_;
		TRightBorder rightBorder_;
	};

	template <class T, typename LeftComparator, typename RightComparator>
	class Interval : public IntervalBase<T, LeftComparator, RightComparator>
	{
	public:
		Interval(const T& leftBorder, const T& rightBorder)
			: IntervalBase<T, LeftComparator, RightComparator>(leftBorder, rightBorder) {}

		T& rLeft() { return leftBorder_; }
		T& rRight() { return rightBorder_; }
	};

	template <class T>
	using OpenInterval = Interval<T, std::less<T>, std::less<T> >;

	template <class T>
	using CloseInterval = Interval<T, std::less_equal<T>, std::less_equal<T> >;

	
	template <class T, typename LeftComparator>
	class PositiveRay : 
		public IntervalBase<T, LeftComparator, typename PositiveInfinity<T>::less, T, PositiveInfinity<T> >
	{
	public:
		PositiveRay(const T& leftBorder)
			: IntervalBase(leftBorder, PositiveInfinity<T>()) {}

		T& rLeft() { return leftBorder_; }
	};

	template <class T, typename RightComparator>
	class NegativeRay :
		public IntervalBase<T, typename NegativeInfinity<T>::less, RightComparator, NegativeInfinity<T>, T >
	{
	public:
		NegativeRay(const T& rightBorder)
			: IntervalBase(NegativeInfinity<T>(), rightBorder) {}

		T& rRight() { return rightBorder_; }
	};

	template <class T>
	class UniversumInterval :
		public IntervalBase<
			T, 
			typename NegativeInfinity<T>::less, 
			typename PositiveInfinity<T>::less, 
			NegativeInfinity<T>, 
			PositiveInfinity<T>
		>
	{
	public:
		UniversumInterval() : IntervalBase(NegativeInfinity<T>(), PositiveInfinity<T>()) {}
	};

	//-----------------------------------------//

	

}


