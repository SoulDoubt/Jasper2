#pragma once

#include <vector>
#include <memory>
#include <algorithm>


namespace Jasper
{

class Resource {

public:

	enum class ResourceType {
		Mesh,
		Texture,
		Skeleton,
		Material,
		ModelData
	};

	virtual ResourceType GetResourceType() = 0;


};

template<typename T>
class ResourceManager
{

	using Cache = std::vector<std::unique_ptr<T>>;

private:

	Cache m_cache;

public:

	ResourceManager() {
		m_cache.reserve(20);
	}

	template<typename U, typename... Args>
	U* CreateInstance(Args&&... args) {
		static_assert(std::is_base_of<T, U>::value, "Instance creation must be of correct type");
		std::unique_ptr<U> instance = std::make_unique<U>(std::forward<Args>(args)...);
		U* ret = instance.get();
		m_cache.push_back(std::move(instance));
		return ret;
	}

	T* GetResourceByName(const std::string& name) {
		auto found = std::find_if(std::begin(m_cache), std::end(m_cache),
			[&name](const std::unique_ptr<T>& obj) {
			return obj->GetName() == name;
		}
		);
		if (found != std::end(m_cache)) {
			return found->get();
		}
		return nullptr;
	}

	void Clear() {
		m_cache.clear();
	}

	Cache& GetCache() {
		return m_cache;
	}

	const Cache& GetCache() const {
		return m_cache;
	}

	T* GetLatestInstanceAdded() const {
		if (m_cache.size() > 0) {
			return m_cache[m_cache.size() - 1].get();
		}
		return nullptr;
	}

	T* GetInstance(int index) const {
		assert(index < m_cache.size());
		return m_cache[index].get();
	}

	const size_t GetSize() const {
		return m_cache.size();
	}
};

}
