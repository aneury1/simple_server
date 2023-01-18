#ifndef HOF_H_DEFINED
#define HOF_H_DEFINED



template<class T>
class AStream
{
	T transformedValue;
public:

	AStream(T v) {
		transformedValue = v;
	}

	AStream(const AStream& stream) {
		this->transformedValue = stream.transformedValue;
	}


	AStream& operator =(const AStream& ref) {
		return this;
	}

	T operator ()() {
		return transformedValue;
	}

 

};


#endif