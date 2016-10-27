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

#include "Resource.h"

#include <algorithm>

#include "../utils/Logging.h"

/*****************************************************************************
 * The ResourceManager class
 *****************************************************************************/

std::vector<ResourceManager*> ResourceManager::currentManagers;

void ResourceManager::add(Resource* resource) {
	//Add the resource
	resources.push_back(resource);
}

void ResourceManager::remove(Resource* resource) {
	//Remove the resource
	resources.erase(std::remove(resources.begin(), resources.end(), resource), resources.end());
}

void ResourceManager::destroyAll() {
	//Go through and delete all resource added to this manager
	for (unsigned int i = 0; i < resources.size(); i++)
		delete resources[i];
	resources.clear();
}

void ResourceManager::removeResourceManager() {
	//Remove the last resource manager as long as there is one
	if (currentManagers.size() > 0)
		currentManagers.pop_back();
}

void ResourceManager::destroyAllManagers() {
	//Go through and delete all resource managers
	for (unsigned int i = 0; i < currentManagers.size(); i++)
		delete currentManagers[i];
	currentManagers.clear();
}

ResourceManager* ResourceManager::getCurrent() {
	//Return a pointer to the current manager if there is one (the last one in the
	//'currentManagers' array) otherwise return null and log an error
	if (currentManagers.size() > 0)
		return currentManagers[currentManagers.size() - 1];
	else {
		Logger::log("No ResourceManager's added", "ResourceManager", LogType::Error);
		return NULL;
	}
}
