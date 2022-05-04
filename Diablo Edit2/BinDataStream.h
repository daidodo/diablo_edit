#pragma once

#include <vector>
#include <cstring>
#include <cassert>
#include <string>

// bits
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

template<typename T>
Bits<const T> bits(const T & v, int b) { return Bits<const T>(v, b); }

// offset_value
template<typename T>
class OffsetValue
{
	const T & v_;
	DWORD off_;
public:
	OffsetValue(const T & v, DWORD off) :v_(v), off_(off) {}
	T value() const { return v_; }
	DWORD offset() const { return off_; }
};

template<typename T>
OffsetValue<T> offset_value(DWORD offset, const T & value) {
	return OffsetValue<T>(value, offset);
}

//��from����len�������ݵ�to���ֱ�����from��ǰfromOff���أ���to��ǰtoOff���ء�
void CopyBits(const BYTE * from, BYTE * to, DWORD fromOff, DWORD toOff, DWORD len);

class CInBitsStream
{
	std::vector<BYTE> data_;
	DWORD bytes_, bits_;
	bool bad_;
public:
	CInBitsStream()
		: bytes_(0)
		, bits_(0)
		, bad_(false){}
	CInBitsStream(const BYTE * data, size_t sz)
		: bytes_(0)
		, bits_(0)
		, bad_(false) {
		assert(data);
		data_.assign(data, data + sz);
	}
	DWORD DataSize() const { return data_.size(); }
	DWORD BytePos() const{return bytes_;}
	bool Good() const { return !bad_; }
	std::vector<BYTE> & Data() { return data_; }	//���ؿ��޸ĵ����ݣ���Ҫ����CRCУ��
	void ReadFile(CFile & cf) {
		data_.resize(size_t(cf.GetLength()));
		if (!data_.empty())
			cf.Read(&data_[0], UINT(cf.GetLength()));
	}
	//�����λ
	void SeekBack(DWORD back) {
		if (ensurePos(bytes_ - back))
			bytes_ -= back;
	}
	//������������ֱ���ҵ�pattern����λ��pattern�����ַ�λ��
	void SkipUntil(const char * pattern) {
		assert(pattern);
		if (ensure(0)) {
			const auto src = reinterpret_cast<const char *>(&data_[bytes_]);
			const auto wh = std::strstr(src, pattern);
			bytes_ = (wh ? bytes_ + (wh - src) : data_.size());
		}
	}
	//�ֽڶ�ȡ
	CInBitsStream & operator >>(DWORD & value) {return readPod(value);}
	CInBitsStream & operator >>(WORD & value){ return readPod(value); }
	CInBitsStream & operator >>(BYTE & value){ return readPod(value); }
	template<typename T, int N>
	CInBitsStream & operator >>(T (&value)[N]){
		for(T & v : value)
			*this >> v;
		return *this;
	}
	template<int N>
	CInBitsStream & operator >>(BYTE (&value)[N]){
		if (ensure(N)) {
			::CopyMemory(value, &data_[bytes_], N);
			bytes_ += N;
		}
		return *this;
	}
	//λ��ȡ
	void AlignByte(){	//����8λ���ж��룬���������λ
		bytes_ += (bits_ + 7) / 8;
		bits_ = 0;
	}
	CInBitsStream & operator >>(BOOL & b) {return readBits(bits(b, 1));}
	CInBitsStream & operator >>(const Bits<DWORD> & m) { return readBits(m); }
	CInBitsStream & operator >>(const Bits<WORD> & m) { return readBits(m); }
	CInBitsStream & operator >>(const Bits<BYTE> & m) { return readBits(m); }
	//vector<BYTE>
	CInBitsStream & operator >>(std::vector<BYTE> & vec){
		if (ensure(0)) {
			vec.assign(data_.begin() + bytes_, data_.end());
			bytes_ = data_.size();
		}
		return *this;
	}
	//�Ѳ���ʶ�������ת��
	void Restore(std::vector<BYTE> & vec,DWORD from,DWORD to){
		ASSERT(to >= from);
		if(ensurePos(from) && ensurePos(to))
			vec.assign(data_.begin() + from, data_.begin() + to);
	}
	//�����ã��õ�������2����λ��
	std::string ToString(DWORD len = 32) const;
private:
	template<typename T>
	CInBitsStream & readPod(T & value) {
		if (ensure(sizeof value)) {
			value = *reinterpret_cast<T *>(&data_[bytes_]);
			bytes_ += sizeof value;
		}
		return *this;
	}
	template<typename T>
	CInBitsStream & readBits(const Bits<T> & m) {
		if (ensure(0, m.bits(), sizeof(T) * 8)) {
			m.value() = 0;
			BYTE * to = reinterpret_cast<BYTE *>(&m.value());
			::CopyBits(&data_[bytes_], to, bits_, 0, m.bits());
			bytes_ += (bits_ + m.bits()) / 8;
			bits_ = (bits_ + m.bits()) % 8;
		}
		return *this;
	}
	bool ensure(DWORD bytes, DWORD bits = 0, DWORD maxBits = 0) {
		bad_ = (bad_
			|| !(bits > 0 ? bits <= maxBits : bits_ == 0)
			|| !(bytes_ + bytes + (bits_ + bits + 7) / 8 <= data_.size()));
		assert(!bad_);
		return !bad_;
	}
	bool ensurePos(DWORD pos) {
		bad_ = (bad_ || pos > data_.size());
		assert(!bad_);
		return !bad_;
	}
};

class COutBitsStream
{
	std::vector<BYTE> data_;
	DWORD bytes_, bits_;
	bool bad_;
public:
	COutBitsStream() :bytes_(0), bits_(0), bad_(false) {}
	DWORD BytePos() const { return bytes_; }
	bool Good() const { return !bad_; }
	const std::vector<BYTE> & Data() {
		ensure(0);
		data_.resize(bytes_);
		return data_;
	}
	void WriteFile(CFile & cf) {
		if (bytes_ > 0) {
			data_.resize(bytes_);
			cf.Write(&data_[0], UINT(data_.size()));
			cf.Flush();
		}
	}
	//�ֽ�д��
	COutBitsStream & operator <<(DWORD value) { return writePod(value); }
	COutBitsStream & operator <<(WORD value) { return writePod(value); }
	COutBitsStream & operator <<(BYTE value) { return writePod(value); }
	template<typename T, int N>
	COutBitsStream & operator <<(const T (&value)[N]) {
		for (auto & v : value)
			*this << v;
		return *this;
	}
	template<int N>
	COutBitsStream & operator <<(BYTE (&value)[N]) {
		ensure(N);
		::CopyMemory(&data_[bytes_], value, N);
		bytes_ += N;
		return *this;
	}
	template<typename T>
	COutBitsStream & operator <<(const OffsetValue<T> & m) {
		if (ensurePos(m.offset())) {
			const auto save = bytes_;
			bytes_ = m.offset();
			operator <<(m.value());
			bytes_ = save;
		}
		return *this;
	}
	//λд��
	void AlignByte() {	//����8λ���ж��룬���������λ
		if (Good()) {
			bytes_ += (bits_ + 7) / 8;
			bits_ = 0;
		}
	}
	COutBitsStream & operator <<(BOOL b) { return writeBits(bits(b, 1)); }
	COutBitsStream & operator <<(const Bits<const DWORD> & m) { return writeBits(m); }
	COutBitsStream & operator <<(const Bits<DWORD> & m) { return writeBits(m); }
	COutBitsStream & operator <<(const Bits<const WORD> & m) { return writeBits(m); }
	COutBitsStream & operator <<(const Bits<WORD> & m) { return writeBits(m); }
	COutBitsStream & operator <<(const Bits<const BYTE> & m) { return writeBits(m); }
	COutBitsStream & operator <<(const Bits<BYTE> & m) { return writeBits(m); }
	//vector<BYTE>
	COutBitsStream & operator <<(const std::vector<BYTE> & data) {
		if (ensure(data.size())) {
			::CopyMemory(&data_[bytes_], &data[0], data.size());
			bytes_ += data.size();
		}
		return *this;
	}
private:
	template<typename T>
	COutBitsStream & writePod(T v) {
		if (ensure(sizeof v)) {
			*reinterpret_cast<T *>(&data_[bytes_]) = v;
			bytes_ += sizeof v;
		}
		return *this;
	}
	template<typename T>
	COutBitsStream & writeBits(const Bits<T> & m) {
		if (ensure(0, m.bits(), sizeof(T) * 8)) {
			const BYTE * from = reinterpret_cast<const BYTE *>(&m.value());
			CopyBits(from, &data_[bytes_], 0, bits_, m.bits());
			bytes_ += (bits_ + m.bits()) / 8;
			bits_ = (bits_ + m.bits()) % 8;
		}
		return *this;
	}
	bool ensure(DWORD bytes, DWORD bits = 0, DWORD maxBits = 0) {
		bad_ = (bad_ || !(bits > 0 ? bits <= maxBits : bits_ == 0));
		if (!bad_) {
			const DWORD old = data_.size(), need = bytes + (bits_ + bits + 7) / 8;
			if (bytes_ + need > old)
				data_.resize(old + (old >> 1) + need);
		}
		ASSERT(Good());
		return !bad_;
	}
	bool ensurePos(DWORD pos) {
		bad_ = (bad_ || pos > data_.size());
		ASSERT(Good());
		return !bad_;
	}
};