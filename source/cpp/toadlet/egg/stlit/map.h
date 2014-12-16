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

#ifndef STLIT_MAP_H
#define STLIT_MAP_H

#include <toadlet/egg/stlit/vector.h>
#include <toadlet/egg/stlit/utility.h>

namespace stlit{

template <typename K, typename V>
class map : public vector<pair<K,V> > {
public:
	typedef K key_type;
	typedef V data_type;
	typedef const K& const_key_ref;
	typedef const V& const_data_ref;
	typedef const map<K,V>& rcself_t;
	typedef vector<pair<K,V> > base_class;
	typedef pair<K,V> reference;
	typedef pair<K,V> const_reference;
	typedef typename base_class::const_iterator const_iterator;
	typedef typename base_class::iterator iterator;
	typedef pair<const_iterator,const_iterator> const_range_t;
	typedef pair<iterator,iterator> range_t;
public:
	inline map (void) : vector<pair<K,V> > () {}
	explicit inline map (int n) : vector<pair<K,V> > (n) {}
	inline map (rcself_t v) : vector<pair<K,V> > (v) {}
	inline map (const_iterator i1, const_iterator i2) : vector<pair<K,V> >() { insert (i1, i2); }
	inline rcself_t operator= (rcself_t v) { base_class::operator= (v); return (*this); }
	inline const_data_ref operator[] (const_key_ref i) const;
	data_type& operator[] (const_key_ref i);
	inline int size (void) const { return (base_class::size()); }
	inline iterator begin (void) { return (base_class::begin()); }
	inline const_iterator begin (void) const { return (base_class::begin()); }
	inline iterator end (void) { return (base_class::end()); }
	inline const_iterator end (void) const { return (base_class::end()); }
	inline void assign (const_iterator i1, const_iterator i2) { clear(); insert (i1, i2); }
	inline void push_back (const_reference v) { insert (v); }
	inline const_iterator find (const_key_ref k) const;
	inline iterator find (const_key_ref k) { return (iterator(const_cast<rcself_t>(*this).find (k))); }
	inline const_iterator find_data (const_data_ref v, const_iterator first = NULL, const_iterator last = NULL) const;
	inline iterator find_data (const_data_ref v, iterator first = NULL, iterator last = NULL);
	iterator insert (const_reference v);
	void insert (const_iterator i1, const_iterator i2);
	inline void erase (const_key_ref k);
	inline iterator erase (iterator ep)	{ return (base_class::erase (ep)); }
	inline iterator erase (iterator ep1, iterator ep2) { return (base_class::erase (ep1, ep2)); }
	inline void clear (void) { base_class::clear(); }
private:
	const_iterator lower_bound (const_key_ref k) const;
	inline iterator lower_bound (const_key_ref k) { return (iterator(const_cast<rcself_t>(*this).lower_bound (k))); }
};

template <typename K, typename V>
typename map<K,V>::const_iterator map<K,V>::lower_bound (const_key_ref k) const{
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

/// Returns the pair<K,V> where K = \p k.
template <typename K, typename V>
inline typename map<K,V>::const_iterator map<K,V>::find (const_key_ref k) const{
	const_iterator i = lower_bound (k);
	return ((i < end() && k < i->first) ? end() : i);
}

/// Returns the pair<K,V> where V = \p v, occuring in range [first,last).
template <typename K, typename V>
inline typename map<K,V>::const_iterator map<K,V>::find_data (const_data_ref v, const_iterator first, const_iterator last) const{
	if (!first) first = begin();
	if (!last) last = end();
	for (; first != last && first->second != v; ++first);
	return (first);
}

/// Returns the pair<K,V> where V = \p v, occuring in range [first,last).
template <typename K, typename V>
inline typename map<K,V>::iterator map<K,V>::find_data (const_data_ref v, iterator first, iterator last){
	return (const_cast<iterator> (find_data (v, const_cast<const_iterator>(first), const_cast<const_iterator>(last))));
}

/// Returns data associated with key \p k.
template <typename K, typename V>
inline const typename map<K,V>::data_type& map<K,V>::operator[] (const_key_ref k) const{
	TOADLET_ASSERT (find(k) != end() && "operator[] const can not insert non-existent keys");
	return (find(k)->second);
}

/// Returns data associated with key \p k.
template <typename K, typename V>
typename map<K,V>::data_type& map<K,V>::operator[] (const_key_ref k){
	iterator ip = lower_bound (k);
	if (ip == end() || k < ip->first)
	ip = base_class::insert (ip, make_pair (k, V()));
	return (ip->second);
}

/// Inserts the pair into the container.
template <typename K, typename V>
typename map<K,V>::iterator map<K,V>::insert (const_reference v){
	iterator ip = lower_bound (v.first);
	if (ip == end() || v.first < ip->first)
	ip = base_class::insert (ip, v);
	else
	*ip = v;
	return (ip);
}

/// Inserts elements from range [i1,i2) into the container.
template <typename K, typename V>
void map<K,V>::insert (const_iterator i1, const_iterator i2){
	TOADLET_ASSERT (i1 <= i2);
	reserve (size() + distance (i1, i2));
	for (; i1 != i2; ++i1)
	insert (*i1);
}

/// Erases the element with key value \p k.
template <typename K, typename V>
inline void map<K,V>::erase (const_key_ref k){
	iterator ip = find (k);
	if (ip != end())
	erase (ip);
}

}

#endif
