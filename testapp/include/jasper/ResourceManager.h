#ifndef _JASPER_RESOURCE_MANAGER_H_
#define	_JASPER_RESOURCE_MANAGER_H_	

#include <vector>
#include <memory>

namespace Jasper {

template<typename T>
class ResourceManager {

	using Cache = std::vector<std::unique_ptr<T>>;

private:

	Cache m_cache;

public:

	ResourceManager() {
		m_cache.reserve(20);
	}

	template<typename U, typename... Args>
	U* CreateInstance(Args&&... args) {
		if (!std::is_base_of<T, U>::value) {
			return nullptr;
		}
		std::unique_ptr<U> instance = std::make_unique<U>(std::forward<Args>(args)...);
		U* ret = instance.get();
		m_cache.push_back(std::move(instance));
		return ret;
	}
    
    T* GetResourceByName(const std::string& name){
        auto found = std::find_if(begin(m_cache), end(m_cache), 
        [& name](const std::unique_ptr<T>& obj){
            return obj->GetName() == name;
        }
        );
    }

	void Clear() {
		m_cache.clear();
	}

	Cache& GetCache() {
		return m_cache;
	}

	T* GetLatestInstanceAdded() {
		if (m_cache.size() > 0) {
			return m_cache[m_cache.size() - 1].get();
		}
		return nullptr;
	}

	T* GetInstance(int index) {
		assert(index < m_cache.size());
		return m_cache[index].get();
	}

	size_t GetSize() {
		return m_cache.size();
	}
};

}

#endif // !_JASPER_RESOURCE_MANAGER_H_

