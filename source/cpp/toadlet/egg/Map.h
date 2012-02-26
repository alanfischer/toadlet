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

#ifndef TOADLET_EGG_MAP_H
#define TOADLET_EGG_MAP_H

#include <toadlet/egg/Collection.h>

namespace toadlet{
namespace egg{

template <typename T>
inline T advance (T i, int offset){
	return (i + offset);
}

template <typename T1, typename T2>
inline int distance (T1 i1, T2 i2){
	return (i2 - i1);
}

template <typename T1, typename T2>
class Pair {
public:
	typedef T1		first_type;
	typedef T2		second_type;
public:
	/// Default constructor.
	inline Pair (void)				: first (T1()), second (T2()) {}
	/// Initializes members with \p a, and \p b.
	inline Pair (const T1& a, const T2& b)	: first (a), second (b) {}
	//inline Pair&	operator= (const Pair<T1, T2>& p2) { first = p2.first; second = p2.second; return (*this); }
	template <typename T3, typename T4>
	inline Pair&	operator= (const Pair<T3, T4>& p2) { first = p2.first; second = p2.second; return (*this); }
public:
	first_type		first;
	second_type		second;
};

/// Compares both values of \p p1 to those of \p p2.
template <typename T1, typename T2>
inline bool operator== (const Pair<T1,T2>& p1, const Pair<T1,T2>& p2){
	return (p1.first == p2.first && p1.second == p2.second);
}

/// Compares both values of \p p1 to those of \p p2.
template <typename T1, typename T2>
bool operator< (const Pair<T1,T2>& p1, const Pair<T1,T2>& p2){
	return (p1.first < p2.first || (p1.first == p2.first && p1.second < p2.second));
}

/// Returns a pair object with (a,b)
template <typename T1, typename T2>
inline Pair<T1,T2> make_pair (const T1& a, const T2& b){
	return (Pair<T1,T2> (a, b));
}

template <typename K, typename V>
class TOADLET_API Map : public Collection<Pair<K,V> > {
public:
	TOADLET_SPTR(Map);

	typedef K										key_type;
	typedef V										data_type;
	typedef const K&								const_key_ref;
	typedef const V&								const_data_ref;
	typedef const Map<K,V>&							rcself_t;
	typedef Collection<Pair<K,V> >					base_class;
	typedef Pair<K,V>								reference;
	typedef Pair<K,V>								const_reference;
	typedef typename base_class::const_iterator		const_iterator;
	typedef typename base_class::iterator			iterator;
	typedef Pair<const_iterator,const_iterator>		const_range_t;
	typedef Pair<iterator,iterator>					range_t;
public:
	inline			Map (void)			: Collection<Pair<K,V> > () {}
	explicit inline		Map (int n)		: Collection<Pair<K,V> > (n) {}
	inline			Map (rcself_t v)		: Collection<Pair<K,V> > (v) {}
	inline			Map (const_iterator i1, const_iterator i2) : Collection<Pair<K,V> >() { insert (i1, i2); }
	inline rcself_t		operator= (rcself_t v)		{ base_class::operator= (v); return (*this); }
	inline const_data_ref	operator[] (const_key_ref i) const;
	data_type&			operator[] (const_key_ref i);
	inline int		size (void) const		{ return (base_class::size()); }
	inline iterator		begin (void)			{ return (base_class::begin()); }
	inline const_iterator	begin (void) const		{ return (base_class::begin()); }
	inline iterator		end (void)			{ return (base_class::end()); }
	inline const_iterator	end (void) const		{ return (base_class::end()); }
	inline void			assign (const_iterator i1, const_iterator i2)	{ clear(); insert (i1, i2); }
	inline void			push_back (const_reference v)			{ insert (v); }
	inline const_iterator	find (const_key_ref k) const;
	inline iterator		find (const_key_ref k)	{ return (const_cast<iterator> (const_cast<rcself_t>(*this).find (k))); }
	inline const_iterator	find_data (const_data_ref v, const_iterator first = NULL, const_iterator last = NULL) const;
	inline iterator		find_data (const_data_ref v, iterator first = NULL, iterator last = NULL);
	iterator			insert (const_reference v);
	void			insert (const_iterator i1, const_iterator i2);
	inline void			erase (const_key_ref k);
	inline iterator		erase (iterator ep)	{ return (base_class::erase (ep)); }
	inline iterator		erase (iterator ep1, iterator ep2) { return (base_class::erase (ep1, ep2)); }
	inline void			clear (void)		{ base_class::clear(); }
	inline void add(const_key_ref key,const_data_ref value);
private:
	const_iterator		lower_bound (const_key_ref k) const;
	inline iterator		lower_bound (const_key_ref k) { return (const_cast<iterator>(const_cast<rcself_t>(*this).lower_bound (k))); }
};

template <typename K, typename V>
typename Map<K,V>::const_iterator Map<K,V>::lower_bound (const_key_ref k) const{
	const_iterator first (begin()), last (end());
	while (first != last) {
	const_iterator mid = advance (first, distance (first,last) / 2);
	if (mid->first < k)
		first = advance (mid, 1);
	else
		last = mid;
	}
	return (first);
}

/// Returns the Pair<K,V> where K = \p k.
template <typename K, typename V>
inline typename Map<K,V>::const_iterator Map<K,V>::find (const_key_ref k) const{
	const_iterator i = lower_bound (k);
	return ((i < end() && k < i->first) ? end() : i);
}

/// Returns the Pair<K,V> where V = \p v, occuring in range [first,last).
template <typename K, typename V>
inline typename Map<K,V>::const_iterator Map<K,V>::find_data (const_data_ref v, const_iterator first, const_iterator last) const{
	if (!first) first = begin();
	if (!last) last = end();
	for (; first != last && first->second != v; ++first);
	return (first);
}

/// Returns the Pair<K,V> where V = \p v, occuring in range [first,last).
template <typename K, typename V>
inline typename Map<K,V>::iterator Map<K,V>::find_data (const_data_ref v, iterator first, iterator last){
	return (const_cast<iterator> (find_data (v, const_cast<const_iterator>(first), const_cast<const_iterator>(last))));
}

/// Returns data associated with key \p k.
template <typename K, typename V>
inline const typename Map<K,V>::data_type& Map<K,V>::operator[] (const_key_ref k) const{
	TOADLET_ASSERT (find(k) != end() && "operator[] const can not insert non-existent keys");
	return (find(k)->second);
}

/// Returns data associated with key \p k.
template <typename K, typename V>
typename Map<K,V>::data_type& Map<K,V>::operator[] (const_key_ref k){
	iterator ip = lower_bound (k);
	if (ip == end() || k < ip->first)
	ip = base_class::insert (ip, make_pair (k, V()));
	return (ip->second);
}

template <typename K, typename V>
inline void Map<K,V>::add(const_key_ref k,const_data_ref v){
	iterator ip = lower_bound (k);
	if (ip == end() || k < ip->first)
	ip = base_class::insert (ip, make_pair (k, V()));
	ip->second=v;
}

/// Inserts the Pair into the container.
template <typename K, typename V>
typename Map<K,V>::iterator Map<K,V>::insert (const_reference v){
	iterator ip = lower_bound (v.first);
	if (ip == end() || v.first < ip->first)
	ip = base_class::insert (ip, v);
	else
	*ip = v;
	return (ip);
}

/// Inserts elements from range [i1,i2) into the container.
template <typename K, typename V>
void Map<K,V>::insert (const_iterator i1, const_iterator i2){
	TOADLET_ASSERT (i1 <= i2);
	reserve (size() + distance (i1, i2));
	for (; i1 != i2; ++i1)
	insert (*i1);
}

/// Erases the element with key value \p k.
template <typename K, typename V>
inline void Map<K,V>::erase (const_key_ref k){
	iterator ip = find (k);
	if (ip != end())
	erase (ip);
}

}
}

#endif
