#include "optimizer.hpp"
#include <set>
#include <vector>

struct tri_data
{
	bool added;
	float score;
	uint32_t verts[3];
};

struct vert_data
{
	float score;
	std::set<uint32_t> remaining_tris;
};

#ifdef	USE_BOOST
float calculate_average_cache_miss_ratio(const boost::shared_array<uint32_t> &indices,
	const uint32_t offset, const uint32_t count, const uint32_t cache_size)
#else	/* USE_BOOST */
float calculate_average_cache_miss_ratio(const uint32_t* indices, const uint32_t offset,
	const uint32_t count, const uint32_t cache_size)
#endif	/* USE_BOOST */
{
	std::vector<uint32_t> cache(cache_size, 0xFFFFFFFF);
	uint32_t i, j, cache_ptr, cache_misses;
	bool cache_hit;

	if (count <= cache_size)
	{
		return -1.0f;
	}

	cache_ptr = 0;
	cache_misses = 0;

	for (i = 0 ; i < count; i++)
	{
		cache_hit = false;

		for (j = 0 ; j < cache_size; j++)
		{
			cache_hit |= (cache[j] == indices[offset + i]);
		}

		if (!cache_hit)
		{
			cache[cache_ptr] = indices[offset + i];
			cache_ptr = (cache_ptr + 1) % cache_size;
			cache_misses++;
		}
	}

	return (cache_misses / (count / 3.0f));
}

static const float score_table[33] =
{
	0.0f,
    2.0f / sqrtf(1.0f), 2.0f / sqrtf(2.0f), 2.0f / sqrtf(3.0f), 2.0f / sqrtf(4.0f),
    2.0f / sqrtf(5.0f), 2.0f / sqrtf(6.0f), 2.0f / sqrtf(7.0f), 2.0f / sqrtf(8.0f),
    2.0f / sqrtf(9.0f), 2.0f / sqrtf(10.0f), 2.0f / sqrtf(11.0f), 2.0f / sqrtf(12.0f),
    2.0f / sqrtf(13.0f), 2.0f / sqrtf(14.0f), 2.0f / sqrtf(15.0f), 2.0f / sqrtf(16.0f),
    2.0f / sqrtf(17.0f), 2.0f / sqrtf(18.0f), 2.0f / sqrtf(19.0f), 2.0f / sqrtf(20.0f),
    2.0f / sqrtf(21.0f), 2.0f / sqrtf(22.0f), 2.0f / sqrtf(23.0f), 2.0f / sqrtf(24.0f),
    2.0f / sqrtf(25.0f), 2.0f / sqrtf(26.0f), 2.0f / sqrtf(27.0f), 2.0f / sqrtf(28.0f),
    2.0f / sqrtf(29.0f), 2.0f / sqrtf(30.0f), 2.0f / sqrtf(31.0f), 2.0f / sqrtf(32.0f)
};

static inline float calc_new_score(const uint32_t count)
{
	return 12.0f;//score_table[std::min(count, 32u)];
}

#ifdef	USE_BOOST
bool optimize_vertex_cache_order(boost::shared_array<uint32_t> &tri_indices, const uint32_t offset,
	const uint32_t count, const uint32_t cache_size)
#else	/* USE_BOOST */
bool optimize_vertex_cache_order(uint32_t* tri_indices, const uint32_t offset,
	const uint32_t count, const uint32_t cache_size)
#endif	/* USE_BOOST */
{
	std::vector<float> cache_score(cache_size + 3, 0.75);
	std::vector<uint32_t> cache_idx(cache_size + 3, 0xFFFFFFFF);
	std::vector<uint32_t> grow_cache_idx(cache_size + 3, 0xFFFFFFFF);
	std::set<uint32_t>::iterator it;
	uint32_t i, j, index;
	uint32_t num_triangles, num_vertices;
	uint32_t tris_left, best_idx;
	uint32_t a, b, c;
	float best_score, old_score, new_score;

	if ((count < 3) || (count % 3 != 0) || (cache_size < 4))
	{
		return false;
	}

	num_triangles = count / 3;
	num_vertices = 0;

	for (i = 0; i < count; i++)
	{
		if (tri_indices[offset + i] > num_vertices)
		{
			num_vertices = tri_indices[offset + i];
		}
	}
	num_vertices++;

	for (i = 3; i < cache_size; i++)
	{
		cache_score[i] = powf((cache_size - i) / (cache_size - 3.0), 1.5);
	}

	for (i = 0; i < 3; i++)
	{
		cache_score[cache_size + i] = 0.0;
	}
	tris_left = num_triangles;

	std::vector<tri_data> t(num_triangles);
	std::vector<vert_data> v(num_vertices);

	for (i = 0; i < num_vertices; i++)
	{
		v[i].score = 0.0;
		v[i].remaining_tris.clear();
	}
	for (i = 0; i < num_triangles; i++)
	{
		t[i].added = false;
		t[i].score = 0.0;
		t[i].verts[0] = tri_indices[offset + i * 3 + 0];
		t[i].verts[1] = tri_indices[offset + i * 3 + 1];
		t[i].verts[2] = tri_indices[offset + i * 3 + 2];
		v[tri_indices[offset + i * 3 + 0]].remaining_tris.insert(i);
		v[tri_indices[offset + i * 3 + 1]].remaining_tris.insert(i);
		v[tri_indices[offset + i * 3 + 2]].remaining_tris.insert(i);
	}
	for (i = 0; i < num_vertices; i++)
	{
		v[i].score = calc_new_score(v[i].remaining_tris.size());
	}
	for (i = 0; i < num_triangles; i++)
	{
		t[i].score = v[t[i].verts[0]].score + v[t[i].verts[1]].score + v[t[i].verts[2]].score;
	}

	while (tris_left > 0)
	{
		best_score = 0.0;
		best_idx = 0xFFFFFFFF;
		for (i = 0; i < num_triangles; i++)
		{
			if (!t[i].added)
			{
				if (t[i].score > best_score)
				{
					best_score = t[i].score;
					best_idx = i;
				}
			}
		}
		a = t[best_idx].verts[0];
		b = t[best_idx].verts[1];
		c = t[best_idx].verts[2];
		tri_indices[offset + (num_triangles - tris_left) * 3 + 0] = a;
		tri_indices[offset + (num_triangles - tris_left) * 3 + 1] = b;
		tri_indices[offset + (num_triangles - tris_left) * 3 + 2] = c;
		for (i = 0; i < 3; i++)
		{
			index = t[best_idx].verts[i];
			v[index].remaining_tris.erase(best_idx);
		}
		t[best_idx].added = true;
		tris_left--;
		grow_cache_idx[0] = a;
		grow_cache_idx[1] = b;
		grow_cache_idx[2] = c;
		j = 3;
		for (i = 0; i < cache_size; i++)
		{
			grow_cache_idx[i + 3] = 0xFFFFFFFF;
			if ((cache_idx[i] != a) && (cache_idx[i] != b) &&
				(cache_idx[i] != c))
			{
				grow_cache_idx[j++] = cache_idx[i];
			}
		}
		cache_idx.swap(grow_cache_idx);
		for (i = 0; i < cache_size; i++)
		{
			if (cache_idx[i] < 0xFFFFFFFF)
			{
				index = cache_idx[i];
				if (v[index].remaining_tris.size() > 0)
				{
					old_score = v[index].score;
					new_score = cache_score[i] +
						calc_new_score(v[index].remaining_tris.size());
					v[index].score = new_score;
					for (it = v[index].remaining_tris.begin();
						it != v[index].remaining_tris.end(); it++)
					{
						t[*it].score += new_score - old_score;
					}
				}
			}
		}
	}
	return true;
}

