//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: GLRenderer.cpp
// ========
// Source file for OpenGL renderer.
//
// Author(s): Paulo Pagliosa, Carlos Monteiro e Yago Mescolotte
// Last revision: 15/10/2019

#include "GLRenderer.h"

namespace cg
{ // begin namespace cg


//////////////////////////////////////////////////////////
//
// GLRenderer implementation
// ==========
void
GLRenderer::update()
{
  Renderer::update();
  // TODO
}

inline void
drawMesh(GLMesh* mesh, GLuint mode)
{
	glPolygonMode(GL_FRONT_AND_BACK, mode);
	glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

void
GLRenderer::render()
{
  const auto& bc = _sceneCurrent->backgroundColor;

  glClearColor(bc.r, bc.g, bc.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // TODO

	const auto& p = _camera->transform()->position();
	auto vp = vpMatrix(_camera);

	_program->setUniformMat4("vpMatrix", vp);
	_program->setUniformVec4("ambientLight", _sceneCurrent->ambientLight);
	_program->setUniformVec3("lightPosition", p);

	Reference<SceneObject> obj = _sceneCurrent->root();
	renderRecursivo(obj);
}

void
GLRenderer::renderRecursivo(Reference<SceneObject>& object)
{
	auto begin = object->IteratorSceneObject();
	auto end = object->IteratorEndSceneObject();
	for (auto it = begin; it != end; it++)
	{
		if (!(*it)->visible)
			continue;

		renderRecursivo(*it);

		Component* primitiveCurrent = nullptr;
		auto begin = (*it)->IteratorComponent();
		auto end = (*it)->IteratorEndComponent();

		for (auto component = begin; component != end; component++)
		{
			if (auto primitive = dynamic_cast<Primitive*>((*component).get()))
			{
				auto m = glMesh(primitive->mesh());

				if (nullptr == m)
					return;

				auto t = primitive->transform();
				auto normalMatrix = mat3f{ t->worldToLocalMatrix() }.transposed();

				_program->setUniformMat4("transform", t->localToWorldMatrix());
				_program->setUniformMat3("normalMatrix", normalMatrix);
				_program->setUniformVec4("color", primitive->color);
				_program->setUniform("flatMode", (int)0);
				m->bind();
				drawMesh(m, GL_FILL);
			}
		}
	}
}

} // end namespace cg
