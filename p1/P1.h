// Author(s): Paulo Pagliosa, Yago Mescolotte, Carlos Monteiro
// Last revision: 31/08/2019

#ifndef __P1_h
#define __P1_h

#include "Primitive.h"
#include "Scene.h"
#include "graphics/Application.h"

using namespace cg;

class P1: public GLWindow
{
public:
  P1(int width, int height):
    GLWindow{"cg2019 - P1", width, height},
    _program{"P1"}
  {
    // do nothing
  }

  /// Initialize the app.
  void initialize() override;

  /// Update the GUI.
  void gui() override;

  /// Render the scene.
  void render() override;

private:
  GLSL::Program _program;
  Reference<Scene> _scene;
  SceneNode* _current{};
  Color selectedWireframeColor{0, 0, 0};
	int boxCount = 0;
	int objectCount = 0;

  void buildScene();

  void hierarchyWindow();
  void inspectorWindow();
  void sceneGui();
  void sceneObjectGui();
  void objectGui();

	void hierarchyWindowRecursive(ListColection<Reference<SceneObject>>* sceneObjectColection);
	auto findPrimitive(Reference<SceneObject> obj);
	void removeSceneObject(Reference <SceneObject> obj);

	enum objectBox { Object, Box };
	template <typename T>
	Reference<SceneObject> objectOrBoxCreator(T father, objectBox obj);

}; // P1

#endif // __P1_h
