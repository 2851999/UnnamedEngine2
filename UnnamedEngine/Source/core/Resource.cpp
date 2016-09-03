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

#include <algorithm>

#include "Resource.h"
#include "../utils/Logging.h"

/*****************************************************************************
 * The ResourceManager class
 *****************************************************************************/

std::vector<ResourceManager*> ResourceManager::currentManagers;

void ResourceManager::add(Resource* resource) {
	resources.push_back(resource);
}

void ResourceManager::remove(Resource* resource) {
	resources.erase(std::remove(resources.begin(), resources.end(), resource), resources.end());
}

void ResourceManager::destroyAll() {
	for (unsigned int i = 0; i < resources.size(); i++)
		delete resources[i];
	resources.clear();
}


void ResourceManager::removeResourceManager() {
	if (currentManagers.size() > 0)
		currentManagers.pop_back();
}

void ResourceManager::destroyAllManagers() {
	for (unsigned int i = 0; i < currentManagers.size(); i++)
		delete currentManagers[i];
	currentManagers.clear();
}

ResourceManager* ResourceManager::getCurrent() {
	if (currentManagers.size() > 0)
		return currentManagers[currentManagers.size() - 1];
	else {
		Logger::log("No ResourceManager's added", "ResourceManager", Logger::Error);
		return NULL;
	}
}
