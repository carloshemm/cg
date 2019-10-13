// Author(s): Paulo Pagliosa, Carlos Monteiro e Yago Mescolotte
// Last revision: 15/10/2019

#include "geometry/MeshSweeper.h"
#include "P2.h"
#include <iostream>

#define MIN_SCALE				0.0001f
#define FOCUS_OFFSET			10
MeshMap P2::_defaultMeshes;

inline void
P2::buildDefaultMeshes()
{
  _defaultMeshes["None"] = nullptr;
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
  return new Primitive{mit->second, mit->first};
}

Reference<SceneObject>
P2::nodeCreator(Reference<SceneObject> objectFather, objectType type)
{
	std::string name;
	Reference<SceneObject> son;

	switch (type)
	{
	case P2::Object:
		_objectCount++;
		name = "Object " + std::to_string(_objectCount);
		son = new SceneObject(name.c_str(), *_sceneCurrent);
		//son->addComponent(makePrimitive(_defaultMeshes.find("None")));
		son->setParent(objectFather);
		break;

	case P2::Box:
		_boxCount++;
		name = "Box " + std::to_string(_boxCount);
		son = new SceneObject(name.c_str(), *_sceneCurrent);
		son->addComponent(makePrimitive(_defaultMeshes.find("Box")));
		son->setParent(objectFather);
		break;

	
	case P2::Sphere:
		_sphereCount++;
		name = "Sphere " + std::to_string(_sphereCount);
		son = new SceneObject(name.c_str(), *_sceneCurrent);
		son->addComponent(makePrimitive(_defaultMeshes.find("Sphere")));
		son->setParent(objectFather);
		break;

	case P2::Cam:
		_camCount++;
		name = "Camera " + std::to_string(_camCount);
		son = new SceneObject(name.c_str(), *_sceneCurrent);
		auto camera = new Camera;
		son->addComponent(camera);
		son->setParent(objectFather);
		break;

	}
	return son;
}

inline void
P2::createObject()
{
	SceneObject* objectCurrent = dynamic_cast<SceneObject*> (_current);
	SceneObject* object = nullptr;
	if (objectCurrent == nullptr)
	{
		Scene* sceneFather = dynamic_cast<Scene*> (_current);
		objectCurrent = sceneFather->root();
	}
	if (ImGui::MenuItem("Empty Object"))
	{
		object = nodeCreator(Reference<SceneObject>(objectCurrent), Object);
	}
	if (ImGui::BeginMenu("3D Object"))
	{
		if (ImGui::MenuItem("Box"))
		{
			object = nodeCreator(Reference<SceneObject>(objectCurrent), Box);
		}
		if (ImGui::MenuItem("Sphere"))
		{
			object = nodeCreator(Reference<SceneObject>(objectCurrent), Sphere);
		}
		ImGui::EndMenu();
	}
	if (ImGui::MenuItem("Camera"))
	{
		object = nodeCreator(Reference<SceneObject>(objectCurrent), Cam);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (object != nullptr)
		_current = object;
}

inline void
P2::deleteObject()
{
	// Check if _current is a SceneObject
	if (SceneObject* objectCurrent = dynamic_cast<SceneObject*> (_current)) // It's a SceneObject
	{
		// Check parent to remove SceneObject
		auto parent = objectCurrent->parent();
		parent->removeSceneObject(objectCurrent);
		
		// Refresh _current
		if (parent->sizeSceneObject() > 0)
			_current = *(parent->IteratorSceneObject());
		else if (_sceneCurrent->root() == parent)
			_current = _sceneCurrent;
		else
			_current = parent;
	}
	else // It's a Scene
	{
		if (sizeScene() > 1)
		{
			removeScene(Reference<Scene>(_sceneCurrent));
			_current = _sceneCurrent = *IteratorScene();
		}
		else
		{
			_flagDeletePopup = true;
		}
	}
}

inline void
P2::buildScene()
{
	_sceneCount++;
	std::string name = "Scene " + std::to_string(_sceneCount);
	_current = _sceneCurrent = new Scene{ name.c_str() };
	addScene(_sceneCurrent);

  _editor = new SceneEditor{*_sceneCurrent};
  _editor->setDefaultView((float)width() / (float)height());

	// Create the Objects
	Reference<SceneObject> cam1 = nodeCreator(_sceneCurrent->root(), Cam);
	Reference<SceneObject> box1 = nodeCreator(_sceneCurrent->root(), Box);
	Reference<SceneObject> object1 = nodeCreator(_sceneCurrent->root(), Object);
	Reference<SceneObject> object2 = nodeCreator(_sceneCurrent->root(), Object);
	Reference<SceneObject> object3 = nodeCreator(object1, Object);
	Reference<SceneObject> object4 = nodeCreator(object1, Object);
	Reference<SceneObject> object5 = nodeCreator(object2, Object);
	Reference<SceneObject> object6 = nodeCreator(object5, Object);
	Reference<SceneObject> Sphere1 = nodeCreator(object3, Sphere);
	Reference<SceneObject> box2 = nodeCreator(object4, Box);
	Reference<SceneObject> Sphere2 = nodeCreator(object6, Sphere);
}

void
P2::initialize()
{
  Application::loadShaders(_program, "shaders/p2.vs", "shaders/p2.fs");
  Assets::initialize();
  buildDefaultMeshes();
  buildScene();
  _renderer = new GLRenderer{*_sceneCurrent, &_program};
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  _program.use();
}

namespace ImGui
{
  void ShowDemoWindow(bool*);
}

inline void
P2::hierarchyWindow()
{
  ImGui::Begin("Hierarchy");

	if (ImGui::Button("Create###object"))
    ImGui::OpenPopup("CreateObjectPopup");
	if (ImGui::BeginPopup("CreateObjectPopup"))
	{
		if (ImGui::MenuItem("Scene"))
		{
			_sceneCount++;
			std::string name = "Scene " + std::to_string(_sceneCount);
			_current = _sceneCurrent = new Scene{ name.c_str() };
			addScene(_sceneCurrent);
		}
		createObject();
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	
	if (ImGui::Button("Delete"))
		deleteObject();
	if (_flagDeletePopup)
	{
		ImGui::OpenPopup("Can't delete");
		_flagDeletePopup = false;
	}
	if (ImGui::BeginPopup("Can't delete"))
	{
		ImGui::Text("Must be at least one scene");
		ImGui::EndPopup();
	}

	ImGui::Separator();

	auto begin = IteratorScene();
	auto end = IteratorEndScene();
	for (auto sceneIt = begin; sceneIt != end; sceneIt++)
	{
		auto f = ImGuiTreeNodeFlags_OpenOnArrow;
		auto open = ImGui::TreeNodeEx(*sceneIt,
			_current == *sceneIt ? f | ImGuiTreeNodeFlags_Selected : f,
			(*sceneIt)->name());

		if (ImGui::IsItemClicked())
			_current = _sceneCurrent = *sceneIt;

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(GLFW_MOUSE_BUTTON_RIGHT, false))
			{
				_current = *sceneIt;
				ImGui::OpenPopup("Options");
			}
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (auto* payload = ImGui::AcceptDragDropPayload("SceneObject"))
			{
				auto obj = *(std::list<cg::Reference<SceneObject>>::iterator*)payload->Data;
				(*obj)->setParent((*sceneIt)->root());
				_sceneCurrent = *sceneIt;
			}
			ImGui::EndDragDropTarget();
		}

		// OPEN THE TREE
		if (open) {
			Reference<SceneObject> obj = (*sceneIt)->root();
			hierarchyWindowRecursive(obj);
		}
	}
	if (ImGui::BeginPopup("Options"))
	{
		if (ImGui::MenuItem("Delete"))
			deleteObject();
		if (ImGui::BeginMenu("Create"))
		{
			createObject();
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

  ImGui::End();
}

void
P2::hierarchyWindowRecursive(Reference<SceneObject>& object)
{
	auto begin = object->IteratorSceneObject();
	auto end = object->IteratorEndSceneObject();
	for (auto objectIt = begin; objectIt != end; objectIt++)
	{
		// Check if it's a tree leaf node to define its icon
		auto flag = ((*objectIt)->sizeSceneObject() == 0) ?
			ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen : ImGuiTreeNodeFlags_OpenOnArrow;

		// Create Level nodes
		auto open = ImGui::TreeNodeEx(*objectIt,
			_current == *objectIt ? flag | ImGuiTreeNodeFlags_Selected : flag,
			(*objectIt)->name());

		// Check if is selected
		if (ImGui::IsItemClicked())
			_current = *objectIt;

		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(GLFW_MOUSE_BUTTON_RIGHT, false))
			{
				_current = *objectIt;
				ImGui::OpenPopup("Options");
			}
		}

		if (ImGui::BeginDragDropSource())
		{
			ImGui::Text((*objectIt)->name());
			ImGui::SetDragDropPayload("SceneObject", &objectIt, sizeof(SceneObject));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (auto* payload = ImGui::AcceptDragDropPayload("SceneObject"))
			{
				auto obj = *(std::list<cg::Reference<SceneObject>>::iterator*)payload->Data;
				(*obj)->setParent(*objectIt);
				_sceneCurrent = (*objectIt)->scene();
			}
			ImGui::EndDragDropTarget();
		}

		// Open the next level
		if (open && flag == ImGuiTreeNodeFlags_OpenOnArrow) {
			hierarchyWindowRecursive(*objectIt);
		}

	}
	if (ImGui::BeginPopup("Options"))
	{
		if (ImGui::MenuItem("Delete"))
			deleteObject();
		if (ImGui::BeginMenu("Create"))
		{
			createObject();
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	ImGui::TreePop();
}

namespace ImGui
{ // begin namespace ImGui

void
ObjectNameInput(NameableObject* object)
{
  const int bufferSize{128};
  static NameableObject* current;
  static char buffer[bufferSize];

  if (object != current)
  {
    strcpy_s(buffer, bufferSize, object->name());
    current = object;
  }
  if (ImGui::InputText("Name", buffer, bufferSize))
    object->setName(buffer);
}

inline bool
ColorEdit3(const char* label, Color& color)
{
  return ImGui::ColorEdit3(label, (float*)&color);
}

inline bool
DragVec3(const char* label, vec3f& v)
{
  return DragFloat3(label, (float*)&v, 0.1f, 0.0f, 0.0f, "%.2g");
}

void
TransformEdit(Transform* transform)
{
  vec3f temp;

  temp = transform->localPosition();
  if (ImGui::DragVec3("Position", temp))
    transform->setLocalPosition(temp);
  
	temp = transform->localEulerAngles();
  if (ImGui::DragVec3("Rotation", temp))
    transform->setLocalEulerAngles(temp);
  
	temp = transform->localScale();
	if (ImGui::DragVec3("Scale", temp))
	{
		if (temp[0] < MIN_SCALE)
			temp[0] = MIN_SCALE;
		if (temp[1] < MIN_SCALE)
			temp[1] = MIN_SCALE;
		if (temp[2] < MIN_SCALE)
			temp[2] = MIN_SCALE;
		transform->setLocalScale(temp);
	}
}

} // end namespace ImGui

inline void
P2::sceneGui()
{
  auto scene = (Scene*)_current;

  ImGui::ObjectNameInput(_current);
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Colors"))
  {
    ImGui::ColorEdit3("Background", scene->backgroundColor);
    ImGui::ColorEdit3("Ambient Light", scene->ambientLight);
  }
}

inline void
P2::inspectPrimitive(Primitive& primitive)
{
  char buffer[16];

  snprintf(buffer, 16, "%s", primitive.meshName());
  ImGui::InputText("Mesh", buffer, 16, ImGuiInputTextFlags_ReadOnly);
  if (ImGui::BeginDragDropTarget())
  {
    if (auto* payload = ImGui::AcceptDragDropPayload("PrimitiveMesh"))
    {
      auto mit = *(MeshMapIterator*)payload->Data;
      primitive.setMesh(mit->second, mit->first);
    }
    ImGui::EndDragDropTarget();
  }
  ImGui::SameLine();
  if (ImGui::Button("...###PrimitiveMesh"))
    ImGui::OpenPopup("PrimitiveMeshPopup");
  if (ImGui::BeginPopup("PrimitiveMeshPopup"))
  {
    auto& meshes = Assets::meshes();

    if (!meshes.empty())
    {
      for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
        if (ImGui::Selectable(mit->first.c_str()))
          primitive.setMesh(Assets::loadMesh(mit), mit->first);
      ImGui::Separator();
    }
    for (auto mit = _defaultMeshes.begin(); mit != _defaultMeshes.end(); ++mit)
      if (ImGui::Selectable(mit->first.c_str()))
        primitive.setMesh(mit->second, mit->first);
    ImGui::EndPopup();
  }
  ImGui::ColorEdit3("Mesh Color", (float*)&primitive.color);
}

inline void
P2::inspectCamera(Camera& camera)
{
  static const char* projectionNames[]{"Perspective", "Orthographic"};
  auto cp = camera.projectionType();

  if (ImGui::BeginCombo("Projection", projectionNames[cp]))
  {
    for (auto i = 0; i < IM_ARRAYSIZE(projectionNames); ++i)
    {
      auto selected = cp == i;

      if (ImGui::Selectable(projectionNames[i], selected))
        cp = (Camera::ProjectionType)i;
      if (selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  camera.setProjectionType(cp);
  if (cp == View3::Perspective)
  {
    auto fov = camera.viewAngle();

    if (ImGui::SliderFloat("View Angle",
      &fov,
      MIN_ANGLE,
      MAX_ANGLE,
      "%.0f deg",
      1.0f))
      camera.setViewAngle(fov <= MIN_ANGLE ? MIN_ANGLE : fov);
  }
	else
	{
		auto h = camera.height();

		if (ImGui::DragFloat("Height",
			&h,
			MIN_HEIGHT * 10.0f,
			MIN_HEIGHT,
			math::Limits<float>::inf()))
			camera.setHeight(h <= 0 ? MIN_HEIGHT : h);
	}

  float n;
  float f;
	int v[2], size;

  camera.clippingPlanes(n, f);

  if (ImGui::DragFloatRange2("Clipping Planes",
    &n,
    &f,
    MIN_DEPTH,
    MIN_DEPTH,
    math::Limits<float>::inf(),
    "Near: %.2f",
    "Far: %.2f"))
  {
    if (n <= 0)
      n = MIN_DEPTH;
    if (f - n < MIN_DEPTH)
      f = n + MIN_DEPTH;
    camera.setClippingPlanes(n, f);
  }
}

inline void
P2::addComponentButton(SceneObject& object)
{
  if (ImGui::Button("Add Component"))
    ImGui::OpenPopup("AddComponentPopup");
  if (ImGui::BeginPopup("AddComponentPopup"))
  {
		if (auto object = dynamic_cast<SceneObject*>((_current)))
		{
			if (ImGui::MenuItem("Primitive"))
			{
				object->addComponent(makePrimitive(_defaultMeshes.find("None")));
			}
			if (ImGui::MenuItem("Camera"))
			{
				auto camera = new Camera;
				object->addComponent(camera);
			}
		}
    ImGui::EndPopup();
  }
}

inline void
P2::sceneObjectGui()
{
  auto object = (SceneObject*)_current;

  addComponentButton(*object);
  ImGui::Separator();
  ImGui::ObjectNameInput(object);
  ImGui::SameLine();
  ImGui::Checkbox("visible", &object->visible);
  ImGui::Separator();
  if (ImGui::CollapsingHeader(object->transform()->typeName()))
    ImGui::TransformEdit(object->transform());

	Component* primitiveCurrent = nullptr;
	auto begin = object->IteratorComponent();
	auto end = object->IteratorEndComponent();

	for (auto component = begin; component != end; component++)
	{

		if (auto p = dynamic_cast<Primitive*>((*component).get()))
		{
			auto notDelete{ true };
			auto open = ImGui::CollapsingHeader(p->typeName(), &notDelete);

			if (!notDelete)
			{
				object->removeComponent(p);
				break;
			}
			else if (open)
				inspectPrimitive(*p);
		}
		else if (auto c = dynamic_cast<Camera*>((*component).get()))
		{
			auto notDelete{ true };
			auto open = ImGui::CollapsingHeader(c->typeName(), &notDelete);

			if (!notDelete)
			{
				object->removeComponent(c);
				break;
			}
			else if (open)
			{
				auto isCurrent = c == Camera::current();

				ImGui::Checkbox("Current", &isCurrent);
				Camera::setCurrent(isCurrent ? c : nullptr);
				inspectCamera(*c);
			}
		}
	}
}

inline void
P2::objectGui()
{
  if (_current == nullptr)
    return;
  if (dynamic_cast<SceneObject*>(_current))
  {
    sceneObjectGui();
    return;
  }
  if (dynamic_cast<Scene*>(_current))
    sceneGui();
}

inline void
P2::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

inline void
P2::editorViewGui()
{
  if (ImGui::Button("Set Default View"))
    _editor->setDefaultView(float(width()) / float(height()));
  ImGui::Separator();

  auto t = _editor->camera()->transform();
  vec3f temp;

  temp = t->localPosition();
  if (ImGui::DragVec3("Position", temp))
    t->setLocalPosition(temp);
  temp = t->localEulerAngles();
  if (ImGui::DragVec3("Rotation", temp))
    t->setLocalEulerAngles(temp);
  inspectCamera(*_editor->camera());
  ImGui::Separator();
  ImGui::Checkbox("Show Ground", &_editor->showGround);
}

inline void
P2::assetsWindow()
{
  if (!_showAssets)
    return;
  ImGui::Begin("Assets");
  if (ImGui::CollapsingHeader("Meshes"))
  {
    auto& meshes = Assets::meshes();

    for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
    {
      auto meshName = mit->first.c_str();
      auto selected = false;

      ImGui::Selectable(meshName, &selected);
      if (ImGui::BeginDragDropSource())
      {
        Assets::loadMesh(mit);
        ImGui::Text(meshName);
        ImGui::SetDragDropPayload("PrimitiveMesh", &mit, sizeof(mit));
        ImGui::EndDragDropSource();
      }
    }
  }
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Textures"))
  {
    // p3
  }
  ImGui::End();
}

inline void
P2::editorView()
{
  if (!_showEditorView)
    return;
  ImGui::Begin("Editor View Settings");
  editorViewGui();
  ImGui::End();
}

inline void
P2::fileMenu()
{
  if (ImGui::MenuItem("New"))
  {
    // TODO
  }
  if (ImGui::MenuItem("Open...", "Ctrl+O"))
  {
    // TODO
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Save", "Ctrl+S"))
  {
		// TODO
  }
  if (ImGui::MenuItem("Save As..."))
  {
    // TODO
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Exit", "Alt+F4"))
  {
    shutdown();
  }
}

inline bool
showStyleSelector(const char* label)
{
  static int style = 1;

  if (!ImGui::Combo(label, &style, "Classic\0Dark\0Light\0"))
    return false;
  switch (style)
  {
    case 0: ImGui::StyleColorsClassic();
      break;
    case 1: ImGui::StyleColorsDark();
      break;
    case 2: ImGui::StyleColorsLight();
      break;
  }
  return true;
}

inline void
P2::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  showStyleSelector("Color Theme##Selector");

	ImGui::Separator();

  ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
	
	ImGui::Separator();

	ImGui::ColorEdit3("Frustum Wireframe", _frustumWireframeColor);
	ImGui::PopItemWidth();
}

inline void
P2::mainMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      fileMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
      if (Camera::current() == 0)
        ImGui::MenuItem("Edit View", nullptr, true, false);
      else
      {
        static const char* viewLabels[]{"Editor", "Renderer"};

        if (ImGui::BeginCombo("View", viewLabels[_viewMode]))
        {
          for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
          {
            if (ImGui::Selectable(viewLabels[i], _viewMode == i))
              _viewMode = (ViewMode)i;
          }
          ImGui::EndCombo();
        }
      }
      ImGui::Separator();
      ImGui::MenuItem("Assets Window", nullptr, &_showAssets);
      ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools"))
    {
      if (ImGui::BeginMenu("Options"))
      {
        showOptions();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void
P2::gui()
{
  mainMenu();
  hierarchyWindow();
  inspectorWindow();
  assetsWindow();
  editorView();

  /*
  static bool demo = true;
  ImGui::ShowDemoWindow(&demo);
  */
}

inline void
drawMesh(GLMesh* mesh, GLuint mode)
{
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

inline void
P2::drawPrimitive(Primitive& primitive)
{
  auto m = glMesh(primitive.mesh());

  if (nullptr == m)
    return;

  auto t = primitive.transform();
  auto normalMatrix = mat3f{t->worldToLocalMatrix()}.transposed();

  _program.setUniformMat4("transform", t->localToWorldMatrix());
  _program.setUniformMat3("normalMatrix", normalMatrix);
  _program.setUniformVec4("color", primitive.color);
  _program.setUniform("flatMode", (int)0);
  m->bind();
  drawMesh(m, GL_FILL);
  if (primitive.sceneObject() != _current)
    return;
  _program.setUniformVec4("color", _selectedWireframeColor);
  _program.setUniform("flatMode", (int)1);
  drawMesh(m, GL_LINE);
}

inline void
P2::preview(Camera& camera)
{
	int v[4];
	vec3f p[4];

	int h = 200;
	int w = int(camera.aspectRatio() * h);

	int V[2];
	
	darkPreview(camera);
	 

	glGetIntegerv(GL_VIEWPORT, v); // Save viewport
	glViewport(0, 0, w, h); // Define new window size (X, Y, W, H)
	glScissor(0, 0, w, h); // Define the cut window (X, Y, W, H)
	glEnable(GL_SCISSOR_TEST);

	renderScene();
	
	glDisable(GL_SCISSOR_TEST);
	glViewport(v[0], v[1], v[2], v[3]); // restore viewport
}



inline void
P2::darkPreview(Camera& camera)
{
	int v[4];
	vec3f p[4];

	int h = 200;
	int w = int(camera.aspectRatio() * h);

	int V[2];


	glGetIntegerv(GL_VIEWPORT, v); // Save viewport
	glViewport(0, 0, w+2, h+2); // Define new window size (X, Y, W, H)
	glScissor(0, 0, w+2, h+2); // Define the cut window (X, Y, W, H)
	glEnable(GL_SCISSOR_TEST);

	renderScene();

	
	glClearDepth(1.0);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	glDisable(GL_SCISSOR_TEST);
	glViewport(v[0], v[1], v[2], v[3]); // restore viewport

}

void P2::focus() {
	auto ob = dynamic_cast<SceneObject*>(_current);
	auto T = ob->transform();
	auto sc = T->localScale();
	auto scMedia = (sc.x + sc.z + sc.y)/3;
	auto lz = T->localPosition();
	lz.z += FOCUS_OFFSET*scMedia;
	_editor->camera()->transform()->setLocalPosition(lz);
		

}


inline void
P2::drawCamera(Camera& camera)
{
	vec3f A[4];
	vec3f B[4];
	float f, b;
	float h_A, w_A, h_B, w_B;
	float ang = (float) tan(camera.viewAngle() / 2 * M_PI / 180);
	auto matriz = mat4f{ camera.cameraToWorldMatrix()};
	auto bf = camera.clippingPlanes(f, b);
	if (bf > 10) {
		b = f + 10;
		bf = 10;
	}

	//        PLAN XY              PLAN YZ
	//  B1----------------B0   _______b________|
	//	|                  |                   |
	//	|    A1------A0    |       |           |
	//	|    |        |    |   __f_|           |
	//  |    |        |    |       |           |
	//	|    A2------A3    |       |A          |
	//	|                  |                   |
	//	B2----------------B3                   |B
	
	switch (camera.projectionType())
	{
	default:
	case camera.Perspective:
		h_A = ang * f;
		w_A = h_A * camera.aspectRatio();
		h_B = ang * b;
		w_B = h_B * camera.aspectRatio();
		break;

	case camera.Parallel:
		h_A = camera.height() / 2;
		w_A = h_A * camera.aspectRatio();
		h_B = h_A;
		w_B = w_A;
		break;
	}
	A[0] = { w_A, h_A, -f };
	A[1] = { -w_A, h_A, -f };
	A[2] = { -w_A, -h_A, -f };
	A[3] = { w_A, -h_A, -f };

	B[0] = { w_B, h_B, -b };
	B[1] = { -w_B, h_B, -b };
	B[2] = { -w_B, -h_B, -b };
	B[3] = { w_B, -h_B, -b };

	for (int i = 0; i < 4; i++) 
	{
		A[i] = matriz.transform(A[i]);
		B[i] = matriz.transform(B[i]);
	}	

	_editor->setLineColor(_frustumWireframeColor);
	_editor->drawLine(A[0], A[1]);
	_editor->drawLine(A[1], A[2]);
	_editor->drawLine(A[2], A[3]);
	_editor->drawLine(A[3], A[0]);

	_editor->drawLine(B[0], B[1]);
	_editor->drawLine(B[1], B[2]);
	_editor->drawLine(B[2], B[3]);
	_editor->drawLine(B[3], B[0]);

	_editor->drawLine(A[0], B[0]);
	_editor->drawLine(A[1], B[1]);
	_editor->drawLine(A[2], B[2]);
	_editor->drawLine(A[3], B[3]);
}

inline void
P2::renderScene()
{
  if (auto camera = Camera::current())
  {
    _renderer->setCamera(camera);
    _renderer->setImageSize(width(), height());
    _renderer->render();
    _program.use();
  }
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
P2::render()
{
  if (_viewMode == ViewMode::Renderer)
  {
    renderScene();
    return;
  }
  if (_moveFlags)
  {
    const auto delta = _editor->orbitDistance() * CAMERA_RES;
    auto d = vec3f::null();

    if (_moveFlags.isSet(MoveBits::Forward))
      d.z -= delta;
    if (_moveFlags.isSet(MoveBits::Back))
      d.z += delta;
    if (_moveFlags.isSet(MoveBits::Left))
      d.x -= delta;
    if (_moveFlags.isSet(MoveBits::Right))
      d.x += delta;
    if (_moveFlags.isSet(MoveBits::Up))
      d.y += delta;
    if (_moveFlags.isSet(MoveBits::Down))
      d.y -= delta;
    _editor->pan(d);
  }
  _editor->newFrame();

  auto ec = _editor->camera();
  const auto& p = ec->transform()->position();
  auto vp = vpMatrix(ec);

	auto object = _sceneCurrent->root();
	auto begin = object->IteratorSceneObject();
	auto end = object->IteratorEndSceneObject();
	for (auto it = begin; it != end; it++) {
		_program.setUniformMat4("vpMatrix", vp);
		_program.setUniformVec4("ambientLight", _sceneCurrent->ambientLight);
		_program.setUniformVec3("lightPosition", p);
		renderRecursivo(*it);
	}
}

void
P2::renderRecursivo(Reference<SceneObject>& object)
{
	if (!object->visible)
		return;

	auto beginS = object->IteratorSceneObject();
	auto endS = object->IteratorEndSceneObject();
	for (auto it = beginS; it != endS; it++)
		renderRecursivo(*it);

	Component* primitiveCurrent = nullptr;
	auto begin = object->IteratorComponent();
	auto end = object->IteratorEndComponent();

	for (auto component = begin; component != end; component++)
	{
		if (object == _current)
		{
			auto t = object->transform();
			_editor->drawAxes(t->position(), mat3f{ t->rotation() });
		}
		if (auto p = dynamic_cast<Primitive*>((*component).get()))
			drawPrimitive(*p);
		
		else if (auto c = dynamic_cast<Camera*>((*component).get()))
			if (object == _current)
			{
				drawCamera(*c);
				//darkPreview(*c);
				preview(*c);
			}
	}
}

bool
P2::windowResizeEvent(int width, int height)
{
  _editor->camera()->setAspectRatio(float(width) / float(height));
  return true;
}

bool
P2::keyInputEvent(int key, int action, int mods)
{
  auto active = action != GLFW_RELEASE && mods == GLFW_MOD_ALT;

  switch (key)
  {
    case GLFW_KEY_W:
      _moveFlags.enable(MoveBits::Forward, active);
      break;
    case GLFW_KEY_S:
      _moveFlags.enable(MoveBits::Back, active);
      break;
    case GLFW_KEY_A:
      _moveFlags.enable(MoveBits::Left, active);
      break;
    case GLFW_KEY_D:
      _moveFlags.enable(MoveBits::Right, active);
      break;
    case GLFW_KEY_Q:
      _moveFlags.enable(MoveBits::Up, active);
      break;
    case GLFW_KEY_E:
      _moveFlags.enable(MoveBits::Down, active);
      break;
		case GLFW_KEY_DELETE:
			if (action == GLFW_RELEASE)
				deleteObject();
			break;
		case GLFW_KEY_F:
			if (mods == GLFW_MOD_ALT)
				focus();
			break;
  }

  return false;
}

bool
P2::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse || _viewMode == ViewMode::Renderer)
    return false;
  _editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
P2::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse || _viewMode == ViewMode::Renderer)
    return false;
  (void)mods;

  auto active = actions == GLFW_PRESS;

	if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  if (_dragFlags)
    cursorPosition(_pivotX, _pivotY);
	
	return true;
}

bool
P2::mouseMoveEvent(double xPos, double yPos)
{
  if (!_dragFlags)
    return false;
  _mouseX = (int)xPos;
  _mouseY = (int)yPos;

  const auto dx = (_pivotX - _mouseX);
  const auto dy = (_pivotY - _mouseY);

  _pivotX = _mouseX;
  _pivotY = _mouseY;
  if (dx != 0 || dy != 0)
  {
    if (_dragFlags.isSet(DragBits::Rotate))
    {
      const auto da = -_editor->camera()->viewAngle() * CAMERA_RES;
      isKeyPressed(GLFW_KEY_LEFT_ALT) ?
        _editor->orbit(dy * da, dx * da) :
        _editor->rotateView(dy * da, dx * da);
    }
    if (_dragFlags.isSet(DragBits::Pan))
    {
      const auto dt = -_editor->orbitDistance() * CAMERA_RES;
      _editor->pan(-dt * math::sign(dx), dt * math::sign(dy), 0);
    }
  }
  return true;
}
