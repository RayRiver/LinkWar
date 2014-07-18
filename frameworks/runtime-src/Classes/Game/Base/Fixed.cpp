#include "Fixed.h"

#include <stdio.h>

Fixed const Fixed::ZERO = Fixed();
Fixed const Fixed::ONE = Fixed(1);

Fixed::Fixed()
	: m_intPart(0)
	, m_decPart(0)
{

}

Fixed::Fixed( int i )
{
	Fixed::_fromint(i, *this);
}

Fixed::Fixed( float f )
{
	Fixed::_fromfloat(f, *this);
}

Fixed::Fixed( double f )
{
	Fixed::_fromdouble(f, *this);
}

Fixed::Fixed( const Fixed &other )
{
	Fixed::_copy(other, *this);
}

Fixed::~Fixed()
{

}

void Fixed::_print( const Fixed &f )
{
	printf("%+d %+f\t", f.m_intPart, (float)f.m_decPart / Fixed::MultiplyFactor);
}

