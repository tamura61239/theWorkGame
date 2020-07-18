#pragma once
#pragma once
//**************************************
//   include Headers
//**************************************
#include <algorithm>
#include <DirectXMath.h>

template <typename T> constexpr const T& Clamp(const T& v, const T& lo, const T& hi)
{
	assert(hi >= lo);
	return (std::max)((std::min)(v, hi), lo);
}
inline float _FRand(const float& lo, const float& hi)
{
	return static_cast<float>(rand()) / RAND_MAX * (hi - lo) + lo;
}
inline int _IRand(const int& lo, const int& hi)
{
	return rand() / RAND_MAX * (hi - lo) + lo;
}


class VECTOR2 : public DirectX::XMINT2
{
public:
	VECTOR2() : DirectX::XMINT2(0, 0) {};
	~VECTOR2() {};

	VECTOR2(const int x, const int y) : DirectX::XMINT2(x, y) {};
	VECTOR2(const VECTOR2& v) { x = v.x; y = v.y; }

	VECTOR2& operator=(const VECTOR2& v) { x = v.x; y = v.y; return *this; }
	VECTOR2& operator+=(const VECTOR2& v) { x += v.x; y += v.y; return *this; }
	VECTOR2& operator-=(const VECTOR2& v) { x -= v.x; y -= v.y; return *this; }
	VECTOR2& operator*=(const int f) { x *= f; y *= f; return *this; }
	VECTOR2& operator/=(const int f) { x /= f; y *= f; return *this; }

	VECTOR2 operator+() const { return VECTOR2(x, y); }
	VECTOR2 operator-() const { return VECTOR2(-x, -y); }

	VECTOR2 operator+(const VECTOR2& v) const { return VECTOR2(x + v.x, y + v.y); }
	VECTOR2 operator-(const VECTOR2& v) const { return VECTOR2(x - v.x, y - v.y); }
	VECTOR2 operator*(const int f) const { return VECTOR2(x * f, y * f); }
	VECTOR2 operator/(const int f) const { return VECTOR2(x / f, y / f); }

	bool operator == (const VECTOR2& v) const { return (x == v.x) && (y == v.y); }
	bool operator != (const VECTOR2& v) const { return (x != v.x) || (y != v.y); }
};

class UVECTOR2 : public DirectX::XMUINT2
{
public:
	UVECTOR2() : DirectX::XMUINT2(0, 0) {};
	~UVECTOR2() {};

	UVECTOR2(const int x, const int y) : DirectX::XMUINT2(x, y) {};
	UVECTOR2(const UVECTOR2& v) { x = v.x; y = v.y; }

	UVECTOR2& operator=(const UVECTOR2& v) { x = v.x; y = v.y; return *this; }
	UVECTOR2& operator+=(const UVECTOR2& v) { x += v.x; y += v.y; return *this; }
	UVECTOR2& operator-=(const UVECTOR2& v) { x -= v.x; y -= v.y; return *this; }
	UVECTOR2& operator*=(const int f) { x *= f; y *= f; return *this; }
	UVECTOR2& operator/=(const int f) { x /= f; y /= f; return *this; }

	UVECTOR2 operator+() const { return UVECTOR2(x, y); }
	//UVECTOR2 operator-() const { return UVECTOR2(-x, -y); }

	UVECTOR2 operator+(const UVECTOR2& v) const { return UVECTOR2(x + v.x, y + v.y); }
	UVECTOR2 operator-(const UVECTOR2& v) const { return UVECTOR2(x - v.x, y - v.y); }
	UVECTOR2 operator*(const int f) const { return UVECTOR2(x * f, y * f); }
	UVECTOR2 operator/(const int f) const { return UVECTOR2(x / f, y / f); }

	bool operator == (const UVECTOR2& v) const { return (x == v.x) && (y == v.y); }
	bool operator != (const UVECTOR2& v) const { return (x != v.x) || (y != v.y); }
};

class VECTOR2F : public DirectX::XMFLOAT2
{
public:
	VECTOR2F() : DirectX::XMFLOAT2(0.f, 0.f) {}
	VECTOR2F(const float x, const float y) : DirectX::XMFLOAT2(x, y) {}
	VECTOR2F(const VECTOR2F& v) { x = v.x; y = v.y; }
	~VECTOR2F() {}

	VECTOR2F& operator=(const VECTOR2F& v) { x = v.x; y = v.y; return *this; }
	VECTOR2F& operator+=(const VECTOR2F& v) { x += v.x; y += v.y; return *this; }
	VECTOR2F& operator-=(const VECTOR2F& v) { x -= v.x; y -= v.y; return *this; }
	VECTOR2F& operator*=(const float f) { x *= f; y *= f; return *this; }
	VECTOR2F& operator/=(const float f) { x /= f; y /= f; return *this; }

	VECTOR2F operator+() const { return VECTOR2F(x, y); }
	VECTOR2F operator-() const { return VECTOR2F(-x, -y); }

	VECTOR2F operator+(const VECTOR2F& v) const { return VECTOR2F(x + v.x, y + v.y); }
	VECTOR2F operator-(const VECTOR2F& v) const { return VECTOR2F(x - v.x, y - v.y); }
	VECTOR2F operator*(const float f) const { return VECTOR2F(x * f, y * f); }
	VECTOR2F operator/(const float f) const { return VECTOR2F(x / f, y / f); }

	bool operator == (const VECTOR2F& v) const { return (x == v.x) && (y == v.y); }
	bool operator != (const VECTOR2F& v) const { return (x != v.x) || (y != v.y); }
};

float ToDistVec2(const VECTOR2F v);
VECTOR2F NormalizeVec2(const VECTOR2F v);
VECTOR2F ConvertVec2(const float conv, const VECTOR2F v);
float CrossVec2(const VECTOR2F v1, const VECTOR2F v2);
float DotVec2(const VECTOR2F v1, const VECTOR2F v2);


class VECTOR3 : public DirectX::XMINT3
{
public:
	VECTOR3() : DirectX::XMINT3(0, 0, 0) {};
	~VECTOR3() {};

	VECTOR3(const int x, const int y, const int z) : DirectX::XMINT3(x, y, z) {};
	VECTOR3(const VECTOR3& v) { x = v.x; y = v.y; z = v.z; }

	VECTOR3& operator=(const VECTOR3& v) { x = v.x; y = v.y; z = v.z; return *this; }
	VECTOR3& operator+=(const VECTOR3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	VECTOR3& operator-=(const VECTOR3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	VECTOR3& operator*=(const int f) { x *= f; y *= f; z *= f; return *this; }
	VECTOR3& operator/=(const int f) { x /= f; y /= f; z /= f; return *this; }

	VECTOR3 operator+() const { return VECTOR3(x, y, z); }
	VECTOR3 operator-() const { return VECTOR3(-x, -y, -z); }

	VECTOR3 operator+(const VECTOR3& v) const { return VECTOR3(x + v.x, y + v.y, z + v.z); }
	VECTOR3 operator-(const VECTOR3& v) const { return VECTOR3(x - v.x, y - v.y, z - v.z); }
	VECTOR3 operator*(const int f) const { return VECTOR3(x * f, y * f, z * f); }
	VECTOR3 operator/(const int f) const { return VECTOR3(x / f, y / f, z / f); }

	bool operator == (const VECTOR3& v) const { return (x == v.x) && (y == v.y) && (z == v.z); }
	bool operator != (const VECTOR3& v) const { return (x != v.x) || (y != v.y) || (z != v.z); }
};

class UVECTOR3 : public DirectX::XMUINT3
{
public:
	UVECTOR3() : DirectX::XMUINT3(0, 0, 0) {};
	~UVECTOR3() {};

	UVECTOR3(const int x, const int y, const int z) : DirectX::XMUINT3(x, y, z) {};
	UVECTOR3(const VECTOR3& v) { x = v.x; y = v.y; z = v.z; }

	UVECTOR3& operator=(const UVECTOR3& v) { x = v.x; y = v.y; z = v.z; return *this; }
	UVECTOR3& operator+=(const UVECTOR3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	UVECTOR3& operator-=(const UVECTOR3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	UVECTOR3& operator*=(const int f) { x *= f; y *= f; z *= f; return *this; }
	UVECTOR3& operator/=(const int f) { x /= f; y /= f; z /= f; return *this; }

	UVECTOR3 operator+() const { return UVECTOR3(x, y, z); }
	//UVECTOR3 operator-() const { return UVECTOR3(-x, -y, -z); }

	UVECTOR3 operator+(const UVECTOR3& v) const { return UVECTOR3(x + v.x, y + v.y, z + v.z); }
	UVECTOR3 operator-(const UVECTOR3& v) const { return UVECTOR3(x - v.x, y - v.y, z - v.z); }
	UVECTOR3 operator*(const int f) const { return UVECTOR3(x * f, y * f, z * f); }
	UVECTOR3 operator/(const int f) const { return UVECTOR3(x / f, y / f, z / f); }

	bool operator == (const UVECTOR3& v) const { return (x == v.x) && (y == v.y) && (z == v.z); }
	bool operator != (const UVECTOR3& v) const { return (x != v.x) || (y != v.y) || (z != v.z); }
};

class VECTOR3F : public DirectX::XMFLOAT3
{
public:
	VECTOR3F() : DirectX::XMFLOAT3(0.f, 0.f, 0.f) {};
	~VECTOR3F() {};

	VECTOR3F(const float x, const float y, const float z) : DirectX::XMFLOAT3(x, y, z) {};
	VECTOR3F(const VECTOR3F& v) { x = v.x; y = v.y; z = v.z; }

	VECTOR3F& operator=(const VECTOR3F& v) { x = v.x; y = v.y; z = v.z; return *this; }
	VECTOR3F& operator+=(const VECTOR3F& v) { x += v.x; y += v.y; z += v.z; return *this; }
	VECTOR3F& operator-=(const VECTOR3F& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	VECTOR3F& operator*=(const float f) { x *= f; y *= f; z *= f; return *this; }
	VECTOR3F& operator/=(const float f) { x /= f; y /= f; z /= f; return *this; }

	VECTOR3F operator+() const { return VECTOR3F(x, y, z); }
	VECTOR3F operator-() const { return VECTOR3F(-x, -y, -z); }

	VECTOR3F operator+(const VECTOR3F& v) const { return VECTOR3F(x + v.x, y + v.y, z + v.z); }
	VECTOR3F operator-(const VECTOR3F& v) const { return VECTOR3F(x - v.x, y - v.y, z - v.z); }
	VECTOR3F operator*(const float f) const { return VECTOR3F(x * f, y * f, z * f); }
	VECTOR3F operator/(const float f) const { return VECTOR3F(x / f, y / f, z / f); }

	bool operator == (const VECTOR3F& v) const { return (x == v.x) && (y == v.y) && (z == v.z); }
	bool operator != (const VECTOR3F& v) const { return (x != v.x) || (y != v.y) || (z != v.z); }
};

float ToDistVec3(const VECTOR3F v);
VECTOR3F NormalizeVec3(const VECTOR3F v);
VECTOR3F ConvertVec3(const float conv, const VECTOR3F v);
VECTOR3F CrossVec3(const VECTOR3F v1, const VECTOR3F v2);
float DotVec3(const VECTOR3F v1, const VECTOR3F v2);

class VECTOR4 : public DirectX::XMINT4
{
public:
	VECTOR4() : DirectX::XMINT4(0, 0, 0, 0) {};
	~VECTOR4() {};

	VECTOR4(const int x, const int y, const int z, const int w) : DirectX::XMINT4(x, y, z, w) {};
	VECTOR4(const VECTOR4& v) { x = v.x; y = v.y; z = v.z; w = v.z; }

	VECTOR4& operator=(const VECTOR4& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
	VECTOR4& operator+=(const VECTOR4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	VECTOR4& operator-=(const VECTOR4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	VECTOR4& operator*=(const int f) { x *= f; y *= f; z *= f; w *= f; return *this; }
	VECTOR4& operator/=(const int f) { x /= f; y /= f; z /= f; w /= f; return *this; }

	VECTOR4 operator+() const { return VECTOR4(x, y, z, w); }
	VECTOR4 operator-() const { return VECTOR4(-x, -y, -z, -w); }

	VECTOR4 operator+(const VECTOR4& v) const { return VECTOR4(x + v.x, y + v.y, z + v.z, w + v.w); }
	VECTOR4 operator-(const VECTOR4& v) const { return VECTOR4(x - v.x, y - v.y, z - v.z, w - v.w); }
	VECTOR4 operator*(const int f) const { return VECTOR4(x * f, y * f, z * f, w * f); }
	VECTOR4 operator/(const int f) const { return VECTOR4(x / f, y / f, z / f, w * f); }

	bool operator == (const VECTOR4& v) const { return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w); }
	bool operator != (const VECTOR4& v) const { return (x != v.x) || (y != v.y) || (z != v.z) || (w != v.w); }
};

class UVECTOR4 : public DirectX::XMUINT4
{
public:
	UVECTOR4() : DirectX::XMUINT4(0, 0, 0, 0) {};
	~UVECTOR4() {};

	UVECTOR4(const int x, const int y, const int z, const int w) : DirectX::XMUINT4(x, y, z, w) {};
	UVECTOR4(const UVECTOR4& v) { x = v.x; y = v.y; z = v.z; w = v.z; }

	UVECTOR4& operator=(const UVECTOR4& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
	UVECTOR4& operator+=(const UVECTOR4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	UVECTOR4& operator-=(const UVECTOR4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	UVECTOR4& operator*=(const int f) { x *= f; y *= f; z *= f; w *= f; return *this; }
	UVECTOR4& operator/=(const int f) { x /= f; y /= f; z /= f; w /= f; return *this; }

	UVECTOR4 operator+() const { return UVECTOR4(x, y, z, w); }
	//UVECTOR4 operator-() const { return UVECTOR4(-x, -y, -z, -w); }

	UVECTOR4 operator+(const UVECTOR4& v) const { return UVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w); }
	UVECTOR4 operator-(const UVECTOR4& v) const { return UVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w); }
	UVECTOR4 operator*(const int f) const { return UVECTOR4(x * f, y * f, z * f, w * f); }
	UVECTOR4 operator/(const int f) const { return UVECTOR4(x / f, y / f, z / f, w * f); }

	bool operator == (const UVECTOR4& v) const { return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w); }
	bool operator != (const UVECTOR4& v) const { return (x != v.x) || (y != v.y) || (z != v.z) || (w != v.w); }
};

class VECTOR4F : public DirectX::XMFLOAT4
{
public:
	VECTOR4F() : DirectX::XMFLOAT4(0.f, 0.f, 0.f, 0.f) {};
	~VECTOR4F() {};

	VECTOR4F(const float x, const float y, const float z, const float w) : DirectX::XMFLOAT4(x, y, z, w) {};
	VECTOR4F(const VECTOR4F& v) { x = v.x; y = v.y; z = v.z; w = v.w; }

	VECTOR4F& operator=(const VECTOR4F& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
	VECTOR4F& operator+=(const VECTOR4F& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	VECTOR4F& operator-=(const VECTOR4F& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	VECTOR4F& operator*=(const float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
	VECTOR4F& operator/=(const float f) { x /= f; y /= f; z /= f; w /= f; return *this; }

	VECTOR4F operator+() const { return VECTOR4F(x, y, z, w); }
	VECTOR4F operator-() const { return VECTOR4F(-x, -y, -z, -w); }

	VECTOR4F operator+(const VECTOR4F& v) const { return VECTOR4F(x + v.x, y + v.y, z + v.z, w + v.w); }
	VECTOR4F operator-(const VECTOR4F& v) const { return VECTOR4F(x - v.x, y - v.y, z - v.z, w - v.w); }
	VECTOR4F operator*(const float f) const { return VECTOR4F(x * f, y * f, z * f, w * f); }
	VECTOR4F operator/(const float f) const { return VECTOR4F(x / f, y / f, z / f, w / f); }

	bool operator == (const VECTOR4F& v) const { return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w); }
	bool operator != (const VECTOR4F& v) const { return (x != v.x) || (y != v.y) || (z != v.z) || (w != v.w); }
};

class FLOAT4X4 : public DirectX::XMFLOAT4X4
{
public:
	FLOAT4X4() : DirectX::XMFLOAT4X4() {};
	~FLOAT4X4() {};

	FLOAT4X4(const float _11, const float _12, const float _13, const float _14,
		const float _21, const float _22, const float _23, const float _24,
		const float _31, const float _32, const float _33, const float _34,
		const float _41, const float _42, const float _43, const float _44)
	{
		this->_11 = _11; this->_12 = _12; this->_13 = _13; this->_14 = _14;
		this->_21 = _21; this->_22 = _22; this->_23 = _23; this->_24 = _24;
		this->_31 = _31; this->_32 = _32; this->_33 = _33; this->_34 = _34;
		this->_41 = _41; this->_42 = _42; this->_43 = _43; this->_44 = _44;
	}

	FLOAT4X4(const FLOAT4X4& f4x4)
	{
		_11 = f4x4._11; _12 = f4x4._12; _13 = f4x4._13; _14 = f4x4._14;
		_21 = f4x4._21; _22 = f4x4._22; _23 = f4x4._23; _24 = f4x4._24;
		_31 = f4x4._31; _32 = f4x4._32; _33 = f4x4._33; _34 = f4x4._34;
		_41 = f4x4._41; _42 = f4x4._42; _43 = f4x4._43; _44 = f4x4._44;
	}

	FLOAT4X4 operator*(const FLOAT4X4& m) const {
		float x = _11;
		float y = _12;
		float z = _13;
		float w = _14;

		FLOAT4X4 result;
		result._11 = (m._11 * x) + (m._21 * y) + (m._31 * z) + (m._41 * w);
		result._12 = (m._12 * x) + (m._22 * y) + (m._32 * z) + (m._42 * w);
		result._13 = (m._13 * x) + (m._23 * y) + (m._33 * z) + (m._43 * w);
		result._14 = (m._14 * x) + (m._24 * y) + (m._34 * z) + (m._44 * w);

		x = _21;
		y = _22;
		z = _23;
		w = _24;
		result._21 = (m._11 * x) + (m._21 * y) + (m._31 * z) + (m._41 * w);
		result._22 = (m._12 * x) + (m._22 * y) + (m._32 * z) + (m._42 * w);
		result._23 = (m._13 * x) + (m._23 * y) + (m._33 * z) + (m._43 * w);
		result._24 = (m._14 * x) + (m._24 * y) + (m._34 * z) + (m._44 * w);

		x = _31;
		y = _32;
		z = _33;
		w = _34;
		result._31 = (m._11 * x) + (m._21 * y) + (m._31 * z) + (m._41 * w);
		result._32 = (m._12 * x) + (m._22 * y) + (m._32 * z) + (m._42 * w);
		result._33 = (m._13 * x) + (m._23 * y) + (m._33 * z) + (m._43 * w);
		result._34 = (m._14 * x) + (m._24 * y) + (m._34 * z) + (m._44 * w);

		x = _41;
		y = _42;
		z = _43;
		w = _44;
		result._41 = (m._11 * x) + (m._21 * y) + (m._31 * z) + (m._41 * w);
		result._42 = (m._12 * x) + (m._22 * y) + (m._32 * z) + (m._42 * w);
		result._43 = (m._13 * x) + (m._23 * y) + (m._33 * z) + (m._43 * w);
		result._44 = (m._14 * x) + (m._24 * y) + (m._34 * z) + (m._44 * w);

		return result;
	}
};