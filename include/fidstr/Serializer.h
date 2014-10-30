#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

#include <algorithm>

// NOTE Code from localizer library by Humphrey Hu

namespace fidstr {
	
	/*! \class Serializer Serializer.h
	* \brief conveniently writes to a packed byte stream. It automatically
	* increments an internal counter to keep appending. */
	class Serializer {
	private:
		char *dst;
		size_t counter;

	public:
		/*! \brief Create a serializer that writes to the specified buffer. */
		Serializer(void *target);
		~Serializer();

		/*! \brief Write an instance of T to the buffer. */
		template <class T>
		void Write(const T *src);

		/*! \brief Write n instances of T to the buffer from an array. */
		template <class T>
		void Write(const T *src, int n);
	};

	template <class T>
	void Serializer::Write(const T* src) {
		const char* source = reinterpret_cast<const char*>(src);
		std::copy(source, source + sizeof(T), dst + counter);
		counter += sizeof(T);
	}

	template <class T>
	void Serializer::Write(const T *src, int n) {
		const char* source = reinterpret_cast<const char*>(src);
		std::copy(source, source + n*sizeof(T), dst + counter);
		counter += n*sizeof(T);
	}

	/*! \class Deserializer Serializer.h
	* \brief reads from a serialized byte stream. It automatically increments a
	* counter to keep reading from after the last read position. */
	class Deserializer {
		void *src;
		size_t counter;

	public:
		/*! \brief Create a deserializer from the buffer. */
		Deserializer(void *src);
		~Deserializer();

		/*! \brief Read an instance of T from the buffer. */
		template <class T>
		T Read();

		/*! \brief Read n instances of T from the buffer to an array. */
		template<class T>
		void Read(T *dst, int n);
	};

	template <class T>
	T Deserializer::Read() {
		char *s = static_cast<char*>(src);
		s += counter;
		T *t = reinterpret_cast<T*>(s);
		counter += sizeof(T);
		return *t;
	}

	template <class T>
	void Deserializer::Read(T *dst, int n) {
		char *s = static_cast<char*>(src);
		s += counter;
		T *t = reinterpret_cast<T*>(s);
		std::copy(t, t + n, dst);
		counter += sizeof(T)*n;
	}

}

#endif // _SERIALIZER_H_