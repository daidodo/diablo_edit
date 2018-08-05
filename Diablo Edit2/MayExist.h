#pragma once

#include <vector>
#include <cassert>

#include "BinDataStream.h"

template<class T, int N = 1>
class MayExist;

template<class T, int N>
class MayExist : std::vector<T>
{
	static_assert(N > 0);
	typedef std::vector<T> __Base;
public:
	std::vector<T> & ensure() { resize(N); return *this; }
	bool exist() const { return !__Base::empty(); }
	operator const T *() const { return &operator [](0); }
	using __Base::begin;
	using __Base::end;
	using __Base::size;
	using __Base::operator [];
};

template<class T>
class MayExist<T, 1>
{
	std::vector<T> v_;
public:
	T & ensure() { v_.resize(1); return v_.front(); }
	bool exist() const { return !v_.empty(); }
	//void operator =(const T & v) { v_.front() = v; }
	const T * operator ->() const { return &operator *(); }
	T * operator ->() { return &operator *(); }
	const T & operator *() const { return v_.front(); }
	T & operator *() { return v_.front(); }
};

template<class T>
CInBitsStream & operator >>(CInBitsStream & bs, MayExist<T, 1> & v) {
	return bs >> v.ensure();
}

template<class T>
COutBitsStream & operator <<(COutBitsStream & bs, const MayExist<T, 1> & v) {
	return bs << *v;
}

#define MAY_EXIST_POD(T)								\
	template<>											\
	class MayExist<T, 1> {								\
		T v_ = 0;										\
		bool e_ = false;								\
	public:												\
		T & ensure() { e_ = true; return v_; }			\
		bool exist() const { return e_; }				\
		T operator =(T v) { assert(e_); v_ = v; }		\
		MayExist & operator =(const MayExist & a) {		\
			v_ = a.v_; e_ = a.e_; return *this;			\
		}												\
		operator T() const { assert(e_); return v_; }	\
	};													\
inline CInBitsStream & operator >>(CInBitsStream & bs, const Bits<MayExist<T, 1>> & m) {			\
	return bs >> bits(m.value().ensure(), m.bits());												\
}																									\
inline COutBitsStream & operator <<(COutBitsStream & bs, const Bits<const MayExist<T, 1>> & m) {	\
	return bs << bits<T>(m.value(), m.bits());														\
}

MAY_EXIST_POD(BYTE);
MAY_EXIST_POD(WORD);

#undef MAY_EXIST_POD
