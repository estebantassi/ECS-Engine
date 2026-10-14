#include <ui/uimanager.h>

UIManager::UIManager(EngineContext* context) : m_Context(context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_IO = &ImGui::GetIO();
	m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	m_IO->ConfigFlags |= ImGuiConfigFlags_NoMouse;
	
	ImGui_ImplGlfw_InitForOpenGL(m_Context->window, true);
	ImGui_ImplOpenGL3_Init();
}

void UIManager::CreateChild(EntityID parententity, PrefabDefinition prefab)
{
	m_CurrentObjectID = prefab.instantiate(m_Context);
	Hierarchy& child = m_Context->scene->AddComponent<Hierarchy>(m_CurrentObjectID);
	child.parent = parententity;

	Hierarchy* parent = m_Context->scene->TryGetComponent<Hierarchy>(parententity);
	if (parent) {
		if (parent->firstChild != INVALID_ID) {
			Hierarchy& firstChild = m_Context->scene->GetComponent<Hierarchy>(parent->firstChild);
			firstChild.previousSibling = m_CurrentObjectID;
			child.nextSibling = parent->firstChild;
		}
		parent->firstChild = m_CurrentObjectID;
	}
	else {
		Hierarchy& newparent = m_Context->scene->AddComponent<Hierarchy>(parententity);
		newparent.firstChild = m_CurrentObjectID;
	}
}

void UIManager::RemoveChildFromParent(EntityID child)
{
	Hierarchy* childhptr = m_Context->scene->TryGetComponent<Hierarchy>(child);
	if (!childhptr) return;

	Hierarchy* oldparenthptr = m_Context->scene->TryGetComponent<Hierarchy>(childhptr->parent);
	if (!oldparenthptr) return;
	
	Hierarchy* oldnextsiblinghptr = m_Context->scene->TryGetComponent<Hierarchy>(childhptr->nextSibling);
	Hierarchy* oldprevioussiblinghptr = m_Context->scene->TryGetComponent<Hierarchy>(childhptr->previousSibling);

	if (oldnextsiblinghptr)
	{
		if (oldprevioussiblinghptr)
		{
			oldnextsiblinghptr->previousSibling = childhptr->previousSibling;
			oldprevioussiblinghptr->nextSibling = childhptr->nextSibling;
		}
		else {
			oldnextsiblinghptr->previousSibling = INVALID_ID;
		}
	}
	else {
		if (oldprevioussiblinghptr)
			oldprevioussiblinghptr->nextSibling = INVALID_ID;
	}

	if (oldparenthptr->firstChild == child) oldparenthptr->firstChild = childhptr->nextSibling;

	childhptr->nextSibling = INVALID_ID;
	childhptr->previousSibling = INVALID_ID;
	childhptr->parent = INVALID_ID;
}

void UIManager::ReparentChild(EntityID child, EntityID newparent)
{
	Hierarchy* childhptr = m_Context->scene->TryGetComponent<Hierarchy>(child);
	if (childhptr && childhptr->parent == newparent) return;
	if (IsDescendantOf(newparent, child)) return;

	RemoveChildFromParent(child);

	EntityID nextsibling = INVALID_ID;

	Hierarchy* newparenthptr = m_Context->scene->TryGetComponent<Hierarchy>(newparent);
	if (!newparenthptr) newparenthptr = &m_Context->scene->AddComponent<Hierarchy>(newparent);
	else {
		if (newparenthptr->firstChild != INVALID_ID)
		{
			Hierarchy& firstchild = m_Context->scene->GetComponent<Hierarchy>(newparenthptr->firstChild);
			nextsibling = newparenthptr->firstChild;
			firstchild.previousSibling = child;
		}
	}
	newparenthptr->firstChild = child;
	
	childhptr = m_Context->scene->TryGetComponent<Hierarchy>(child);
	if (!childhptr) childhptr = &m_Context->scene->AddComponent<Hierarchy>(child);

	childhptr->nextSibling = nextsibling;
	childhptr->parent = newparent;
}

bool UIManager::IsDescendantOf(EntityID entity, EntityID potentialAncestor)
{
	Hierarchy* h = m_Context->scene->TryGetComponent<Hierarchy>(entity);

	while (h)
	{
		if (h->parent == potentialAncestor)
			return true;

		h = m_Context->scene->TryGetComponent<Hierarchy>(h->parent);
	}

	return false;
}

void UIManager::DrawEntityNode(EntityID entity)
{	
	ImGui::PushID(entity);

	std::string name = m_Context->scene->GetComponent<Name>(entity).value;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (entity == m_CurrentObjectID) flags |= ImGuiTreeNodeFlags_Selected;

	Hierarchy* entityH = m_Context->scene->TryGetComponent<Hierarchy>(entity);
	if (!entityH || entityH->firstChild == INVALID_ID) flags |= ImGuiTreeNodeFlags_Leaf;

	bool open = ImGui::TreeNodeEx(
		"Node",
		flags,
		"%s",
		name.c_str());

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload(
			"ENTITY",
			&entity,
			sizeof(EntityID));

		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload =
			ImGui::AcceptDragDropPayload("ENTITY"))
		{
			EntityID draggedEntity =
				*(EntityID*)payload->Data;

			if (draggedEntity != entity)
			{
				m_Context->scene->CommandBuffer.QueueCommand([this, draggedEntity, entity]()
				{
					ReparentChild(draggedEntity, entity);
				});
			}
		}

		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked()) m_CurrentObjectID = entity;

	if (ImGui::BeginPopupContextItem("Sup"))
	{
		if (ImGui::BeginMenu("Add Child"))
		{
			for (auto& prefab : m_Context->prefabRegistry->GetPrefabs())
				if (ImGui::MenuItem(prefab.name.c_str()))
				{
					m_Context->scene->CommandBuffer.QueueCommand([this, prefab, entity]() {
						CreateChild(entity, prefab);
						});
				}

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Delete"))
		{
			m_Context->scene->CommandBuffer.QueueCommand([this, entity]()
			{
				//if (m_Context->scene->HasComponent<DirectionalLight>(entity)) m_Context->lightsManager->RemoveDirectionalLight();
				RemoveChildFromParent(entity);
				m_Context->scene->RemoveEntity(entity);
			});

			if (m_CurrentObjectID == entity || (m_CurrentObjectID, entity)) m_CurrentObjectID = INVALID_ID;
		}

		ImGui::EndPopup();
	}

	if (open)
	{
		Hierarchy* hierarchy = m_Context->scene->TryGetComponent<Hierarchy>(entity);
		if (hierarchy)
		{
			EntityID child = hierarchy->firstChild;
			
			while (child != INVALID_ID)
			{
				EntityID nextChild = m_Context->scene->GetComponent<Hierarchy>(child).nextSibling;

				DrawEntityNode(child);

				child = nextChild;
			}
		}

		ImGui::TreePop();
	}



	ImGui::PopID();
}

void UIManager::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(
		0,
		0,
		(float)m_Context->width,
		(float)m_Context->height
	);

	if (Input::KeyPressed(GLFW_KEY_R))
	{
		switch (m_CurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			m_CurrentGizmoOperation = ImGuizmo::ROTATE;
			break;
		case ImGuizmo::ROTATE:
			m_CurrentGizmoOperation = ImGuizmo::SCALE;
			break;
		case ImGuizmo::SCALE:
			m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
			break;
		}
	}

	//Move all of this menu into a separate menu file. Maybe even split it into scene and object menu.
	ImGui::Begin("My Window");

	ImGui::Text("FPS: %d", m_Context->fps);

	//////////////////////////
	// Instantiate Entities //
	//////////////////////////
	if (ImGui::BeginMenu("Spawn"))
	{
		for (auto& prefab : m_Context->prefabRegistry->GetPrefabs())
			if (ImGui::MenuItem(prefab.name.c_str()))
				m_CurrentObjectID = prefab.instantiate(m_Context);
		
		ImGui::EndMenu();
	}
	//////////////////////////

	ImGui::Text("Scene");

	for (EntityID entityID : m_Context->scene->GetEntities())
	{
		Hierarchy* hierarchy = m_Context->scene->TryGetComponent<Hierarchy>(entityID);

		if (!hierarchy || hierarchy->parent == INVALID_ID)
		{
			DrawEntityNode(entityID);
		}
	}

	ImGui::Dummy(ImVec2(
		ImGui::GetContentRegionAvail().x,
		ImGui::GetContentRegionAvail().y));

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload =
			ImGui::AcceptDragDropPayload("ENTITY"))
		{
			EntityID dragged =
				*(EntityID*)payload->Data;

			m_Context->scene->CommandBuffer.QueueCommand(
				[this, dragged]()
				{
					RemoveChildFromParent(dragged);
				});
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::End();

	if (m_CurrentObjectID == INVALID_ID)
		return;

	if (Input::KeyPressed(GLFW_KEY_DELETE))
	{
		//if (m_Context->scene->HasComponent<DirectionalLight>(m_CurrentObjectID)) m_Context->lightsManager->RemoveDirectionalLight();
		RemoveChildFromParent(m_CurrentObjectID);
		m_Context->scene->RemoveEntity(m_CurrentObjectID);
		m_CurrentObjectID = INVALID_ID;
		return;
	}

	glm::mat4 matrix = m_Context->scene->GetComponent<Transform>(m_CurrentObjectID).GetMatrix();
	ImGuizmo::Manipulate(
		glm::value_ptr(m_Context->player->GetView()),
		glm::value_ptr(m_Context->player->GetProjection()),
		m_CurrentGizmoOperation,
		ImGuizmo::LOCAL,
		glm::value_ptr(matrix)
	);

	if (ImGuizmo::IsUsing())
	{
		m_Context->scene->GetComponent<Transform>(m_CurrentObjectID).SetMatrix(matrix);
		m_Context->scene->UpdateTracker.MarkUpdate<Transform>(m_CurrentObjectID);
	}

	ImGui::Begin("Properties");
	for (IComponentPool* pool : m_Context->scene->GetComponentPools())
	{
		std::type_index type = pool->GetType();
		if (type == typeid(PointLight))
		{
			if (DrawProperties<PointLight>(*static_cast<ComponentPool<PointLight>*>(pool), m_CurrentObjectID, m_Context->scene))
				m_Context->scene->UpdateTracker.MarkUpdate<PointLight>(m_CurrentObjectID);
		}
		if (type == typeid(SpotLight))
		{
			if (DrawProperties<SpotLight>(*static_cast<ComponentPool<SpotLight>*>(pool), m_CurrentObjectID, m_Context->scene))
				m_Context->scene->UpdateTracker.MarkUpdate<SpotLight>(m_CurrentObjectID);
		}
		else if (type == typeid(Transform))
		{
			if (DrawProperties<Transform>(*static_cast<ComponentPool<Transform>*>(pool), m_CurrentObjectID, m_Context->scene))
				m_Context->scene->UpdateTracker.MarkUpdate<Transform>(m_CurrentObjectID);
		}
		else if (type == typeid(DirectionalLight))
		{
			if (DrawProperties<DirectionalLight>(*static_cast<ComponentPool<DirectionalLight>*>(pool), m_CurrentObjectID, m_Context->scene))
				m_Context->scene->UpdateTracker.MarkUpdate<DirectionalLight>(m_CurrentObjectID);
		}
		else if (type == typeid(Name))
		{
			if (DrawProperties<Name>(*static_cast<ComponentPool<Name>*>(pool), m_CurrentObjectID, m_Context->scene))
				m_Context->scene->UpdateTracker.MarkUpdate<Name>(m_CurrentObjectID);
		}
	}
	ImGui::End();

	//m_LastMouseState = Input::GetMouseState();
	//if (m_IO->WantTextInput)
	//{
	//	Input::SetMouseState(MouseState::UI);
	//}
	//else {
	//	if (Input::GetMouseState() != m_LastMouseState) Input::SetMouseState(m_LastMouseState);
	//}
}

void UIManager::Close()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}