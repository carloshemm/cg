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
// OVERVIEW: SceneObject.h
// ========
// Class definition for scene object.
//
// Author(s): Paulo Pagliosa, Carlos Monteiro e Yago Mescolotte
// Last revision: 15/10/2019

#ifndef __SceneObject_h
#define __SceneObject_h

#include "SceneNode.h"
#include "Transform.h"
#include "Primitive.h"
#include "Camera.h"

#include <list>
#include <vector>
#include <iterator> 

namespace cg
{ // begin namespace cg

// Forward definition
class Scene;


/////////////////////////////////////////////////////////////////////
//
// SceneObject: scene object class
// ===========
class SceneObject: public SceneNode
{
public:
  bool visible{true};

  /// Constructs an empty scene object.
  SceneObject(const char* name, Scene& scene):
    SceneNode{name},
    _sceneCurrent{&scene},
    _parent{}
  {
		_transform = new Transform();
		addComponent(makeUse(_transform));    
  }

  /// Returns the scene which this scene object belong to.
  auto scene() const {
    return _sceneCurrent;
  }

  /// Returns the parent of this scene object.
  auto parent() const {
    return _parent;
  }

  /// Sets the parent of this scene object.
  void setParent(SceneObject* parent);

	void setEditorParent(SceneObject* parent) {
		this->_parent = parent;
	}

  /// Returns the transform of this scene object.
  auto transform() const {
    return _transform;
  }

  auto transform() {
    return _transform;
  }

	// COMPONENT VECTOR
 	auto IteratorComponent() {
		return componentColection.begin();
	}

	auto IteratorEndComponent() {
		return componentColection.end();
	}

	void addComponent(Component* component)
	{
		auto begin = this->componentColection.begin();
		auto end = this->componentColection.end();

		if (auto c = dynamic_cast<Transform*>(component))
		{
			for (auto comp = begin; comp != end; comp++)
			{
				if (auto p = dynamic_cast<Transform*>((*comp).get()))
					return;
			}
		}
		else if (auto c = dynamic_cast<Primitive*>(component))
		{
			for (auto comp = begin; comp != end; comp++)
			{
				if (auto p = dynamic_cast<Primitive*>((*comp).get()))
					return;
			}
		}
		else if (auto c = dynamic_cast<Camera*>(component))
		{
			for (auto comp = begin; comp != end; comp++)
			{
				if (auto p = dynamic_cast<Camera*>((*comp).get()))
					return;
			}
		}
		else
			return;
		component->_sceneObject = this;
		componentColection.push_back(Component::makeUse(component));
	}

	void removeComponent(Reference<Component> component)
	{
		auto begin = componentColection.begin();
		auto end = componentColection.end();
		for (auto it = begin; it != end; it++)
		{
			if (*it == component) {
				componentColection.erase(it);
				break;
			}
		}
	}

	auto sizeComponent() {
		return componentColection.size();
	}

	// SCENEOBJECT LIST
	auto IteratorSceneObject() {
		return sceneObjectColection.begin();
	}

	auto IteratorEndSceneObject() {
		return sceneObjectColection.end();
	}

	void addSceneObject(SceneObject* object) {
		sceneObjectColection.push_back(SceneObject::makeUse(object));
	}

	void removeSceneObject(SceneObject* object) {
		sceneObjectColection.remove(object);
	}

	auto sizeSceneObject() {
		return sceneObjectColection.size();
	}

private:
  Scene* _sceneCurrent;
  SceneObject* _parent;
  Transform* _transform;
	std::list<Reference<SceneObject>> sceneObjectColection;
	std::vector<Reference<Component>> componentColection;

  friend class Scene;

}; // SceneObject

/// Returns the transform of a component.
inline Transform*
Component::transform() { // declared in Component.h
  return sceneObject()->transform();
}

/// Returns the parent of a transform.
inline Transform*
Transform::parent() const // declared in Transform.h
{
  if (auto p = sceneObject()->parent())
    return p->transform();
   return nullptr;
}

} // end namespace cg

#endif // __SceneObject_h
