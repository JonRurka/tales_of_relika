#include "Component.h"

#include <boost/algorithm/string.hpp>

#include "WorldObject.h"
#include "Component.h"
#include "Camera.h"
#include "Light.h"
#include "MeshRenderer.h"
#include "Scene.h"

WorldObject& Component::Object()
{
    assert(!m_object.expired());
    return *m_object.lock();
}

Component* Component::Load_Component(WorldObject* obj, json data)
{
    /*
    Component* res = nullptr;

    std::string type;
    data.get_to(type);

    boost::algorithm::to_lower(type);

    if (type == "light") {
        res = obj->Add_Component<Light>();
        res->Load(data);
    }
    else if (type == "camera") {
        res = obj->Add_Component<Camera>();
        res->Load(data);
    }
    else {

    }


    return res;*/
}

void Component::Destroy()
{
    OnDestroy();
}

std::weak_ptr<WorldObject> Component::Instantiate() {
    return Object().scene().Instantiate();
}

std::weak_ptr<WorldObject> Component::Instantiate(std::string name) {
    return Object().scene().Instantiate(name);
}
