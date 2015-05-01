#ifndef	_OPTIMIZER_HPP_
#define	_OPTIMIZER_HPP_

#ifdef	USE_BOOST
#include <boost/shared_array.hpp>
#endif	/* USE_BOOST */

#include "platform.h"
#include <SDL.h>

#ifdef	USE_BOOST
float calculate_average_cache_miss_ratio(const boost::shared_array<uint32_t> &indices,
	const uint32_t offset, const uint32_t count, const uint32_t cache_size);
bool optimize_vertex_cache_order(boost::shared_array<uint32_t> &tri_indices, const uint32_t offset,
	const uint32_t count, const uint32_t cache_size);
#else	/* USE_BOOST */
float calculate_average_cache_miss_ratio(const uint32_t* indices, const uint32_t offset,
	const uint32_t count, const uint32_t cache_size);
bool optimize_vertex_cache_order(uint32_t* tri_indices, const uint32_t offset, const uint32_t count,
	const uint32_t cache_size);
#endif	/* USE_BOOST */

#endif	/* _OPTIMIZER_HPP_ */
