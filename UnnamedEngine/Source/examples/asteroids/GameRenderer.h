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

#ifndef EXAMPLES_ASTEROIDS_GAMERENDERER_H_
#define EXAMPLES_ASTEROIDS_GAMERENDERER_H_

#include "../../core/render/RenderData.h"
#include "../../core/render/Renderer.h"

/*****************************************************************************
 * The GameRenderer class attempts to instance render stuff for the game
 *****************************************************************************/

class GameRenderer {
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

	/* The mesh used for rendering */
	Mesh* mesh;

	/* The shader used for rendering */
	Shader* shader;

	/* The number of objects */
	unsigned int numObjects;

	/* The game objects who's position/rotation/scale data will be used for rendering */
	std::vector<GameObject3D*> objects;

	/* States whether lighting should be used */
	bool useLighting;
public:
	/* The constructor */
	GameRenderer(Mesh* mesh, Shader* shader, unsigned int numObjects, bool useTextureCoords, bool useLighting, bool useNormalMapping);

	/* The destructor */
	virtual ~GameRenderer();

	/* The method used to add an object */
	inline void add(GameObject3D* object) { objects.push_back(object); }

	/* The method used to hide an object */
	void show(unsigned int index);
	void hide(unsigned int index);

	/* Method used to check whether an object is visible */
	inline bool isVisible(unsigned int index) { return visibleData[index] > 0.5f; }

	/* Method called to update a group of the objects */
	void update(unsigned int startIndex, unsigned int endIndex);
	/* Method called to update all of the objects */
	inline void updateAll() { update(0, objects.size()); }
	/* Method called to render the objects */
	void render();

	/* Method called to show all of the objects */
	void showAll();

	/* Method called to hide all of the objects */
	void hideAll();
};

#endif /* EXAMPLES_ASTEROIDS_GAMERENDERER_H_ */
