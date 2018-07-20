#pragma once

#include <vector>
#include <cassert>

template<class T, int N = 1>
class MayExist;

template<class T, int N>
class MayExist : protected std::vector<T>
{
};

template<class T>
class MayExist<T, 1>
{
	std::vector<T> v_;
public:
	T & ensure() { v_.resize(1); return v_.front(); }
	bool exist() const { return !v_.empty(); }
	T * operator ->() { return &operator *(); }
	const T * operator ->() const { return &operator *(); }
	T & operator *() { return v_.front(); }
	const T & operator *() const { return v_.front(); }
};

#define MAY_EXIST_POD(T)								\
	template<>											\
	class MayExist<T, 1> {								\
		T v_ = 0;										\
		bool e_ = false;								\
	public:												\
		T & ensure() { e_ = true; return v_; }			\
		bool exist() const { return e_; }				\
		T operator =(T v) { v_ = v; e_ = true; }		\
		MayExist & operator =(const MayExist & a) {		\
			v_ = a.v_; e_ = a.e_; return *this;			\
		}												\
		operator T() const { assert(e_); return v_; }	\
	}

MAY_EXIST_POD(BYTE);
MAY_EXIST_POD(WORD);

#undef MAY_EXIST_POD

//template<class T, int N>
//class __MayExistBase : protected std::vector<T> {
//	typedef std::vector<T> __Base;
//	friend class CInBitsStream;
//	friend class COutBitsStream;
//	void ensure() {
//		static_assert(N > 0);
//		if (empty())
//			resize(N);
//	}
//public:
//	using __Base::empty;
//};
//
//template<class T>
//struct MayExist<T, 1> : public __MayExistBase<T, 1>
//{
//	operator T &() { return front(); }
//	operator const T &() const { return front(); }
//};


template<class T>
struct CMayExist
{
	//members
	CMayExist(void) :p_(0) {}
	~CMayExist(void) { delete p_; }
	T * operator ->() { return (p_ ? p_ : p_ = new T); }
	const T * operator ->() const { return p_; }
	T & operator *() { return *operator ->(); }
	const T & operator *() const { return Value(); }
	const T & Value() const { return *p_; }
	bool operator !() const { return !p_; }
	bool IsValid() const { return p_; }
private:
	CMayExist(const CMayExist &) = delete;
	CMayExist & operator =(const CMayExist &) = delete;
	//fields
	T * p_;
};

template<class T, int N>
struct CMayExistArray
{
	//members
	CMayExistArray(void) :p_(0) {}
	~CMayExistArray(void) { delete[] p_; }
	T & operator [](int i) { return (p_ ? p_[i] : (p_ = new T[N])[i]); }
	const T & operator [](int i) const { return p_[i]; }
	const T * Value() const { return p_; }
	int Size() const { return N; }
private:
	CMayExistArray(const CMayExistArray &) = delete;
	CMayExistArray & operator =(const CMayExistArray &) = delete;
	//fields
	T * p_;
};
