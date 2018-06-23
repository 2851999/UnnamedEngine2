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

#ifndef EXAMPLES_BASIC_BASICRENDERING3D_H_
#define EXAMPLES_BASIC_BASICRENDERING3D_H_

/*****************************************************************************
 * The Basic Rendering Example Program
 *
 * Tutorial Number: 4
 * Tutorial Name: Basic Rendering 3D
 * Engine Version: V0.2.9 (1::0::6::4)
 * Date Created: 13/04/2018
 * Date Updated: 13/04/2018
 *
 * Description: Demonstrates how to start rendering in 3D
 *****************************************************************************/

#include "../../core/BaseEngine.h"
#include "../../core/render/Renderer.h"
#include "../../utils/GLUtils.h"

class Tutorial : public BaseEngine {
private:
	/* This is exactly the same as before, just now using the 3D versions */
	GameObject3D* object;
	Camera3D* camera;
public:
	void initialise() override;
	void created() override;
	void update() override;
	void render() override;
	void destroy() override;

	void onKeyPressed(int key) override;
};

void Tutorial::initialise() {
	//Assign some Settings
	getSettings().windowTitle = "Tutorial 4 - Basic Rendering 3D";
	getSettings().videoResolution = VideoResolution::RES_1280x720;
	getSettings().debugShowInformation = true;

	//Apply antialiasing (MSAA) to produce smooth edges, this value can typically
	//range between 0 (for no MSAA) and 16 (for 16x MSAA)
	getSettings().videoSamples = 16;
}

void Tutorial::created() {
	//Now to create the 2D object

	//This time make a cube with a size of 1 (unlike 2D the sizes do not directly
	//correspond to the number of pixels on the screen
	Mesh* mesh = new Mesh(MeshBuilder::createCube(1.0f, 1.0f, 1.0f));

	mesh->getMaterial()->diffuseColour = Colour::ORANGE;

	//Frustum culling can be applied on a mesh per mesh basis, and by default it will be
	//enabled, however it can be disabled using the following
	mesh->setCullingEnabled(false);

	//Now create the object instance
	object = new GameObject3D(mesh, Renderer::SHADER_MATERIAL, 1.0f, 1.0f, 1.0f);
	//Since this is in 3D, the size is not really necessary since it isn't needed for
	//rotations

	//The last three parameters above are to assign the width, height and depth values
	//of the object

	//Now we can change various properties of the object

	//Place the object deeper into the screen than the camera (which will be at (0, 0, 0))
	object->setPosition(0.0f, 0.0f, -2.0f);

	//Now to create the camera, with an perspective projection matrix
	//In this case there is a constructor to build this matrix for us, using the fov
	//(field of view) given in degree, aspect ratio of the screen and zNear and zFar
	//values
	camera = new Camera3D(90.0f, getSettings().windowAspectRatio, 0.1f, 100.0f);
	camera->update();

	Renderer::addCamera(camera);
}

void Tutorial::update() {
	//This rotates the object by a certain amount equal to 10 degrees per second
	//In 3D however there are 3 rotation axis (x, y and z) so this will actually rotate
	//along all 3
	//object->setRotation(object->getLocalRotationEuler() + Vector3f(10.0f * getDeltaSeconds()));

	//Alternatively the object can be rotated along a standard axis using quaternions like so
	object->getTransform()->rotate(object->getTransform()->getRotation().getUp(), 10.0f * getDeltaSeconds());
	//This rotates along the "up" direction, i.e the direction of the top of the object when no rotation is applied
	//this will rotate it clockwise around this axis

	//The transform of an object stores all of the data for transforming an object using a model matrix
	//and can be used to rotate, scale and translate the model

	//Now to update the object
	object->update();
}

void Tutorial::render() {
	//This is the same as the 2D variant but also handles clearing the depth buffer and
	//depth testing
	utils_gl::setupSimple3DView();

	//Now render the object
	object->render();
}

void Tutorial::destroy() {
	//Delete the created resources
	delete object;
}

void Tutorial::onKeyPressed(int key) {
	//Check the key
	if (key == GLFW_KEY_R)
		object->getMaterial()->diffuseColour = Colour::RED;
	else if (key == GLFW_KEY_O)
		object->getMaterial()->diffuseColour = Colour::ORANGE;
	else if (key == GLFW_KEY_Q)
		object->getMaterial()->diffuseColour = Colour(1.0, 0.5f, 0.5f);
}

#endif /* EXAMPLES_BASIC_BASICRENDERING3D_H_ */
