/// HEADER
#include <csapex/box_manager.h>

/// COMPONENT
#include <csapex/selector_proxy.h>
#include <csapex/command_meta.h>
#include <csapex/command_delete_box.h>
#include <csapex/box.h>
#include <csapex/graph.h>

/// SYSTEM
#include <boost/foreach.hpp>
#include <QApplication>
#include <stack>

using namespace csapex;

BoxManager::BoxManager()
    : PluginManager<csapex::BoxedObject>("csapex::BoxedObject")
{
}

void BoxManager::insertAvailableBoxedObjects(QLayout* layout)
{
    if(!pluginsLoaded()) {
        reload();
    }

    BOOST_FOREACH(SelectorProxy::Ptr p, available_elements_prototypes) {
        layout->addWidget(p->clone());
    }
}

namespace {
bool compare (SelectorProxy::Ptr a, SelectorProxy::Ptr b) {
    const std::string& as = BoxManager::stripNamespace(a->getType());
    const std::string& bs = BoxManager::stripNamespace(b->getType());
    return as.compare(bs) < 0;
}
}

void BoxManager::insertAvailableBoxedObjects(QMenu* menu)
{
    if(!pluginsLoaded()) {
        reload();
    }

    std::map<std::string, std::vector<SelectorProxy::Ptr> > map;
    std::set<std::string> categories;

    std::string no_cat = "General";

    categories.insert(no_cat);

    foreach(SelectorProxy::Ptr p, available_elements_prototypes) {
        std::string cat = p->getCategory();
        if(cat.empty()) {
            cat = no_cat;
        }
        map[cat].push_back(p);
        categories.insert(cat);
    }

    foreach(const std::string& cat, categories) {
        QMenu* submenu = new QMenu(cat.c_str());
        menu->addMenu(submenu);

        std::sort(map[cat].begin(), map[cat].end(), compare);

        foreach(const SelectorProxy::Ptr& proxy, map[cat]) {
            QIcon icon = proxy->getIcon();
            QAction* action = new QAction(stripNamespace(proxy->getType()).c_str(), submenu);
            action->setData(QString(proxy->getType().c_str()));
            if(!icon.isNull()) {
                action->setIcon(icon);
                action->setIconVisibleInMenu(true);
            }
            submenu->addAction(action);
        }
    }

    menu->menuAction()->setIconVisibleInMenu(true);

}

void BoxManager::register_box_type(SelectorProxy::Ptr provider)
{
    available_elements_prototypes.push_back(provider);
}

void BoxManager::startPlacingBox(const std::string &type, const QPoint& offset)
{
    foreach(SelectorProxy::Ptr p, available_elements_prototypes) {
        if(p->getType() == type) {
            p->startObjectPositioning(offset);
            return;
        }
    }
}

std::string BoxManager::stripNamespace(const std::string &name)
{
    size_t from = name.find_first_of("::");
    return name.substr(from != name.npos ? from + 2 : 0);
}

Box* BoxManager::makeBox(QPoint pos, const std::string& target_type, const std::string& uuid)
{
    std::string type = target_type;
    if(type.find_first_of(" ") != type.npos) {
        std::cout << "warning: type '" << type << "' contains spaces, stripping them!" << std::endl;
        while(type.find(" ") != type.npos) {
            type.replace(type.find(" "), 1, "");
        }
    }


    std::string uuid_ = uuid;
    BOOST_FOREACH(SelectorProxy::Ptr p, available_elements_prototypes) {
        if(p->getType() == type) {
            if(uuid_.empty()) {
                uuid_ = makeUUID(type);
            }
            Box* box = p->create(pos, type, uuid);
            //box->setParent(parent);
            return box;
        }
    }

    std::cout << "warning: cannot make box, type '" << type << "' is unknown, trying different namespace" << std::endl;

    std::string type_wo_ns = stripNamespace(type);

    BOOST_FOREACH(SelectorProxy::Ptr p, available_elements_prototypes) {
        std::string p_type_wo_ns = stripNamespace(p->getType());

        if(p_type_wo_ns == type_wo_ns) {
            if(uuid_.empty()) {
                uuid_ = makeUUID(type);
            }
            std::cout << "found a match: '" << type << " == " << p->getType() << std::endl;
            Box* box = p->create(pos, p->getType(), uuid);
            //box->setParent(parent);
            return box;
        }
    }

    std::cerr << "error: cannot make box, type '" << type << "' is unknown\navailable:\n";
    BOOST_FOREACH(SelectorProxy::Ptr p, available_elements_prototypes) {
        std::cerr << p->getType() << '\n';
    }
    std::cerr << std::endl;
    return NULL;
}


void BoxManager::setContainer(QWidget *c)
{
    container_ = c;
}

QWidget* BoxManager::container()
{
    return container_;
}

std::string BoxManager::makeUUID(const std::string& name)
{
    int& last_id = uuids[name];
    ++last_id;

    std::stringstream ss;
    ss << name << "_" << last_id;

    return ss.str();
}