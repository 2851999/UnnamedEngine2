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
 * The Resources class is used to manage the resources defined below
 *****************************************************************************/

class ResourceManager {
private:
	/* The current resource managers - the last one added will be the one new
	 * resources will be added to */
	static std::vector<ResourceManager*> currentManagers;

	/* All of the resources added to this resource manager */
	std::vector<Resource*> resources;
public:
	/* The constructor */
	ResourceManager() {}

	/* The destructor */
	virtual ~ResourceManager() { destroyAll(); }

	/* Methods used to add/remove resources */
	void add(Resource* resource);
	void remove(Resource* resource);

	/* Method used to delete all added resources */
	void destroyAll();

	/* Method used to add a resource manager */
	static inline void addResourceManager(ResourceManager* manager) { currentManagers.push_back(manager); }

	/* Method used to remove a resource manager */
	static void removeResourceManager();

	/* Method used to release destroy all of the resource managers */
	static void destroyAllManagers();

	/* Method used to get the active resource manager */
	static ResourceManager* getCurrent();
};

/*****************************************************************************
 * The Resource class is inherited from to allow data to be managed
 *****************************************************************************/
#include <iostream>
class Resource {
private:
	/* Pointer to the resource manager this is loaded in */
	ResourceManager* manager;
public:
	/* The constructor */
	Resource() { manager = ResourceManager::getCurrent(); manager->add(this); }

	/* The destructor */
	virtual ~Resource() { manager->remove(this); }

	/* The method called when all resources being used should be released */
	virtual void destroy() {}
};

#endif /* CORE_RESOURCE_H_ */
