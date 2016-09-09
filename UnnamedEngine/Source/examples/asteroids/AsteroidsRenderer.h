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

#ifndef EXAMPLES_ASTEROIDS_ASTEROIDSRENDERER_H_
#define EXAMPLES_ASTEROIDS_ASTEROIDSRENDERER_H_

#include "../../core/Matrix.h"
#include "../../core/ResourceLoader.h"
#include "../../core/render/RenderData.h"

/*****************************************************************************
 * The AsteroidsRenderer class attempts to instance render a lot of asteroids
 *****************************************************************************/

class AsteroidsRenderer {
private:
	/* The render data instance */
	RenderData* renderData;

	/* The various VBO's needed for rendering */
	VBO<GLfloat>* vboVerticesData; //Vertices/TextureCoordinates/Normals/Tangents/Bitangents
	VBO<GLfloat>* vboMatricesData;
	VBO<GLfloat>* vboNormalMatricesData;
	VBO<GLfloat>* vboVisibleData;
	VBO<unsigned int>* vboIndices;

	std::vector<Matrix4f> matricesData;
	std::vector<GLfloat> matricesDataRaw;
	std::vector<Matrix3f> normalMatricesData;
	std::vector<GLfloat> normalMatricesDataRaw;
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
	AsteroidsRenderer(const ResourceLoader& loader, unsigned int numObjects);

	/* The destructor */
	virtual ~AsteroidsRenderer();

	/* The method used to add an asteroid */
	inline void addAsteroid(GameObject3D* object) { objects.push_back(object); }

	/* The method used to hide an asteroid */
	void hideAsteroid(unsigned int index);

	/* Method used to check whether an asteroid is visible */
	inline bool isAsteroidVisible(unsigned int index) { return visibleData[index] > 0.5f; }

	/* Method called to update the asteroids */
	void update();
	/* Method called to render the asteroids */
	void render();

	/* Method called to show all of the asteroids */
	void showAll();

	/* Method called to hide all of the asteroids */
	void hideAll();
};

#endif /* EXAMPLES_ASTEROIDS_ASTEROIDSRENDERER_H_ */
