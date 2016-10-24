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

#ifndef EXAMPLES_BASIC_BASICSETUP_H_
#define EXAMPLES_BASIC_BASICSETUP_H_

/*****************************************************************************
 * The Basic Rendering Example Program
 *
 * Tutorial Number: 3
 * Tutorial Name: Basic Rendering
 * Engine Version: V0.1.1 (0::1::3::2)
 * Date Created: 17/10/2016
 * Date Updated: 19/10/2016
 *
 * Description: Demonstrates how to start rendering in 2D
 *****************************************************************************/

/* Include all of the headers needed */
#include "../../core/BaseEngine.h"
#include "../../core/render/Renderer.h"
#include "../../utils/GLUtils.h"

class Tutorial : public BaseEngine {
private:
	/* Here is a container class for a 2D game object which can handle
	 * Mesh instances for rendering */
	GameObject2D* object;
	/* We also need a camera to be able to view anything */
	Camera2D* camera;
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
	getSettings().windowTitle = "Tutorial 3 - Basic Rendering";
	getSettings().videoResolution = VideoResolution::RES_1280x720;
	getSettings().debuggingShowInformation = true;
}

void Tutorial::created() {
	//Now to create the 2D object

	//Firstly there needs to be a mesh to render, for this example we
	//will make one ourselves using the MeshBuilder
	Mesh* mesh = new Mesh(MeshBuilder::createQuad(100, 100));

	//Now we can change the colour of the mesh, or modify other
	//material properties
	mesh->getMaterial()->setDiffuseColour(Colour::ORANGE);

	//Now create the object instance, passing the mesh, and the shader to
	//use when rendering it (in this case the 'Material' shader)
	object = new GameObject2D(mesh, "Material", 100, 100);

	//We can also change the material using the game object:
	object->getMaterial()->setDiffuseColour(Colour::RED);

	//But when using more complex objects with multiple meshes, this will
	//only assign the material of the first mesh added, so will only work
	//properly in a case such as this where there is only one mesh

	//The last two parameters above are to assign the width and height values of the
	//object, which ensures that rotations performed on it will always occur about
	//it's centre due to the way the MeshBuilder creates the mesh using window
	//coordinates

	//Now we can change various properties of the object
	object->setPosition(400, 200);

	//All GameObject's have to have update() called on them at least once
	//to setup their model matrix for rendering, otherwise they wont show
	//in this case this call is not needed here as it is called once per
	//frame in update
	//object->update();

	//Now to create the camera, with an orphographic projection matrix - generally this
	//is used for 2D rendering, whereas perspective would be for 3D rendering
	//In this case there is a constructor to build this matrix for us, using the left,
	//bottom, zNear and zFar values as shown below
	camera = new Camera2D(getSettings().windowWidth, getSettings().windowHeight, -1.0f, 1.0f);
	//All cameras also have view matrix that needs to be set up, so update() should also
	//be called on them, in this case it is not updated later as it doesn't move so it
	//can simply be updated once here
	camera->update();

	//In order to see any thing, the renderer needs to have access to a camera, so
	//we simply pass the camera above to the renderer
	Renderer::addCamera(camera);

	//There is also a method to remove a camera, which removes the last added camera:
	//Renderer::removeCamera();
	//The last camera added is the one used when rendering
}

void Tutorial::update() {
	//This rotates the object by a certain amount equal to 5 degrees per second
	//getDeltaSeconds() returns the time between the current frame, and the
	//last frame in seconds
	object->getRelRotation() += 5.0f * getDeltaSeconds();
	//In general any getRel methods return a reference to the value in question
	//allowing them to be directly changed - also this is probably better since
	//using:
	//object->setRotation(object->getRotation() + 1.0f * getDeltaSeconds);
	//Will only work when the object is not attached to anything else that is
	//rotation as getRotation() will return the total rotation of the object in
	//this case

	//Now to update the object's model matrix so the changes to it's rotation
	//become visible the next time it is rendered
	object->update();
}

void Tutorial::render() {
	GLUtils::setupSimple2DView();

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
		object->getMaterial()->setDiffuseColour(Colour::RED);
	else if (key == GLFW_KEY_O)
		object->getMaterial()->setDiffuseColour(Colour::ORANGE);
	else if (key == GLFW_KEY_Q)
		object->getMaterial()->setDiffuseColour(Colour(1.0, 0.5f, 0.5f));
}

#endif /* EXAMPLES_BASIC_BASICSETUP_H_ */
