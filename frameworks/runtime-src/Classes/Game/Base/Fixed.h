// 定点数实现;
// see http://blog.sina.com.cn/s/blog_6e22f4ce0101s70i.html for details;

#ifndef Fixed_h__
#define Fixed_h__

#include <stdint.h>
#include <math.h>
#include <assert.h>

class Fixed
{
public:
	static const Fixed ZERO;
	static const Fixed ONE;

public:
	Fixed();

	explicit Fixed(int i);
	explicit Fixed(float f);
	explicit Fixed(double f);

	Fixed(const Fixed &other);

	~Fixed();

	inline int intPart() const { return m_intPart; }

	inline Fixed operator-() const
	{
		Fixed result;
		Fixed::_rev(*this, result);
		return result;
	}

	// 赋值方法;
	inline void operator=(int i)
	{
		Fixed::_fromint(i, *this);
	}
	inline void operator=(float f)
	{
		Fixed::_fromfloat(f, *this);
	}
	inline void operator=(double f)
	{
		Fixed::_fromdouble(f, *this);
	}
	inline void operator=(const Fixed &other)
	{
		Fixed::_copy(other, *this);
	}

	inline void set(int i)
	{
		
	}
	inline void set(float f)
	{
		Fixed::_fromfloat(f, *this);
	}
	inline void set(double f)
	{
		Fixed::_fromdouble(f, *this);
	}
	inline void set(const Fixed &other)
	{
		Fixed::_copy(other, *this);
	}

	// 运算方法;
	inline Fixed operator+(const Fixed &other) const
	{
		Fixed result;
		Fixed::_add(*this, other, result);
		return result;
	}
	inline void operator+=(const Fixed &other)
	{
		Fixed::_add(*this, other, *this);
	}
	inline Fixed operator-(const Fixed &other) const
	{
		Fixed result;
		Fixed::_sub(*this, other, result);
		return result;
	}
	inline void operator-=(const Fixed &other)
	{
		Fixed::_sub(*this, other, *this);
	}
	inline Fixed operator*(const Fixed &other) const
	{
		Fixed result;
		Fixed::_mul(*this, other, result);
		return result;
	}
	inline void operator*=(const Fixed &other)
	{
		Fixed::_mul(*this, other, *this);
	}
	inline Fixed operator/(const Fixed &other) const
	{
		Fixed result;
		Fixed::_div(*this, other, result);
		return result;
	}
	inline void operator/=(const Fixed &other)
	{
		Fixed::_div(*this, other, *this);
	}

	inline Fixed sqrt() const
	{
		Fixed result;
		Fixed::_sqrt(*this, result);
		return result;
	}

	inline Fixed abs() const
	{
		Fixed result;
		Fixed::_abs(*this, result);
		return result;
	}

	// ++i
	inline Fixed &operator++()
	{
		Fixed::_add(*this, Fixed::ONE, *this);
		return *this;
	}
	// i++
	const Fixed operator++(int)
	{
		Fixed result = *this;
		Fixed::_add(*this, Fixed::ONE, *this);
		return result;
	}
	// --i
	inline Fixed &operator--()
	{
		Fixed::_sub(*this, Fixed::ONE, *this);
		return *this;
	}
	// i--
	const Fixed operator--(int)
	{
		Fixed result = *this;
		Fixed::_sub(*this, Fixed::ONE, *this);
		return result;
	}

	// 比较方法;
	inline bool operator==(const Fixed &other) const
	{
		return Fixed::_cmp(*this, other) == 0;
	}
	inline bool operator!=(const Fixed &other) const
	{
		return Fixed::_cmp(*this, other) != 0;
	}
	inline bool operator<(const Fixed &other) const
	{
		return Fixed::_cmp(*this, other) < 0;
	}
	inline bool operator<=(const Fixed &other) const
	{
		return Fixed::_cmp(*this, other) <= 0;
	}
	inline bool operator>(const Fixed &other) const
	{
		return Fixed::_cmp(*this, other) > 0;
	}
	inline bool operator>=(const Fixed &other) const
	{
		return Fixed::_cmp(*this, other) >= 0;
	}

	// 强转方法;
	inline operator int() const
	{
		return Fixed::_toint(*this);	
	}
	inline operator float() const
	{
		return Fixed::_tofloat(*this);	
	}
	inline operator double() const
	{
		return Fixed::_todouble(*this);	
	}



	// 其他方法;
	inline void print() const
	{
		Fixed::_print(*this);
	}

private:
	typedef int32_t IntPart;
	typedef int32_t DecPart;

	enum 
	{
		MultiplyFactor = 0x7FFF,    // 定点数乘因子;
		ShiftFactor = 15,           // 定点数移位;
		MaxInt = 2147483647,
	};

private:
	// 符号一致处理;
	inline static void _signProcess(Fixed &f)
	{
		if (f.m_intPart > 0 && f.m_decPart < 0)
		{
			// 整数为正，小数为负：借位;
			f.m_intPart -= 1;	
			f.m_decPart += Fixed::MultiplyFactor;
		}
		else if (f.m_intPart < 0 && f.m_decPart > 0)
		{
			// 整数为负，小数为正：进位;
			f.m_intPart += 1;
			f.m_decPart -= Fixed::MultiplyFactor;
		}
	}

	// 计算定点数可以移位最大位数（移位后不溢出）;
	inline static int32_t _divShiftCalc(const Fixed &f)
	{
		if (0 == f.m_intPart)
		{
			return Fixed::ShiftFactor;
		}

		int32_t den = Fixed::MaxInt / ::abs(f.m_intPart);
		for (int32_t i=31; i>0; --i)
		{
			if ( ((0x01u << i) & den) == (0x01u << i) ) 
			{
				return i;
			}
		}

		assert(false);
		return 0;
	}

	// 计算两个数相除的小数部分（忽略整数部分）;
	inline static void _decNumCalc(Fixed &f, int32_t num, int32_t den)
	{
		if (::abs(num) < Fixed::MultiplyFactor)
		{
			f.m_decPart = (num * Fixed::MultiplyFactor) / den;
			return;
		}

		int32_t div = Fixed::MaxInt / ::abs(num);

		int32_t sft = 0;
		for (int32_t i=15; i>0; --i)
		{
			if ( ((0x01u << i) & div) == (0x01u << i) )  
			{
				sft = i;                         
				break;                         
			}
		}

		f.m_decPart = (num << sft) / den;
		f.m_decPart <<= (Fixed::ShiftFactor - sft);
	}


	// 加法;
	inline static void _add(const Fixed &f1, const Fixed &f2, Fixed &result) 
	{
		// 小数相加;
		DecPart decPart = f1.m_decPart + f2.m_decPart;

		// 进位;
		int32_t carry = decPart / Fixed::MultiplyFactor;

		// 小数部分;
		decPart = decPart % Fixed::MultiplyFactor;

		// 整数部分相加, 带进位位;
		IntPart intPart = f1.m_intPart + f2.m_intPart + carry;

		// 结果，符号处理;
		result.m_intPart = intPart;
		result.m_decPart = decPart;
		Fixed::_signProcess(result);
	}

	// 减法;
	inline static void _sub(const Fixed &f1, const Fixed &f2, Fixed &result)
	{
		// 小数相加;
		DecPart decPart = f1.m_decPart - f2.m_decPart;

		// 进位;
		int32_t carry = decPart / Fixed::MultiplyFactor;

		// 小数部分;
		decPart = decPart % Fixed::MultiplyFactor;

		// 整数部分相加, 带进位位;
		IntPart intPart = f1.m_intPart - f2.m_intPart + carry;

		// 结果，符号处理;
		result.m_intPart = intPart;
		result.m_decPart = decPart;
		Fixed::_signProcess(result);
	}

	// 乘法;
	inline static void _mul(const Fixed &f1, const Fixed &f2, Fixed &result)
	{
		// 基本步骤是：先做低位乘法，记录进位标志，再做高位乘法，最后加上进位标志;
		// 1. 小数部分乘小数部分;
		// 2. 整数部分乘小数部分;
		// 3. 整数部分乘整数部分;

		// 小数部分相乘;
		int32_t micro = (f1.m_decPart * f2.m_decPart) >> Fixed::ShiftFactor;

		int32_t sft_num = Fixed::_divShiftCalc(f1);
		int32_t sft_den = Fixed::_divShiftCalc(f2);

		int32_t sft = (sft_num < sft_den) ? sft_num : sft_den;
		sft = (sft < Fixed::ShiftFactor) ? sft : Fixed::ShiftFactor;

		DecPart decPart = f1.m_intPart * ( f2.m_decPart >> (Fixed::ShiftFactor - sft) ) +
			f2.m_intPart * (f1.m_decPart >> (Fixed::ShiftFactor - sft)) +
			(micro >> (Fixed::ShiftFactor - sft));

		int32_t carry = decPart / ( Fixed::MultiplyFactor >> (Fixed::ShiftFactor - sft) );
		decPart = decPart % ( Fixed::MultiplyFactor >> (Fixed::ShiftFactor - sft) );

		decPart <<= (Fixed::ShiftFactor - sft);

		// 整数部分相乘, 加上进位;
		IntPart intPart = f1.m_intPart * f2.m_intPart + carry;

		// 结果，符号处理;
		result.m_intPart = intPart;
		result.m_decPart = decPart;
		Fixed::_signProcess(result);
	}

	// 除法;
	inline static void _div(const Fixed &f1, const Fixed &f2, Fixed &result)
	{
		int32_t sft_num = Fixed::_divShiftCalc(f1);
		int32_t sft_den = Fixed::_divShiftCalc(f2);

		int32_t sft = (sft_num < sft_den) ? sft_num : sft_den;

		IntPart num;
		DecPart den;
		if (sft < Fixed::ShiftFactor)
		{
			num = (f1.m_intPart << sft) + (f1.m_decPart >> (Fixed::ShiftFactor - sft));
			den = (f2.m_intPart << sft) + (f2.m_decPart >> (Fixed::ShiftFactor - sft));
		}
		else
		{
			num = (f1.m_intPart << Fixed::ShiftFactor) + f1.m_decPart;
			den = (f2.m_intPart << Fixed::ShiftFactor) + f2.m_decPart;
		}

		if (0 == den)
		{
			result.m_intPart = (f1.m_intPart > 0) ? Fixed::MultiplyFactor : -Fixed::MultiplyFactor;
			result.m_decPart = (f1.m_decPart > 0) ? Fixed::MultiplyFactor : -Fixed::MultiplyFactor;
			assert(false);
		}

		IntPart intPart = num / den;
		IntPart redu = num - den * intPart;

		result.m_intPart = intPart;
		Fixed::_decNumCalc(result, redu, den);
		_signProcess(result);
	}

	// 开方;
	inline static void _sqrt(const Fixed &f, Fixed &result)
	{
		int32_t sft = _divShiftCalc(f);

		// 移位余数, 如移位3余1, 不移位;
		int32_t redu = sft % 2;

		IntPart tmp = (f.m_intPart << (sft - redu));

		if (sft < Fixed::ShiftFactor)
		{
			tmp += (f.m_decPart >> (Fixed::ShiftFactor + redu - sft));
		}
		else
		{
			tmp += (f.m_decPart << (sft - Fixed::ShiftFactor - redu));
		}
		// 开方;
		tmp = (IntPart)::sqrt((double)tmp);

		// 开方后, 转换成定点数的分母;
		int32_t den = 1 << (sft/2); 

		result.m_intPart = tmp / den;
		Fixed::_decNumCalc(result, (tmp%den), den);
	}

	// 取相反数;
	inline static void _rev(const Fixed &f, Fixed &result)
	{
		result.m_intPart = -f.m_intPart;
		result.m_decPart = -f.m_decPart;
	}

	// 绝对值;
	inline static void _abs(const Fixed &f, Fixed &result)
	{
		result.m_intPart = ::abs(f.m_intPart);
		result.m_decPart = ::abs(f.m_decPart);
	}

	// 比较运算;
	inline static int _cmp(const Fixed &f1, const Fixed &f2) 
	{
		if (f1.m_intPart > f2.m_intPart)
		{
			return 1;
		}
		else if (f1.m_intPart < f2.m_intPart) 
		{
			return -1;
		}

		if (f1.m_decPart > f2.m_decPart)
		{
			return 1;
		}
		else if (f1.m_decPart < f2.m_decPart)
		{
			return -1;
		}

		return 0;
	}

	// 复制;
	inline static void _copy(const Fixed &f, Fixed &result)
	{
		result.m_intPart = f.m_intPart;
		result.m_decPart = f.m_decPart;
	}
	
	// 基本数据类型之间的转换;
	inline static float _tofloat(const Fixed &f)
	{
		return (float)f.m_intPart + (float)f.m_decPart / Fixed::MultiplyFactor;
	}
	inline static double _todouble(const Fixed &f)
	{
		return (double)f.m_intPart + (double)f.m_decPart / Fixed::MultiplyFactor;
	}
	inline static int _toint(const Fixed &f)
	{
		return (int)f.m_intPart;
	}
	inline static void _fromfloat(float f, Fixed &result)
	{
		result.m_intPart = (IntPart)f;
		result.m_decPart = (DecPart)( (f - result.m_intPart) * Fixed::MultiplyFactor );
	}
	inline static void _fromdouble(double f, Fixed &result)
	{
		result.m_intPart = (IntPart)f;
		result.m_decPart = (DecPart)( (f - result.m_intPart) * Fixed::MultiplyFactor );
	}
	inline static void _fromint(int i, Fixed &result)
	{
		result.m_intPart = (IntPart)i;
		result.m_decPart = 0;
	}

	// 打印;
	static void _print(const Fixed &f);

private:
	IntPart m_intPart;
	DecPart m_decPart;

};

#endif // Fixed_h__
