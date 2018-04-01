/*****************************************************************************
 *
 *   Copyright 2018 Joel Davies
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

#include "EquiToCube.h"
#include "../../core/Window.h"
#include "../../core/render/Renderer.h"
#include "../../core/render/Shader.h"

void EquiToCube::generateCubemapAndIrradiance(std::string path, unsigned int &cubemap, unsigned int &irMap) {
	RenderShader* renderShader1 = new RenderShader("EquiToCube", Renderer::loadEngineShader("EquiToCube"), NULL);
	Shader* shader1 = renderShader1->getForwardShader();
	RenderShader* renderShader2 = new RenderShader("Irradiance", Renderer::loadEngineShader("Irradiance"), NULL);
	Shader* shader2 = renderShader2->getForwardShader();
	MeshRenderData* cubeMesh = new MeshRenderData(MeshBuilder::createCube(1.0f, 1.0f, 1.0f), renderShader1);

	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	HDRTexture* texture = new HDRTexture(path);

	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; i++) {
	    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Matrix4f captureProjection = Matrix4f().initPerspective(90.0f, 1.0f, 0.1f, 10.0f);
	Matrix4f captureViews[] = {
	        /*Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 1.0f,  0.0f,  0.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-1.0f,  0.0f,  0.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  1.0f,  0.0f), Vector3f(0.0f,  0.0f,  1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f, -1.0f,  0.0f), Vector3f(0.0f,  0.0f, -1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  0.0f,  1.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  0.0f, -1.0f), Vector3f(0.0f, -1.0f,  0.0f)) */ // UPSIDE DOWN

	        Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 1.0f,  0.0f,  0.0f), Vector3f(0.0f, 1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-1.0f,  0.0f,  0.0f), Vector3f(0.0f, 1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f, -1.0f,  0.0f), Vector3f(0.0f, 0.0f, -1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  1.0f,  0.0f), Vector3f(0.0f, 0.0f,  1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  0.0f, -1.0f), Vector3f(0.0f, 1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  0.0f,  1.0f), Vector3f(0.0f, 1.0f,  0.0f))
	};

	shader1->use();
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->getHandle());
	shader1->setUniformi("EquiMap", 0);
	shader1->setUniformMatrix4("ProjectionMatrix", captureProjection);

	glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i) {
        shader1->setUniformMatrix4("ViewMatrix", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeMesh->render();
    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shader1->stopUsing();

	unsigned int irradianceMap;
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; i++) {
	    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    shader2->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	shader2->setUniformi("EnvMap", 0);
	shader2->setUniformMatrix4("ProjectionMatrix", captureProjection);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; i++) {
		shader2->setUniformMatrix4("ViewMatrix", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeMesh->render();
	}

	shader2->stopUsing();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

//	glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//
//    //Renderer::unbindTexture();
//
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glBindRenderbuffer(GL_RENDERBUFFER, 0);
//
//	glDeleteFramebuffers(1, &captureFBO);
//	glDeleteRenderbuffers(1, &captureRBO);

	//Reset the view port to the default one
	glViewport(0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight);

	cubemap = envCubemap;
	irMap = irradianceMap;
}

Cubemap* EquiToCube::generateIrradianceMap(Cubemap* cubeMap) {
	RenderShader* renderShader = new RenderShader("Irradiance", Renderer::loadEngineShader("Irradiance"), NULL);
	Shader* shader = renderShader->getForwardShader();
	MeshRenderData* cubeMesh = new MeshRenderData(MeshBuilder::createCube(1.0f, 1.0f, 1.0f), renderShader);

	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	unsigned int irradianceMap;
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; i++) {
	    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Matrix4f captureProjection = Matrix4f().initPerspective(90.0f, 1.0f, 0.1f, 10.0f);
	Matrix4f captureViews[] = {
	        /*Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 1.0f,  0.0f,  0.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-1.0f,  0.0f,  0.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  1.0f,  0.0f), Vector3f(0.0f,  0.0f,  1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f, -1.0f,  0.0f), Vector3f(0.0f,  0.0f, -1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  0.0f,  1.0f), Vector3f(0.0f, -1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  0.0f, -1.0f), Vector3f(0.0f, -1.0f,  0.0f)) */ // UPSIDE DOWN

	        Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 1.0f,  0.0f,  0.0f), Vector3f(0.0f, 1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-1.0f,  0.0f,  0.0f), Vector3f(0.0f, 1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f, -1.0f,  0.0f), Vector3f(0.0f, 0.0f, -1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  1.0f,  0.0f), Vector3f(0.0f, 0.0f,  1.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  0.0f, -1.0f), Vector3f(0.0f, 1.0f,  0.0f)),
			Matrix4f().initLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f( 0.0f,  0.0f,  1.0f), Vector3f(0.0f, 1.0f,  0.0f))
	};

	shader->use();
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap->getHandle());
	shader->setUniformi("EnvMap", 0);
	shader->setUniformMatrix4("ProjectionMatrix", captureProjection);

	glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; i++) {
        shader->setUniformMatrix4("ViewMatrix", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeMesh->render();
    }

    shader->stopUsing();

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    //Renderer::unbindTexture();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glDeleteFramebuffers(1, &captureFBO);
	glDeleteRenderbuffers(1, &captureRBO);

	//Reset the view port to the default one
	glViewport(0, 0, Window::getCurrentInstance()->getSettings().windowWidth, Window::getCurrentInstance()->getSettings().windowHeight);

    return new Cubemap(irradianceMap);
}

