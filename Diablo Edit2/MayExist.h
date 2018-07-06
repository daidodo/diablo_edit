#pragma once

template<class T>
struct CMayExist
{
//members
	CMayExist(void):p_(0){}
	~CMayExist(void){
		if(p_)
			delete p_;
	}
	T * operator ->(){return (p_ ? p_ : p_ = new T);}
	const T * operator ->() const{return p_;}
	T & operator *(){return (p_ ? *p_: *(p_ = new T));}
	T Value() const{return *p_;}
    bool operator !() const{return !p_;}
//	void Assert() const{ASSERT(p_);}
private:
	CMayExist(const CMayExist &);
	CMayExist & operator =(const CMayExist &);
//fields
	T * p_;
};

template<class T,int N>
struct CMayExistArray
{
//members
	CMayExistArray(void):p_(0){}
	~CMayExistArray(void){
		if(p_)
			delete [] p_;
	}
	T & operator [](int i){return (p_ ? p_[i] : (p_ = new T[N])[i]);}
	const T & operator [](int i) const{return p_[i];}
	const T * Value() const{return p_;}
private:
	CMayExistArray(const CMayExistArray &);
	CMayExistArray & operator =(const CMayExistArray &);
//fields
	T * p_;
};
