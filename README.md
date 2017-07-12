# lipaboyLibrary
some useful functions for work (C++)

1) I don't know how distribute my functions (intervalFuncs and maths) between namespaces. For example, LipaboyLib::Maths::Point2D or LipaboyMaths::Point2d

2) May be can create such class that would get parameters like lambda where you can give such labmda: [] (int x, int y) { return x < y; } and would inherit from that class.
    + (for my realization) that everything is undestandly and laconically
    + realize the common interface IPlenty

3) Read about Args&& (because you need something like that: "Interval<int>.inAnd(a, b, c, d)" instead of "Interval<int>.in(a) && Interval<int>.in(b) && Interval<int>.in(c) && Interval<int>.in(d)" )

4)  Add new two parameters to Interval (RightComparison and LeftComparison). They intuitevly clear and undestandly but not obviously. May be need some instructions or help for it.
