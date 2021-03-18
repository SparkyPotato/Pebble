#pragma once

template<typename T>
class MPMCQueue
{
public:
	MPMCQueue() = default;

	MPMCQueue(u64 size)
	{
		size--;
		size |= size >> 1;
		size |= size >> 2;
		size |= size >> 4;
		size |= size >> 8;
		size |= size >> 16;
		size |= size >> 32; // Round size to next multiple of 2 - 1.

		m_Mask = size;
		m_Capacity = size + 1;
		m_Slots = new Slot<T>[m_Capacity + 1];
	}

	MPMCQueue(const MPMCQueue<T>& other) = delete;

	~MPMCQueue()
	{
		if (!m_Slots)
		{
			return;
		}

		for (u64 i = 0; i < m_Capacity; i++)
		{
			m_Slots[i].~Slot<T>();
		}

		delete[] m_Slots;
	}

	MPMCQueue<T>& operator=(MPMCQueue<T>&& other)
	{
		this->~MPMCQueue<T>();

		m_Slots = other.m_Slots;
		other.m_Slots = nullptr;
		m_Capacity = other.m_Capacity;
		m_Head = other.m_Head.load();
		m_Tail = other.m_Tail.load();
		m_Mask = other.m_Mask;

		return *this;
	}

	template<typename... Args>
	T& Emplace(Args&&... args)
	{
		u64 head = m_Head.fetch_add(1);
		auto& slot = m_Slots[Index(head)];
		while (Turn(head) * 2 != slot.Turn.load(std::memory_order::acquire)) {}

		auto ptr = new (&slot.Storage) T(std::forward<Args>(args)...);
		slot.Turn.store(Turn(head) * 2 + 1, std::memory_order::release);

		return *ptr;
	}

	template<typename... Args>
	bool TryEmplace(Args&&... args)
	{
		u64 head = m_Head.load(std::memory_order::acquire);
		while (true)
		{
			auto& slot = m_Slots[Index(head)];
			if (Turn(head) * 2 == slot.Turn.load(std::memory_order::acquire))
			{
				if (m_Head.compare_exchange_strong(head, head + 1))
				{
					new (&slot.Storage) T(std::forward<Args>(args)...);
					slot.Turn.store(Turn(head) * 2 + 1, std::memory_order::release);

					return true;
				}
			}
			else
			{
				u64 prev = head;
				head = m_Head.load(std::memory_order::acquire);
				if (head == prev) // Another thread didn't win, the queue is just full.
				{
					return false;
				}
			}
		}
	}

	T& Push(const T& obj) { return Emplace(obj); }

	T& Push(T&& obj) { return Emplace(std::move(obj)); }

	bool TryPush(const T& obj) { return TryEmplace(obj); }

	bool TryPush(T&& obj) { return TryEmplace(Move(obj)); }

	T Pop()
	{
		u64 tail = m_Tail.fetch_add(1);
		auto& slot = m_Slots[Index(tail)];
		while (Turn(tail) * 2 + 1 != slot.Turn.load(std::memory_order::acquire)) {}

		auto temp = std::move(slot.Storage);
		slot.Destroy();
		slot.Turn.store(Turn(tail) * 2 + 2, std::memory_order::release);

		return temp;
	}

	bool TryPop(T& obj)
	{
		u64 tail = m_Tail.load(std::memory_order::acquire);
		while (true)
		{
			auto& slot = m_Slots[Index(tail)];
			if (Turn(tail) * 2 + 1 == slot.Turn.load(std::memory_order::acquire))
			{
				if (m_Tail.compare_exchange_strong(tail, tail + 1))
				{
					obj = std::move(slot.Storage);
					slot.Destroy();
					slot.Turn.store(Turn(tail) * 2 + 2, std::memory_order::release);

					return true;
				}
			}
			else
			{
				u64 prev = tail;
				tail = m_Tail.load(std::memory_order::acquire);
				if (tail == prev)
				{
					return false;
				}
			}
		}
	}

private:
	template<typename S>
	class Slot
	{
	public:
		~Slot()
		{
			if (Turn & 1)
			{
				Destroy();
			}
		}

		void Destroy() { Storage.~S(); }

		alignas(64) std::atomic<u64> Turn = 0;
		S Storage;
	};

	constexpr u64 Index(u64 i) { return i & m_Mask; }
	constexpr u64 Turn(u64 i) { return i / m_Capacity; }

	Slot<T>* m_Slots = nullptr;
	u64 m_Capacity = 0;
	u64 m_Mask = 0;

	alignas(64) std::atomic<u64> m_Head = 0;
	alignas(64) std::atomic<u64> m_Tail = 0;
};
