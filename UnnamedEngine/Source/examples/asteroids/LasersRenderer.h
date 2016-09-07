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

#ifndef EXAMPLES_ASTEROIDS_LASERSRENDERER_H_
#define EXAMPLES_ASTEROIDS_LASERSRENDERER_H_

#include "../../core/Matrix.h"
#include "../../core/ResourceLoader.h"
#include "../../core/render/RenderData.h"

/*****************************************************************************
 * The LasersRenderer class attempts to instance render a lot of asteroids
 *****************************************************************************/

class LasersRenderer {
private:
	/* The render data instance */
	RenderData* renderData;

	/* The various VBO's needed for rendering */
	VBO<GLfloat>* vboVerticesData;
	VBO<GLfloat>* vboMatricesData;
	VBO<GLfloat>* vboVisibleData;
	VBO<unsigned int>* vboIndices;

	std::vector<Matrix4f> matricesData;
	std::vector<GLfloat> matricesDataRaw;
	std::vector<GLfloat> visibleData;

	/* The shader used for rendering */
	Shader* shader;

	/* The mesh used for rendering */
	Mesh* mesh;

	/* The number of objects */
	unsigned int numObjects;

	/* The game objects who's position/rotation/scale data will be used for rendering */
	std::vector<GameObject3D*> objects;
public:
	/* The constructor */
	LasersRenderer(const ResourceLoader& loader, unsigned int numObjects);

	/* The destructor */
	virtual ~LasersRenderer();

	/* The method used to add a laser */
	inline void addLaser(GameObject3D* object) { objects.push_back(object); }

	/* The method used to show/hide a laser */
	void showLaser(unsigned int index);
	void hideLaser(unsigned int index);

	/* Method used to check whether a laser is visible */
	inline bool isLaserVisible(unsigned int index) { return visibleData[index] > 0.5f; }

	/* Method called to update the lasers */
	void update();
	/* Method called to render the lasers */
	void render();
};

#endif /* EXAMPLES_ASTEROIDS_LASERSRENDERER_H_ */
