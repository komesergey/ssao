#ifndef __VMATH_H__
#define __VMATH_H__

#define _USE_MATH_DEFINES  1 // Include constants defined in math.h
#include <math.h>

namespace vmath {

	template <typename T, const int w, const int h> class matNM;
	template <typename T, const int len> class vecN;
	template <typename T> class Tquaternion;

	template <typename T>
	inline T radians(T angleInDegrees) {
		return angleInDegrees * static_cast<T>(M_PI / 180.0);
	}

	template <typename T, const int len>
	class vecN {
	public:
		typedef class vecN<T, len> my_type;
		typedef T element_type;
		inline vecN() {}
		inline vecN(const vecN& that) {
			assign(that);
		}

		inline vecN(T s) {
			int n;
			for (n = 0; n < len; n++) {
				data[n] = s;
			}
		}

		inline vecN& operator=(const vecN& that) {
			assign(that);
			return *this;
		}

		inline vecN& operator=(const T& that) {
			int n;
			for (n = 0; n < len; n++)
				data[n] = that;
			return *this;
		}

		inline vecN operator+(const vecN& that) const {
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] + that.data[n];
			return result;
		}

		inline vecN& operator+=(const vecN& that) {
			return (*this = *this + that);
		}

		inline vecN operator-() const {
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = -data[n];
			return result;
		}

		inline vecN operator-(const vecN& that) const {
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] - that.data[n];
			return result;
		}

		inline vecN& operator-=(const vecN& that){
			return (*this = *this - that);
		}

		inline vecN operator*(const vecN& that) const {
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] * that.data[n];
			return result;
		}

		inline vecN& operator*=(const vecN& that) {
			return (*this = *this * that);
		}

		inline vecN operator*(const T& that) const {
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] * that;
			return result;
		}

		inline vecN& operator*=(const T& that) {
			assign(*this * that);
			return *this;
		}

		inline vecN operator/(const vecN& that) const {
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] / that.data[n];
			return result;
		}

		inline vecN& operator/=(const vecN& that) {
			assign(*this / that);
			return *this;
		}

		inline vecN operator/(const T& that) const {
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] / that;
			return result;
		}

		inline vecN& operator/=(const T& that) {
			assign(*this / that);
			return *this;
		}

		inline T& operator[](int n) { return data[n]; }
		inline const T& operator[](int n) const { return data[n]; }
		inline static int size(void) { return len; }
		inline operator const T* () const { return &data[0]; }

	protected:
		T data[len];
		inline void assign(const vecN& that) {
			int n;
			for (n = 0; n < len; n++)
				data[n] = that.data[n];
		}
	};

	template <typename T>
	class Tvec3 : public vecN<T, 3> {
	public:
		typedef vecN<T, 3> base;
		inline Tvec3() {}
		inline Tvec3(const base& v) : base(v) {}
		inline Tvec3(T x, T y, T z) {
			base::data[0] = x;
			base::data[1] = y;
			base::data[2] = z;
		}
	};

	template <typename T>
	class Tvec4 : public vecN<T, 4> {
	public:
		typedef vecN<T, 4> base;
		inline Tvec4() {}
		inline Tvec4(T x, T y, T z, T w) {
			base::data[0] = x;
			base::data[1] = y;
			base::data[2] = z;
			base::data[3] = w;
		}
	};


	typedef Tvec3<float> vec3;
	typedef Tvec4<float> vec4;

	template <typename T, int n>
	static inline const vecN<T, n> operator * (T x, const vecN<T, n>& v) {
		return v * x;
	}

	template <typename T>
	static inline const Tvec3<T> operator / (T x, const Tvec3<T>& v) {
		return Tvec3<T>(x / v[0], x / v[1], x / v[2]);
	}

	template <typename T>
	static inline const Tvec4<T> operator / (T x, const Tvec4<T>& v) {
		return Tvec4<T>(x / v[0], x / v[1], x / v[2], x / v[3]);
	}

	template <typename T>
	static inline vecN<T, 3> cross(const vecN<T, 3>& a, const vecN<T, 3>& b) {
		return Tvec3<T>(a[1] * b[2] - b[1] * a[2],
			a[2] * b[0] - b[2] * a[0],
			a[0] * b[1] - b[0] * a[1]);
	}

	template <typename T, int len>
	static inline T length(const vecN<T, len>& v) {
		T result(0);
		for (int i = 0; i < v.size(); ++i) {
			result += v[i] * v[i];
		}
		return (T)sqrt(result);
	}

	template <typename T, int len>
	static inline vecN<T, len> normalize(const vecN<T, len>& v) {
		return v / length(v);
	}

	template <typename T>
	class Tquaternion {
	public:
		inline Tquaternion() {}
		inline Tquaternion(const Tquaternion& q) : r(q.r), v(q.v) {}
		inline Tquaternion(T _r, const Tvec3<T>& _v) : r(_r), v(_v) {}
		inline Tquaternion(T _x, T _y, T _z, T _w) : r(_x), v(_y, _z, _w) {}
		inline T& operator[](int n) {
			return a[n];
		}
		inline const T& operator[](int n) const {
			return a[n];
		}
		inline Tquaternion operator+(const Tquaternion& q) const {
			return quaternion(r + q.r, v + q.v);
		}
		inline Tquaternion& operator+=(const Tquaternion& q) {
			r += q.r;
			v += q.v;
			return *this;
		}
		inline Tquaternion operator-(const Tquaternion& q) const {
			return quaternion(r - q.r, v - q.v);
		}
		inline Tquaternion& operator-=(const Tquaternion& q) {
			r -= q.r;
			v -= q.v;
			return *this;
		}
		inline Tquaternion operator-() const {
			return Tquaternion(-r, -v);
		}
		inline Tquaternion operator*(const T s) const {
			return Tquaternion(a[0] * s, a[1] * s, a[2] * s, a[3] * s);
		}
		inline Tquaternion& operator*=(const T s) {
			r *= s;
			v *= s;
			return *this;
		}
		inline Tquaternion operator*(const Tquaternion& q) const {
			const T x1 = a[0];
			const T y1 = a[1];
			const T z1 = a[2];
			const T w1 = a[3];
			const T x2 = q.a[0];
			const T y2 = q.a[1];
			const T z2 = q.a[2];
			const T w2 = q.a[3];
			return Tquaternion(w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2,
				w1 * y2 + y1 * w2 + z1 * x2 - x1 * z2,
				w1 * z2 + z1 * w2 + x1 * y2 - y1 * x2,
				w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2);
		}
		inline Tquaternion operator/(const T s) const {
			return Tquaternion(a[0] / s, a[1] / s, a[2] / s, a[3] / s);
		}
		inline Tquaternion& operator/=(const T s) {
			r /= s;
			v /= s;
			return *this;
		}
		inline operator Tvec4<T>&() {
			return *(Tvec4<T>*)&a[0];
		}
		inline operator const Tvec4<T>&() const {
			return *(const Tvec4<T>*)&a[0];
		}
		inline bool operator==(const Tquaternion& q) const {
			return (r == q.r) && (v == q.v);
		}
		inline bool operator!=(const Tquaternion& q) const {
			return (r != q.r) || (v != q.v);
		}
	private:
		union {
			struct {
				T           r;
				Tvec3<T>    v;
			};
			struct {
				T           x;
				T           y;
				T           z;
				T           w;
			};
			T               a[4];
		};
	};

	typedef Tquaternion<float> quaternion;

	template <typename T>
	static inline Tquaternion<T> operator*(T a, const Tquaternion<T>& b) {
		return b * a;
	}

	template <typename T>
	static inline Tquaternion<T> operator/(T a, const Tquaternion<T>& b) {
		return Tquaternion<T>(a / b[0], a / b[1], a / b[2], a / b[3]);
	}

	template <typename T, const int w, const int h>
	class matNM {
	public:
		typedef class matNM<T, w, h> my_type;
		typedef class vecN<T, h> vector_type;
		inline matNM() {}
		inline matNM(const matNM& that) {
			assign(that);
		}
		inline matNM(const vector_type& v) {
			for (int n = 0; n < w; n++) {
				data[n] = v;
			}
		}
		inline matNM& operator=(const my_type& that) {
			assign(that);
			return *this;
		}
		inline matNM operator+(const my_type& that) const {
			my_type result;
			int n;
			for (n = 0; n < w; n++)
				result.data[n] = data[n] + that.data[n];
			return result;
		}
		inline my_type& operator+=(const my_type& that) {
			return (*this = *this + that);
		}
		inline my_type operator-(const my_type& that) const {
			my_type result;
			int n;
			for (n = 0; n < w; n++)
				result.data[n] = data[n] - that.data[n];
			return result;
		}
		inline my_type& operator-=(const my_type& that) {
			return (*this = *this - that);
		}
		inline my_type operator*(const T& that) const {
			my_type result;
			int n;
			for (n = 0; n < w; n++)
				result.data[n] = data[n] * that;
			return result;
		}
		inline my_type& operator*=(const T& that) {
			int n;
			for (n = 0; n < w; n++)
				data[n] = data[n] * that;
			return *this;
		}
		inline my_type operator*(const my_type& that) const {
			my_type result(0);
			for (int j = 0; j < w; j++) {
				for (int i = 0; i < h; i++) {
					T sum(0);
					for (int n = 0; n < w; n++) {
						sum += data[n][i] * that[j][n];
					}
					result[j][i] = sum;
				}
			}
			return result;
		}
		inline my_type& operator*=(const my_type& that) {
			return (*this = *this * that);
		}
		inline vector_type& operator[](int n) { return data[n]; }
		inline const vector_type& operator[](int n) const { return data[n]; }
		inline operator T*() { return &data[0][0]; }
		inline operator const T*() const { return &data[0][0]; }
		static inline my_type identity() {
			my_type result(0);
			for (int i = 0; i < w; i++) {
				result[i][i] = 1;
			}
			return result;
		}
	protected:
		vecN<T, h> data[w];
		inline void assign(const matNM& that) {
			int n;
			for (n = 0; n < w; n++)
				data[n] = that.data[n];
		}
	};

	template <typename T>
	class Tmat4 : public matNM<T, 4, 4> {
	public:
		typedef matNM<T, 4, 4> base;
		typedef Tmat4<T> my_type;
		inline Tmat4() {}
		inline Tmat4(const my_type& that) : base(that) {}
		inline Tmat4(const base& that) : base(that) {}
		inline Tmat4(const vecN<T, 4>& v) : base(v) {}
		inline Tmat4(const vecN<T, 4>& v0,
			const vecN<T, 4>& v1,
			const vecN<T, 4>& v2,
			const vecN<T, 4>& v3) {
			base::data[0] = v0;
			base::data[1] = v1;
			base::data[2] = v2;
			base::data[3] = v3;
		}
	};
	typedef Tmat4<float> mat4;
	static inline mat4 perspective(float fovy, float aspect, float n, float f) {
		float q = 1.0f / tan(radians(0.5f * fovy));
		float A = q / aspect;
		float B = (n + f) / (n - f);
		float C = (2.0f * n * f) / (n - f);
		mat4 result;
		result[0] = vec4(A, 0.0f, 0.0f, 0.0f);
		result[1] = vec4(0.0f, q, 0.0f, 0.0f);
		result[2] = vec4(0.0f, 0.0f, B, -1.0f);
		result[3] = vec4(0.0f, 0.0f, C, 0.0f);
		return result;
	}

	template <typename T>
	static inline Tmat4<T> translate(T x, T y, T z) {
		return Tmat4<T>(Tvec4<T>(1.0f, 0.0f, 0.0f, 0.0f),
			Tvec4<T>(0.0f, 1.0f, 0.0f, 0.0f),
			Tvec4<T>(0.0f, 0.0f, 1.0f, 0.0f),
			Tvec4<T>(x, y, z, 1.0f));
	}

	template <typename T>
	static inline Tmat4<T> translate(const vecN<T, 3>& v) {
		return translate(v[0], v[1], v[2]);
	}

	template <typename T>
	static inline Tmat4<T> lookat(const vecN<T, 3>& eye, const vecN<T, 3>& center, const vecN<T, 3>& up) {
		const Tvec3<T> f = normalize(center - eye);
		const Tvec3<T> upN = normalize(up);
		const Tvec3<T> s = cross(f, upN);
		const Tvec3<T> u = cross(s, f);
		const Tmat4<T> M = Tmat4<T>(Tvec4<T>(s[0], u[0], -f[0], T(0)),
			Tvec4<T>(s[1], u[1], -f[1], T(0)),
			Tvec4<T>(s[2], u[2], -f[2], T(0)),
			Tvec4<T>(T(0), T(0), T(0), T(1)));

		return M * translate<T>(-eye);
	}

	template <typename T>
	static inline Tmat4<T> scale(T x, T y, T z) {
		return Tmat4<T>(Tvec4<T>(x, 0.0f, 0.0f, 0.0f),
			Tvec4<T>(0.0f, y, 0.0f, 0.0f),
			Tvec4<T>(0.0f, 0.0f, z, 0.0f),
			Tvec4<T>(0.0f, 0.0f, 0.0f, 1.0f));
	}

	template <typename T>
	static inline Tmat4<T> rotate(T angle, T x, T y, T z) {
		Tmat4<T> result;
		const T x2 = x * x;
		const T y2 = y * y;
		const T z2 = z * z;
		float rads = float(angle) * 0.0174532925f;
		const float c = cosf(rads);
		const float s = sinf(rads);
		const float omc = 1.0f - c;
		result[0] = Tvec4<T>(T(x2 * omc + c), T(y * x * omc + z * s), T(x * z * omc - y * s), T(0));
		result[1] = Tvec4<T>(T(x * y * omc - z * s), T(y2 * omc + c), T(y * z * omc + x * s), T(0));
		result[2] = Tvec4<T>(T(x * z * omc + y * s), T(y * z * omc - x * s), T(z2 * omc + c), T(0));
		result[3] = Tvec4<T>(T(0), T(0), T(0), T(1));
		return result;
	}

	template <typename T, const int N, const int M>
	static inline vecN<T, N> operator*(const vecN<T, M>& vec, const matNM<T, N, M>& mat) {
		int n, m;
		vecN<T, N> result(T(0));
		for (m = 0; m < M; m++) {
			for (n = 0; n < N; n++) {
				result[n] += vec[m] * mat[n][m];
			}
		}
		return result;
	}

	template <typename T, const int N>
	static inline vecN<T, N> operator/(const T s, const vecN<T, N>& v) {
		int n;
		vecN<T, N> result;
		for (n = 0; n < N; n++) {
			result[n] = s / v[n];
		}
		return result;
	}
};
#endif /* __VMATH_H__ */
