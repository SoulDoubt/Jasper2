#pragma once

#include <functional>
#include <vector>
#include <algorithm>

template<typename... Args>
class Event {

	typedef std::function<void(Args...)> handler;

public:

	void Call(Args... args) {
		for (auto i = std::begin(m_handlers); i != std::end(m_handlers); i++) {
			(*i)(std::forward<Args>(args)...);
		}
	}

	Event& operator+=(handler h) {
		m_handlers.push_back(h);
		return *this;
	}

	Event& operator-=(handler h) {
		for (auto i = std::begin(m_handlers); i != std::end(m_handlers); i++) {
			
		}
		return *this;
	}



private:

	std::vector<handler> m_handlers;
};
