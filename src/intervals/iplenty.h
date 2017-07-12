#ifndef IPLENTY_H
#define IPLENTY_H

template <class T>
class IPlenty {
public:
	virtual bool contains(const T& element) const = 0;
};

#endif //IPLENTY_H
