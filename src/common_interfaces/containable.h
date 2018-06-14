#ifndef IPLENTY_H
#define IPLENTY_H

template <class T, class TDerived>
class ContainAPIExtender {
public:
    bool contains(const T& element) const {
        return static_cast<TDerived const *>(this)->contains(element);
    }

    bool containsAll(T const & element) { return contains(element); }
	template <class... Args>
    bool containsAll(T const & element, Args... args) { return contains(element) && containsAll(args...); }

    bool containsAny(T const & element) { return contains(element); }
	template <class... Args>
    bool containsAny(T const & element, Args... args) { return contains(element) || containsAny(args...); }

    template <class... Args>
    bool containsNone(T const & element, Args... args) { return !containsAny(element, args...); }
};

#endif //IPLENTY_H
