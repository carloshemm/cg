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
// OVERVIEW: SceneObject.cpp
// ========
// Source file for scene object.
//
// Author(s): Paulo Pagliosa, Carlos Monteiro e Yago Mescolotte
// Last revision: 15/10/2019

#include "SceneObject.h"
#include "Scene.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// SceneObject implementation
// ===========
void
SceneObject::setParent(SceneObject* parent)
{
	SceneObject::makeUse(this);
	// Remove from parent
	if (this->parent() == nullptr) {
		(*this).scene()->root()->removeSceneObject(this);
	}
	else {
		this->parent()->sceneObjectColection.remove(this);
	}

	// Set new parent
	this->_parent = parent;
	this->transform()->update();

	// Add this SceneObject in parent list
	if (parent == nullptr) {
		this->scene()->root()->addSceneObject(this);
	}
	else {
		parent->addSceneObject(this);
	}
	SceneObject::release(this);
}

} // end namespace cg
