// Author(s): Paulo Pagliosa, Carlos Monteiro e Yago Mescolotte
// Last revision: 15/10/2019

#ifndef __P2_h
#define __P2_h

#include "Assets.h"
#include "GLRenderer.h"
#include "Primitive.h"
#include "SceneEditor.h"
#include "core/Flags.h"
#include "graphics/Application.h"
#include <vector>

using namespace cg;

class P2: public GLWindow
{
public:
  P2(int width, int height):
    GLWindow{"cg2019 - P2", width, height},
    _program{"P2"}
  {
    // do nothing
  }

  /// Initialize the app.
  void initialize() override;

  /// Update the GUI.
  void gui() override;

  /// Render the scene.
  void render() override;

	auto IteratorScene()
	{
		return sceneColection.begin();
	}

	auto IteratorEndScene() {
		return sceneColection.end();
	}

	void
	addScene(Reference<Scene> scenes)
	{
		auto begin = this->sceneColection.begin();
		auto end = this->sceneColection.end();

		for (auto sceneIt = begin; sceneIt != end; sceneIt++)
		{
			if (*sceneIt == scenes)
				return;
		}
		sceneColection.push_back(scenes);
	}

	void
	removeScene(Reference<Scene> component)
	{
		auto begin = sceneColection.begin();
		auto end = sceneColection.end();
		for (auto it = begin; it != end; it++)
		{
			if (*it == component) {
				sceneColection.erase(it);
				break;
			}
		}
	}

	auto sizeScene() {
		return sceneColection.size();
	}

private:
  enum ViewMode
  {
    Editor = 0,
    Renderer = 1
  };

  enum class MoveBits
  {
    Left = 1,
    Right = 2,
    Forward = 4,
    Back = 8,
    Up = 16,
    Down = 32
  };

  enum class DragBits
  {
    Rotate = 1,
    Pan = 2
  };

	int _boxCount = 0;
	int _sphereCount = 0;
	int _camCount = 0;
	int _objectCount = 0;
	int _sceneCount = 0;

	bool _flagDeletePopup = false;

  GLSL::Program _program;
	Scene* _sceneCurrent;
	std::vector<Reference<Scene>> sceneColection;
  Reference<SceneEditor> _editor;
  Reference<GLRenderer> _renderer;
  
	SceneNode* _current{};
  Color _selectedWireframeColor{255, 102, 0};
	Color _frustumWireframeColor{ 255, 0, 0 };
  Flags<MoveBits> _moveFlags{};
  Flags<DragBits> _dragFlags{};
  int _pivotX;
  int _pivotY;
  int _mouseX;
  int _mouseY;
  bool _showAssets{true};
  bool _showEditorView{true};
  ViewMode _viewMode{ViewMode::Editor};

  static MeshMap _defaultMeshes;

  void buildScene();
  void renderScene();
	void preview(Camera&);
	void darkPreview(Camera&);
	void focus();

  void mainMenu();
  void fileMenu();
  void showOptions();

	void createObject();
	void deleteObject();
  void hierarchyWindow();
  void inspectorWindow();
  void assetsWindow();
  void editorView();
  void sceneGui();
  void sceneObjectGui();
  void objectGui();
  void editorViewGui();
  void inspectPrimitive(Primitive&);
  void inspectCamera(Camera&);
  void addComponentButton(SceneObject&);

  void drawPrimitive(Primitive&);
  void drawCamera(Camera&);

  bool windowResizeEvent(int, int) override;
  bool keyInputEvent(int, int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

  static void buildDefaultMeshes();

	// Auxiliary functions
	enum objectType { Object, Box, Sphere, Cam };
	Reference<SceneObject> nodeCreator(Reference<SceneObject>, objectType);

	void hierarchyWindowRecursive(Reference<SceneObject>&);
	void renderRecursivo(Reference<SceneObject>&);

}; // P2

#endif // __P2_h
