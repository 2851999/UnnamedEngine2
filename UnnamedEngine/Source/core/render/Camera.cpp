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

#include "Camera.h"

#include "Renderer.h"
#include "../../utils/Utils.h"

 /*****************************************************************************
  * The Camera class
  *****************************************************************************/

Camera::Camera() {
	//Create the descriptor set for per-camera information
	descriptorSet = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_CAMERA));
	descriptorSet->setup();
}

Camera::~Camera() {
	//Destroy the descriptor set
	delete descriptorSet;
}

void Camera::updateUBO() {
	//Update the UBO
	descriptorSet->getShaderBuffer(0)->update(&shaderData, 0, sizeof(ShaderBlock_Camera));
}

void Camera::useView() {
	//Bind the descriptor set
	descriptorSet->bind();
}

/*****************************************************************************
 * The Camera2D class
 *****************************************************************************/

void Camera2D::update() {
	//Setup the view matrix
	getViewMatrix().setIdentity();
//	Vector3f pos = getPosition();
//	pos = Vector2f(utils_maths::roundNearest(pos.getX(), 0.1f), utils_maths::roundNearest(pos.getY(), 0.1f));
	getViewMatrix().transformR(getPosition() * -1, getRotation(), getScale()); //Seems to go opposite direction as expected

	//Update the position
	shaderData.ue_cameraPosition = Vector4f(getPosition(), 0.0f);

	//Update the UBO
	updateUBO();
}

/*****************************************************************************
 * The Camera3D class
 *****************************************************************************/

Camera3D::~Camera3D() {
	if (skyBox)
		delete skyBox;
}

void Camera3D::update() {
	cameraFront = getTransform()->getRotation().getForward();
	cameraRight = getTransform()->getRotation().getRight();
	Vector3f up = getTransform()->getRotation().getUp();

	getViewMatrix().initLookAt(getPosition(), getPosition() + cameraFront, up);

	frustum.update(getProjectionViewMatrix());

	//Update the SkyBox if there is one
	if (skyBox)
		skyBox->update(getPosition());

	//Update the position
	shaderData.ue_cameraPosition = Vector4f(getPosition(), 0.0f);

	//Update the UBO
	updateUBO();
}

void Camera3D::render() {
	if (skyBox) {
		skyBox->getGraphicsPipeline()->bind();
		useView();
		skyBox->render(false);
	}
}
