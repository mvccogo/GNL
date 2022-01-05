#pragma once
#include "NetCommon.h"


namespace SITNet {
	template <typename T>
	class NetQueue {

	public:

		NetQueue() = default;
		NetQueue(const NetQueue<T>&) = delete;
		~NetQueue() { clear(); }

		const T& get_front(){
			// Lock the mutex before performing an operation
			std::scoped_lock<std::mutex> lock(m_muxQueue);
			return m_deqQueue.front();
		}
		const T& get_back() {
			// Lock the mutex before performing an operation
			std::scoped_lock<std::mutex> lock(m_muxQueue);
			return m_deqQueue.back();
		}
		T pop_front() {
			// Lock the mutex before performing an operation
			std::scoped_lock<std::mutex> lock(m_muxQueue);
			auto t = std::move(m_deqQueue.front());
			m_deqQueue.pop_front();
			return t;
		}
		T pop_back() {
			std::scoped_lock<std::mutex> lock(m_muxQueue);
			auto t = std::move(m_deqQueue.back());
			m_deqQueue.pop_back();
			return t;
		}
		void push_back(const T& item) {
			std::scoped_lock<std::mutex> lock(m_muxQueue);
			std::unique_lock<std::mutex> ul(m_muxBlock);

			m_deqQueue.emplace_back(std::move(item));
			// Notify that an item has been added, and we no longer need to wait.
			m_cvBlock.notify_one();

		}
		void push_front(const T& item) {
			std::scoped_lock<std::mutex> lock(m_muxQueue);
			std::unique_lock<std::mutex> ul(m_muxBlock);
			m_deqQueue.emplace_front(std::move(item));
			// Notify that an item has been added, and we no longer need to wait.
			m_cvBlock.notify_one();
		}
		bool is_empty() {
			std::scoped_lock<std::mutex> lock(m_muxQueue);
			return m_deqQueue.empty();
		}
		size_t get_count() {
			std::scoped_lock<std::mutex> lock(m_muxQueue);
			return m_deqQueue.size();
		}
		void clear() {
			std::scoped_lock<std::mutex> lock(m_muxQueue);
			m_deqQueue.clear();
		}
		void wait() {
			while (is_empty())
			{
				std::unique_lock<std::mutex> ul(m_muxBlock);
				// The queue is empty. Make this thread wait until we get a signal to continue.
				m_cvBlock.wait(ul);
			}
		}
		std::mutex& GetQMutex() { return m_muxQueue; }

	protected:
		std::mutex						m_muxQueue;
		std::deque<T>					m_deqQueue;
		std::condition_variable			m_cvBlock;
		std::mutex						m_muxBlock;
	};
}