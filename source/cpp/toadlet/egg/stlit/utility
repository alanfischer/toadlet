/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef STLIT_UTILITY_H
#define STLIT_UTILITY_H

namespace stlit{

template<class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value){
	while (first!=last) {
		if (*first==value) return first;
		++first;
	}
	return last;
}

template <class ForwardIterator, class T>
ForwardIterator remove(ForwardIterator first, ForwardIterator last, const T& value){
	first = stlit::find(first, last, value);
	if (first != last)
		for(ForwardIterator i = first; ++i != last; )
			if (!(*i == value))
				*first++ = *i;
	return first;
}

template <typename T>
inline T advance (T i, int offset){
	return (i + offset);
}

template <typename T1, typename T2>
inline int distance (T1 i1, T2 i2){
	return (i2 - i1);
}

template <typename T1, typename T2>
class pair {
public:
	typedef T1		first_type;
	typedef T2		second_type;
public:
	/// Default constructor.
	inline pair (void)				: first (T1()), second (T2()) {}
	/// Initializes members with \p a, and \p b.
	inline pair (const T1& a, const T2& b)	: first (a), second (b) {}
	//inline pair&	operator= (const pair<T1, T2>& p2) { first = p2.first; second = p2.second; return (*this); }
	template <typename T3, typename T4>
	inline pair&	operator= (const pair<T3, T4>& p2) { first = p2.first; second = p2.second; return (*this); }
public:
	first_type		first;
	second_type		second;
};

/// Compares both values of \p p1 to those of \p p2.
template <typename T1, typename T2>
inline bool operator== (const pair<T1,T2>& p1, const pair<T1,T2>& p2){
	return (p1.first == p2.first && p1.second == p2.second);
}

/// Compares both values of \p p1 to those of \p p2.
template <typename T1, typename T2>
bool operator< (const pair<T1,T2>& p1, const pair<T1,T2>& p2){
	return (p1.first < p2.first || (p1.first == p2.first && p1.second < p2.second));
}

/// Returns a pair object with (a,b)
template <typename T1, typename T2>
inline pair<T1,T2> make_pair (const T1& a, const T2& b){
	return (pair<T1,T2> (a, b));
}

}

#endif
