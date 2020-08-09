#include"DXUT.h"
#include"Misc.h"
#include"Scene.h"

Scene* LRUScenes::Get(std::string scene)
{
	if (cacheMap.find(scene) == cacheMap.end())
	{
		return nullptr;
	}
	else
	{
		auto it = cacheMap[scene];
		auto val = *it;
		container.erase(it);
		container.push_front(val);
		cacheMap[scene] = container.begin();
	}

	return container.front();

}
Scene* LRUScenes::Put(Scene* scene)
{
	if (Capacity == size)
	{

		cacheMap.erase(container.back()->getName());
		container.back()->OnD3D11DestroyDevice(nullptr);
		container.pop_back();
		--size;
	}
	size++;
	container.push_front(scene);
	cacheMap[scene->getName()] = container.begin();
	return container.front();
}

void LRUScenes::destory()
{
	while (!container.empty())
	{
		container.back()->OnD3D11DestroyDevice(nullptr);
		container.pop_back();
	}
	cacheMap.clear();
}