#ifndef __VectorColection_h
#define __VectorColection_h

#include <vector>
#include <iterator> 

template <class T>
class VectorColection
{
public:
	VectorColection();

	void add(T obj);

	void remove(T obj);

	auto beginIterador();

	auto endIterador();

	int qtdColection();

private:
	std::vector<T> colection;
};

template <class T>
inline auto
VectorColection<T>::beginIterador()
{
	return colection.begin();
}

template <class T>
inline auto
VectorColection<T>::endIterador() {
	return colection.end();
}

template <class T>
VectorColection<T>::VectorColection() {
}

template <class T>
void
VectorColection<T>::add(T obj)
{
	colection.push_back(obj);
}

template <class T>
void
VectorColection<T>::remove(T obj)
{
	auto begin = colection.begin();
	auto end = colection.end();
	for (auto it = begin; it != end; it++)
	{
		if (*it == obj) {
			colection.erase(it);
			break;
		}
	}
}

template <class T>
int
VectorColection<T>::qtdColection() {
	return colection.size;
}

#endif // __VectorColection_h