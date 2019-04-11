/*
 * Copyright (c) 2019 BestSolution.at and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Christoph Caks <ccaks@bestsolution.at> - initial API and implementation
 */

#include <GL/glew.h>
#include "../Common.h"
#include <iomanip>

#include <DriftFX/GL/GLTexture.h>
#include <DriftFX/Texture.h>

#include <DriftFX/GL/GLDebug.h>

#include "InternalGLContext.h"

using namespace driftfx;
using namespace driftfx::gl;

using namespace driftfx::internal::gl;

GLTexture::GLTexture(GLContext *context, int width, int height) : Texture(width, height),
	context(context),
	textureName(0) {

	context->SetCurrent();

	logGLErr(InternalGLContext::glGenTextures(1, &textureName););

	// TODO check for error & if textureName is valid
}

GLTexture::~GLTexture() {

	context->SetCurrent();

	glDeleteTextures(1, &textureName);
}

GLuint GLTexture::Name() {
	return textureName;
}


void driftfx::gl::CopyGLTexture(GLuint sourceTex, GLuint targetTex, int width, int height) {
	// COPY OVER
	GLuint fbos[2];

	GLERR(glGenFramebuffers(2, &fbos[0]); );

	GLERR(glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[0]); );
	GLERR(glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sourceTex, 0); );

	GLERR(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos[1]); );
	GLERR(glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, targetTex, 0); );

	GLERR(glClearColor(0, 0, 0, 0); );
	GLERR(glClear(GL_COLOR_BUFFER_BIT); );

	GLERR(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR); );

	// We need to wait here for the blit operation to finish to prevent copying an empty texture in FX context
	glFinish();

	GLERR(glDeleteFramebuffers(2, &fbos[0]); );
}