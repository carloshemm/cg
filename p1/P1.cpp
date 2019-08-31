// Author(s): Paulo Pagliosa, Yago Mescolotte, Carlos Monteiro
// Last revision: 31/08/2019

#include "P1.h"

// Auxiliary functions
inline auto 
P1::findPrimitive(Reference <SceneObject> obj) 
{
	Primitive* primitiveCurrent = nullptr;
	auto begin = obj->componentColection.beginIterador();
	auto end = obj->componentColection.endIterador();

	// Search for primitive component
	for (auto it = begin; it != end; it++) 
	{
		auto component = *it;
		primitiveCurrent = dynamic_cast<Primitive*>(component.get());
		if (primitiveCurrent != nullptr) 
		{
			break;
		}
	}
	return primitiveCurrent;
}

void
P1::removeSceneObject(Reference <SceneObject> obj)
{
	auto begin = obj->sceneObjectColection.beginIterador();
	auto end = obj->sceneObjectColection.endIterador();
	for (auto it = begin; it != end; it++)
	{
		//removeSceneObject(Reference<SceneObject> (*it)); # HAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	}
	auto primitive = findPrimitive(obj);
	if (primitive != nullptr)
		_scene->primitiveColection.remove(primitive);
	if (obj->parent() == nullptr)
		_scene->sceneObjectColection.remove(obj);
	else
		obj->parent()->sceneObjectColection.remove(obj);
}

template <typename T>
//<Reference<Scene>, Reference<SceneObject>>
Reference<SceneObject> P1::objectOrBoxCreator(T father, objectBox obj)
{
	std::string name;

	// Check if father is a SceneObject or a Scene
	SceneObject* objectFather = dynamic_cast<SceneObject*> (father.get());
	if (objectFather == nullptr)
	{
		Scene* sceneFather = dynamic_cast<Scene*> (father.get());
		if (sceneFather == nullptr)
			return nullptr; //sceneCurrent;
	}
	
	Reference<SceneObject> son;

	if (obj == Object) {
		objectCount++;
		name = "Object " + std::to_string(objectCount);
		son = new SceneObject{ name.c_str(), _scene };
		son->setParent(objectFather);
	}
	else if (obj == Box) {
		// Create the Box's
		boxCount++;
		name = "Box " + std::to_string(boxCount);
		son = new SceneObject{ name.c_str(), _scene };
		Reference<Primitive> primitive = cg::makeBoxMesh();
		primitive->setSceneObjectOwner(son);
		son->componentColection.add((Reference <Component>)primitive);
		son->setParent(objectFather);
		_scene->primitiveColection.add(primitive);
	}
	
	return son;
}

namespace cg
{ // begin namespace cg

inline Primitive*
makeBoxMesh()
{
  const vec4f p1{-0.5, -0.5, -0.5, 1};
  const vec4f p2{+0.5, -0.5, -0.5, 1};
  const vec4f p3{+0.5, +0.5, -0.5, 1};
  const vec4f p4{-0.5, +0.5, -0.5, 1};
  const vec4f p5{-0.5, -0.5, +0.5, 1};
  const vec4f p6{+0.5, -0.5, +0.5, 1};
  const vec4f p7{+0.5, +0.5, +0.5, 1};
  const vec4f p8{-0.5, +0.5, +0.5, 1};
  const Color c1{Color::black};
  const Color c2{Color::red};
  const Color c3{Color::yellow};
  const Color c4{Color::green};
  const Color c5{Color::blue};
  const Color c6{Color::magenta};
  const Color c7{Color::cyan};
  const Color c8{Color::white};

  // Box vertices
  static const vec4f v[]
  {
    p1, p5, p8, p4, // x = -0.5
    p2, p3, p7, p6, // x = +0.5
    p1, p2, p6, p5, // y = -0.5
    p4, p8, p7, p3, // y = +0.5
    p1, p4, p3, p2, // z = -0.5
    p5, p6, p7, p8  // z = +0.5
  };

  // Box vertex colors
  static const Color c[]
  {
    c1, c5, c8, c4, // x = -0.5
    c2, c3, c7, c6, // x = +0.5
    c1, c2, c6, c5, // y = -0.5
    c4, c8, c7, c3, // y = +0.5
    c1, c4, c3, c2, // z = -0.5
    c5, c6, c7, c8  // z = +0.5
  };

  // Box triangles
  static const GLMeshArray::Triangle t[]
  {
    { 0,  1,  2}, { 2,  3,  0},
    { 4,  5,  7}, { 5,  6,  7},
    { 8,  9, 11}, { 9, 10, 11},
    {12, 13, 14}, {14, 15, 12},
    {16, 17, 19}, {17, 18, 19},
    {20, 21, 22}, {22, 23, 20}
  };

  return new Primitive{new GLMeshArray{24, {v, 0}, {c, 1}, 12, t}};
}

} // end namespace cg

inline void
P1::buildScene()
{
	std::string name;

	_current = _scene = new Scene("Scene 1");

	// Create the Objects
	Reference<SceneObject> _box1 = objectOrBoxCreator(_scene, Box);
	Reference<SceneObject> _object1 = objectOrBoxCreator(_scene, Object);
	Reference<SceneObject> _object2 = objectOrBoxCreator(_scene, Object);
	Reference<SceneObject> _object3 = objectOrBoxCreator(_object1, Object);
	Reference<SceneObject> _object4 = objectOrBoxCreator(_object1, Object);
	Reference<SceneObject> _object5 = objectOrBoxCreator(_object2, Object);
	Reference<SceneObject> _object6 = objectOrBoxCreator(_object5, Object);
	Reference<SceneObject> _box2 = objectOrBoxCreator(_object3, Box);
	Reference<SceneObject> _box3 = objectOrBoxCreator(_object4, Box);
	Reference<SceneObject> _box4 = objectOrBoxCreator(_object6, Box);

}

void
P1::initialize()
{
  Application::loadShaders(_program, "p1.vs", "p1.fs");
  buildScene();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glLineWidth(2.0f);
  glEnable(GL_LINE_SMOOTH);
  _program.use();
}

namespace ImGui
{
  void ShowDemoWindow(bool*);
}

inline void
P1::hierarchyWindow()
{
  ImGui::Begin("Hierarchy");
  if (ImGui::Button("Create###object"))
    ImGui::OpenPopup("CreateObjectPopup");
  if (ImGui::BeginPopup("CreateObjectPopup"))
  {
    if(ImGui::MenuItem("Empty Object"))
		{
			SceneObject* objectCurrent = dynamic_cast<SceneObject*> (_current);
			if (objectCurrent != nullptr)
				auto obj = objectOrBoxCreator(Reference<SceneObject>(objectCurrent), Object);
			else
				auto obj = objectOrBoxCreator(_scene, Object);
		}
    if (ImGui::BeginMenu("3D Object"))
    {
      if (ImGui::MenuItem("Box"))
      {
				SceneObject* objectCurrent = dynamic_cast<SceneObject*> (_current);
				if (objectCurrent != nullptr)
					auto obj = objectOrBoxCreator(Reference<SceneObject> (objectCurrent), Box);
				else
					auto obj = objectOrBoxCreator(_scene, Box);
			}
      ImGui::EndMenu();
    }
    ImGui::EndPopup();
  }
	ImGui::SameLine();
	if (ImGui::Button("Delete object")){
		// Check if _current is a SceneObject
		SceneObject* objectCurrent = dynamic_cast<SceneObject*> (_current);
		if (objectCurrent != nullptr) {
			// Check parent to remove SceneObject
			auto parent = objectCurrent->parent();
			auto sceneObjectColection = &(_scene->sceneObjectColection);
			if (parent != nullptr)
			{
				sceneObjectColection = &(parent->sceneObjectColection);
				// Refresh _current
				_current = objectCurrent->parent();
			}
			else
			{
				_current = _scene;
			}
			removeSceneObject(objectCurrent);
		}
		else 
		{
			ImGui::OpenPopup("Can't delete Scene");
		}
	}
	if (ImGui::BeginPopup("Can't delete Scene"))
	{
		ImGui::MenuItem("Can't delete Scene");
		ImGui::EndPopup();
	}
  
	ImGui::Separator();
	
	ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnArrow };
  //Open the Scene
	auto open0 = ImGui::TreeNodeEx(_scene,
    _current == _scene ? flag | ImGuiTreeNodeFlags_Selected : flag,
    _scene->name());

	if (ImGui::IsItemClicked()) {
		_current = _scene;
	}
  
	// OPEN THE TREE
	if (open0) {
		hierarchyWindowRecursive(&_scene->sceneObjectColection);
	}

  ImGui::End();
}

inline void
P1::hierarchyWindowRecursive(ListColection<Reference<SceneObject>>* objectColection )
{
	ImGuiTreeNodeFlags flag{};
	ImGuiTreeNodeFlags flag1{ ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen };
	ImGuiTreeNodeFlags flag2{ ImGuiTreeNodeFlags_OpenOnArrow };

	auto begin = objectColection->beginIterador();
	auto end = objectColection->endIterador();
	for (auto objectIt = begin; objectIt != end; objectIt++) 
	{
		// Check next Level tree for each node to set Level icon
		if ((*objectIt)->sceneObjectColection.beginIterador() != (*objectIt)->sceneObjectColection.endIterador())
			flag = flag2;
		else
			flag = flag1;
		// Create Level nodes
		auto open = ImGui::TreeNodeEx(*objectIt,
			_current == *objectIt ? flag | ImGuiTreeNodeFlags_Selected : flag,
			(*objectIt)->name());

		// Check if is selected
		if (ImGui::IsItemClicked())
			_current = *objectIt;
		
		// Open the next level
		if (open && flag == flag2) {
			hierarchyWindowRecursive(&(*objectIt)->sceneObjectColection);
		}

	}
	render();
	ImGui::TreePop();
}

namespace ImGui
{ // begin namespace ImGui

void
ObjectNameInput(SceneNode* object)
{
  const int bufferSize{128};
  static SceneNode* current;
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
    transform->setLocalScale(temp);
}

} // end namespace ImGui

inline void
P1::sceneGui()
{
  auto scene = (Scene*)_current;

  ImGui::ObjectNameInput(_current);
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Colors"))
  {
    ImGui::ColorEdit3("Background", backgroundColor);
    ImGui::ColorEdit3("Selected Wireframe", selectedWireframeColor);
  }
}

inline void
P1::sceneObjectGui()
{
	//Show Transform
	auto object = (SceneObject*)_current;

	ImGui::ObjectNameInput(object);
	ImGui::SameLine();
	ImGui::Checkbox("visible", &object->visible);
	ImGui::Separator();
	if (ImGui::CollapsingHeader(object->transform()->typeName()))
	{
		auto t = object->transform();

		ImGui::TransformEdit(t);
		//_transform = t->localToWorldMatrix();
	}

	auto primitive = findPrimitive(object);

	// Has primitive
	if (primitive != nullptr)
	{
		if (ImGui::CollapsingHeader(primitive->typeName()))
		{
			ImGui::Text("Possui Primitivo");
			auto nVertices = std::to_string(primitive->mesh()->vertexCount());
			ImGui::Text(("Vertices: " + nVertices).c_str());
		}
	}
}

inline void
P1::objectGui()
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
P1::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

void
P1::gui()
{
  hierarchyWindow();
  inspectorWindow();
  /*
  static bool demo = true;
  ImGui::ShowDemoWindow(&demo);
  */
}

// Render ideia do gabriel
void
P1::render()
{
	GLWindow::render();

	auto begin = _scene->primitiveColection.beginIterador();
	auto end = _scene->primitiveColection.endIterador();
	for (auto it = begin; it != end; it++)
	{
		auto m = (*it)->mesh();

		auto obj = (*it)->sceneObject();
		auto transform = obj->transform()->localToWorldMatrix();
		_program.setUniformMat4("transform",  transform);

		m->bind();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);

		// Check if _current is a Scene to render
		SceneObject* currentObject = dynamic_cast<SceneObject*>(_current);
		if (currentObject != nullptr) 
		{
			// if currentObject = nullptr then currentScene = Scene 
			auto currentPrimitive = findPrimitive(currentObject);
			if (currentPrimitive == *it)
			{
				m->setVertexColor(selectedWireframeColor);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);
				m->useVertexColors();
			}
		}
	}
}