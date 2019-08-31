#ifndef __ListColection_h
#define __ListColection_h

#include <list>
#include <iterator> 

template <class T>
class ListColection
{
public:
	ListColection();

	void add(T obj);

	void remove(T obj);

	auto beginIterador();

	auto endIterador();

	int qtdColection();

private:
	std::list<T> colection;
};

template <class T>
inline auto
ListColection<T>::beginIterador()
{
	return colection.begin();
}

template <class T>
inline auto
ListColection<T>::endIterador() {
	return colection.end();
}

template <class T>
ListColection<T>::ListColection() {
}

template <class T>
void
ListColection<T>::add(T obj)
{
	colection.push_back(obj);
}

template <class T>
void
ListColection<T>::remove(T obj)
{
	colection.remove(obj);
}

template <class T>
int
ListColection<T>::qtdColection() {
	return colection.size;
}

#endif // __ListColection_h