/*****************************************************************************
 *
 *   Copyright 2016 Joel Davies
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *****************************************************************************/

#ifndef UTILS_SORTING_H_
#define UTILS_SORTING_H_

#include <vector>
#include <algorithm>

namespace Sorting {
	/* Quicksort: (From Wikipedia)
	 * 1. Pick an element, called a pivot, from the array.
	 * 2. Partitioning: reorder the array so that all elements with values less than the pivot come before the pivot, while all elements with values greater than the pivot come after it (equal values can go either way). After this partitioning, the pivot is in its final position. This is called the partition operation.
     * 3. Recursively apply the above steps to the sub-array of elements with smaller values and separately to the sub-array of elements with greater values.
	 */

	/* Partitioning method to apply step 2 */
	template<typename T>
	int partition(std::vector<T>& data, unsigned int left, unsigned int right) {
		//Get the current pivot element
		T current = data[left];
		unsigned int i = left;

		//Go through all of the current elements and swap them if needed
		for (unsigned int j = left + 1; j < right; j++) {
			if (data[j] < current) {
				i++;
				std::swap(data[i], data[j]);
			}
		}

		std::swap(data[i], data[left]);
		return i;
	}

	/* Method used to apply quicksort to a vector, given the two
	 * pivot points, left and right */
	template<typename T>
	void sort(std::vector<T>& data, unsigned int left, unsigned int right) {
		//Ensure the left index is still before the right one (otherwise the algorithm
		//should stop)
		if (left < right) {
			//Sort the values, and calculate the new pivot
			int index = Sorting::partition(data, left, right);
			//Apply the algorithm on the left side of the pivot
			sort(data, left, index);
			//Apply the algorithm on the right side of the pivot
			sort(data, index + 1, right);
		}
	}

	/* Method used to apply quicksort to a vector */
	template<typename T>
	void sort(std::vector<T>& data) {
		Sorting::sort(data, 0, static_cast<unsigned int>(data.size()));
	}
}



#endif /* UTILS_SORTING_H_ */
