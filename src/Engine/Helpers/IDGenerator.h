#pragma once

namespace Engine
{
	template <typename T>
	class IDGenerator
	{
	public:
		IDGenerator() : m_nextID(1)
		{
		}

		T GenerateID()
		{
			return static_cast<T>(m_nextID++); // Convert to T (ID type)
		}

		uint GetSerializableData() const { return m_nextID; }
		void SetSerializableData( const uint nextID ) { m_nextID = nextID; }

	private:
		uint m_nextID;
	};
}
