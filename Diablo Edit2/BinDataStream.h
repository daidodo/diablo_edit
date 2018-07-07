#pragma once

#include <vector>

template<typename T>
class Bits
{
	T & v_;
	int b_;
public:
	Bits(T & v, int b) :v_(v), b_(b) {}
	T & value() const { return v_; }
	int bits() const { return b_; }
};

template<typename T>
Bits<T> bits(T & v, int b) { return Bits<T>(v, b); }

class CBinDataStream
{
	std::vector<BYTE> data_;
	DWORD bytes_, bits_;
	bool bad_;
public:
	CBinDataStream()
		: bytes_(0)
		, bits_(0)
		, bad_(false){}
	DWORD BytePos() const{return bytes_;}
	void ReadFile(CFile & cf){
		data_.resize(size_t(cf.GetLength()));
		cf.Read(&data_[0],UINT(cf.GetLength()));
	}
	void WriteFile(CFile & cf){
		ASSERT(data_.size() && _T("CBinDataStream::WriteFile(CFile & cf)"));
		cf.Write(&data_[0],UINT(data_.size()));
		cf.Flush();
	}
	//字节读取
	CBinDataStream & operator >>(DWORD & value) {return readPod(value);}
	CBinDataStream & operator >>(WORD & value){ return readPod(value); }
	CBinDataStream & operator >>(BYTE & value){ return readPod(value); }
	template<typename T, int N>
	CBinDataStream & operator >>(T (&value)[N]){
		for(T & v : value)
			operator >>(v);
		return *this;
	}
	template<int N>
	CBinDataStream & operator >>(BYTE (&value)[N]){
		ASSERT(!bits_ && _T("template<int N>CBinDataStream::operator >>(BYTE (&value)[N])"));
		::CopyMemory(value,&data_[bytes_],N);
		bytes_ += N;
		return *this;
	}
	//位读取
	void AlignByte(){	//按照8位进行对齐，舍弃多余的位
		bytes_ += (bits_ + 7) / 8;
		bits_ = 0;
	}
	CBinDataStream & operator >>(BOOL & b) {return readBits(bits(b, 1));}
	CBinDataStream & operator >>(const Bits<DWORD> & m) { return readBits(m); }
	CBinDataStream & operator >>(const Bits<WORD> & m) { return readBits(m); }
	CBinDataStream & operator >>(const Bits<BYTE> & m) {return readBits(m);}
	void ReadBit(BOOL & value) { operator >>(value); }
	void ReadBits(DWORD & value, int len) { operator >>(bits(value, len)); }
	void ReadBits(WORD & value, int len) { operator>>(bits(value, len)); }
	void ReadBits(BYTE & value, int len) { operator>>(bits(value, len)); }
	//vector<BYTE>
	CBinDataStream & operator >>(std::vector<BYTE> & vec){
		ASSERT(!bits_ && _T("CBinDataStream::operator >>(std::vector<BYTE> & vec)"));
		vec.assign(data_.begin() + bytes_,data_.end());
		return *this;
	}
	//把不能识别的数据转储
	void Restore(std::vector<BYTE> & vec,DWORD from,DWORD to){
		ASSERT(to > from && _T("CBinDataStream::Restore(std::vector<BYTE> & vec,DWORD from,DWORD to)"));
		vec.assign(&data_[from],&data_[to]);
	}
	//随机定位
	void SeekBack(DWORD back){bytes_ -= back;}
	//调试用，得到后续的2进制位流
	CString ToString(DWORD len = 32) const{
		CString ret;
		DWORD byte = bytes_;
		DWORD bit = bits_;
		while(len-- != 0 && byte < data_.size()){
			ret = ((data_[byte] >> bit) & 1 ? _T("1") : _T("0")) + ret;
			if(++bit == 8){
				bit = 0,++byte;
				ret = _T(" ") + ret;
			}
		}
		return ret;
	}
private:
	template<typename T>
	CBinDataStream & readPod(T & value) {
		if (ensure(sizeof value)) {
			value = *reinterpret_cast<T *>(&data_[bytes_]);
			bytes_ += sizeof value;
		}
		return *this;
	}
	template<typename T>
	CBinDataStream & readBits(const Bits<T> & m) {
		const int BITS = sizeof(T) * 8;
		if (ensure(0, m.bits(), BITS)) {
			const T HIGHEST_BIT = T(1) << (BITS - 1);
			T value = 0;
			for (int i = m.bits(); i > 0; --i) {
				value >>= 1;
				if ((data_[bytes_] >> bits_) & 1)
					value += HIGHEST_BIT;
				if (++bits_ == 8)
					bits_ = 0, ++bytes_;
			}
			m.value() = value >> (BITS - m.bits());
		}
		return *this;
	}
	bool ensure(DWORD bytes, DWORD bits = 0, DWORD maxBits = 0) {
		bad_ = (bad_
			|| !(bits > 0 ? bits <= maxBits : bits_ == 0)
			|| !(bytes_ + (bits_ + bits + 7) / 8 <= data_.size()));
		return !bad_;
	}
};
