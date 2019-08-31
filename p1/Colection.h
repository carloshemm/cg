#ifndef __Colection_h
#define __Colection_h

#include <list>
#include <vector>
#include <iterator> 

template <class T>
class Colection
{
public:
	Colection() {}

	virtual void add(T obj) {}

	virtual void remove(T obj) {}

	virtual T* beginIterador() {}

	virtual T* endIterador() {}

	virtual int qtdColection() {}

private:
	
};

#endif // __Colection_h