#pragma once

#include<unordered_map>
#include<list>
#include<string>

class Scene;
class LRUScenes
{
public:
	LRUScenes(int Capacity_) :Capacity(Capacity_) {};
	Scene* Get(std::string);
	Scene* Put(Scene*);
	void destory();

private:
	int Capacity;
	int size = 0;
	std::list< Scene*> container;
	std::unordered_map<std::string, std::list< Scene*>::iterator>cacheMap;
};