#ifndef MAP_COMPARERS_H
#define MAP_COMPARERS_H

#include <string>
#include <vector>
#include <cassert>

/**
* Several struct to define the operator function for
* certain std::map
*/

struct LessString {
	bool operator()(const std::string &s1, const std::string &s2) const
	{
		return s1 < s2;
	}
};

struct CompareFloatVector {
	bool operator ()(const std::vector<float> &v1, const std::vector<float> &v2) const {
		assert(v1.size()==v2.size());

		// Compare v1 and v2 and return true if v1 < v2
		for(size_t i=0; i<v1.size(); ++i) {
			if( v1[i] != v2[i] )
				return v1[i]<v2[i];
			/*
			if(v1[i]<v2[i])
				return true;
			else if (v1[i]>v2[i])
				return false;
			*/
		}

		return false;
	}
};

typedef struct ResolutionStruct Resolution;
struct CompareResolution {
	bool operator()(const Resolution &r1, const Resolution &r2) const;
};

#endif // MAP_COMPARERS_H
