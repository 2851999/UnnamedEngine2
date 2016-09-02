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

#ifndef CORE_RESOURCE_H_
#define CORE_RESOURCE_H_

#include <vector>

class Resource;

/*****************************************************************************
 * The ResourceManager class is used to manage the resources defined below
 *****************************************************************************/

class ResourceManager {
private:
	static std::vector<Resource*> resources;
public:
	static void add(Resource* resource);
	static void remove(Resource* resource);

	static void destroyAll();
};

/*****************************************************************************
 * The Resource class is inherited from to allow data to be managed and
 * deleted when the Engine stops
 *****************************************************************************/

class Resource {
public:
	Resource() { ResourceManager::add(this); }
	virtual ~Resource() { destroy(); }

	virtual void destroy() {}
};

#endif /* CORE_RESOURCE_H_ */
