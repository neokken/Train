#pragma once


template <typename T>
class IDGenerator {
public:
    IDGenerator() : m_nextID(1) {}

    T GenerateID() {
        return static_cast<T>(m_nextID++);  // Convert to T (ID type)
    }

private:
    uint m_nextID;
};