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

#pragma once

#include "../Frustum.h"
#include "SkyBox.h"
#include "../Object.h"

/*****************************************************************************
 * The Camera class contains the base functionality of a camera
 *****************************************************************************/

class Camera {
private:
	/* Descriptor set holding per-camera information */
	DescriptorSet* descriptorSet;
protected:
	/* The camera information for shaders */
	ShaderBlock_Camera shaderData;

	/* Method to update the camera data UBO for the current frame */
	void updateUBO();
public:
	Camera();
	Camera(Matrix4f projectionMatrix) : Camera() { shaderData.ue_projectionMatrix = projectionMatrix; }
	Camera(Matrix4f projectionMatrix, Matrix4f viewMatrix) : Camera() { shaderData.ue_projectionMatrix = projectionMatrix; shaderData.ue_viewMatrix = viewMatrix; }
	virtual ~Camera();

	/* Used to update the matrices */
	virtual void update() {}

	/* Used to use the view from this camera for subsequent rendering */
	virtual void useView();

	/* Setters and getters */
	void setProjectionMatrix(Matrix4f projectionMatrix) { shaderData.ue_projectionMatrix = projectionMatrix; }
	void setViewMatrix(Matrix4f viewMatrix) { shaderData.ue_viewMatrix = viewMatrix; }

	Matrix4f& getProjectionMatrix() { return shaderData.ue_projectionMatrix; }
	Matrix4f& getViewMatrix() { return shaderData.ue_viewMatrix; }
	Matrix4f getProjectionViewMatrix() { return shaderData.ue_projectionMatrix * shaderData.ue_viewMatrix; }
	DescriptorSet* getDescriptorSet() { return descriptorSet; }
};

/*****************************************************************************
 * The Camera2D class provides an implementation of a Camera for 2D
 *****************************************************************************/

class Camera2D : public Camera, public GameObject2D {
public:
	/* Various constructors */
	Camera2D() {}
	Camera2D(float left, float right, float bottom, float top, float zNear, float zFar) :
		Camera(Matrix4f().initOrthographic(left, right, bottom, top, zNear, zFar)) {}
	Camera2D(float width, float height, float zNear, float zFar) : Camera2D(0, width, height, 0, zNear, zFar) {}
	Camera2D(Matrix4f projectionMatrix) : Camera(projectionMatrix) {}
	Camera2D(Matrix4f projectionMatrix, Vector2f position) : Camera(projectionMatrix) {
		setPosition(position);
	}
	Camera2D(Matrix4f projectionMatrix, Vector2f position, float rotation) : Camera(projectionMatrix) {
		setPosition(position);
		setRotation(rotation);
	}
	Camera2D(Matrix4f projectionMatrix, Vector2f position, float rotation, Vector2f scale) : Camera(projectionMatrix) {
		setPosition(position);
		setRotation(rotation);
		setScale(scale);
	}
	virtual ~Camera2D() {}

	void update() override;
};

/*****************************************************************************
 * The Camera3D class provides an implementation of a Camera for 3D
 *****************************************************************************/

class Camera3D : public Camera, public GameObject3D {
private:
	/* The SkyBox for this camera if set */
	SkyBox* skyBox = NULL;
	/* The flying value determines whether looking up allows the camera to move upwards */
	bool flying = false;

	Vector3f cameraFront = Vector3f(0.0f, 0.0f, -1.0f);
	Vector3f cameraUp    = Vector3f(0.0f, 1.0f, 0.0f);
	Vector3f cameraRight = Vector3f(1.0f, 0.0f, 0.0f);

	Frustum frustum;
public:
	/* Various constructors */
	Camera3D() {}
	Camera3D(float fovy, float aspect, float zNear, float zFar) :
		Camera(Matrix4f().initPerspective(fovy, aspect, zNear, zFar)) {}
	Camera3D(Matrix4f projectionMatrix) : Camera(projectionMatrix) {}
	Camera3D(Matrix4f projectionMatrix, Vector3f position) : Camera(projectionMatrix) {
		setPosition(position);
	}
	Camera3D(Matrix4f projectionMatrix, Vector3f position, Vector3f rotation) : Camera(projectionMatrix) {
		setPosition(position);
		setRotation(rotation);
	}
	Camera3D(Matrix4f projectionMatrix, Vector3f position, Vector3f rotation, Vector3f scale) : Camera(projectionMatrix) {
		setProjectionMatrix(projectionMatrix);
		setPosition(position);
		setRotation(rotation);
		setScale(scale);
	}
	virtual ~Camera3D();

	void update() override;

	void useView() override;

	/* Used to render the associated skybox (if there is one) */
	void render() override;

	/* Moves the camera forward in the direction it is facing */
	inline void moveForward(float amount) {
		if (! flying) {
			Vector3f direction = cameraFront;
			direction.setY(0.0f);
			getTransform()->translate(direction * amount);
		} else
			getTransform()->translate(cameraFront * amount);
	}

	/* Moves the camera backward in the direction it is facing */
	inline void moveBackward(float amount) {
		if (! flying) {
			Vector3f direction = cameraFront;
			direction.setY(0.0f);
			getTransform()->translate(direction * -amount);
		} else
			getTransform()->translate(cameraFront * -amount);
	}

	/* Moves the camera left in the direction it is facing */
	inline void moveLeft(float amount) {
		getTransform()->translate(cameraRight.normalise() * -amount);
	}

	/* Moves the camera right in the direction it is facing */
	inline void moveRight(float amount) {
		getTransform()->translate(cameraRight.normalise() * amount);
	}

	/* The setters and getters */
	inline void setSkyBox(SkyBox* skyBox) { this->skyBox = skyBox; }
	inline void setFlying(bool flying) { this->flying = flying; }
	inline void toggleFlying() { flying = !flying; }
	inline Vector3f getFront() { return cameraFront; }
	inline Vector3f getUp() { return cameraUp; }
	inline Vector3f getRight() { return cameraRight; }
	inline SkyBox* getSkyBox() { return skyBox; }
	inline bool hasSkyBox() { return skyBox; }
	inline bool isFlying() { return flying; }

	inline Frustum& getFrustum() { return frustum; }
};

