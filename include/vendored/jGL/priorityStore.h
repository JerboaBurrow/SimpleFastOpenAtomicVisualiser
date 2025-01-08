#ifndef PRIORITYSTORE_H
#define PRIORITYSTORE_H

#include <memory>
#include <vector>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <cstdint>

/**
 * @brief User name for an Element.
 * @typedef ElementId
 * */
typedef std::string ElementId;

/**
 * @brief A priority for an element.
 * @typedef Priority
 */
typedef uint64_t Priority;

/**
 * @brief Store elements in a priority ordering, with identities.
 *
 * @tparam T an element, wrapped in a std::shared_ptr.
 * @remark Elements are stored in a std::vector for effficient access.
 * @remark Identities are stored for efficient lookup from string ids.
 */
template <class T>
class PriorityStore
{
public:

    /**
     * @brief Combine an id and a priority.
     *
     */
    struct Info
    {
        Info(ElementId i, Priority p)
        : id(i), priority(p)
        {}

        Info()
        : id(""), priority(0)
        {}

        ElementId id;
        Priority priority;
    };

    /**
     * @brief Construct a new Priority Store with a reserved size.
     *
     * @param sizeHint reserve this many elements.
     */
    PriorityStore(uint64_t sizeHint = 8)
    {
        idToElement.reserve(sizeHint);
        cache.reserve(sizeHint);
    }

    virtual void clear()
    {
        idToElement.clear();
        cache.clear();
    }

    /**
     * @brief Insert an element.
     *
     * @param s the element.
     * @param id its identity.
     * @param priority its priority.
     * @remark Throws std::runtime_error if the id already exists.
     */
    virtual void add(T s, ElementId id, Priority priority = 0)
    {
        if (idToElement.find(id) != idToElement.end())
        {
            throw std::runtime_error("id: "+id+", already use in PriorityStore");
        }

        idToElement[id] = std::pair(s, priority);
        auto pos = std::upper_bound
        (
            cache.begin(),
            cache.end(),
            priority,
            []
            (
                Priority p,
                std::pair<Info, T> r
            )
            {
                return p < r.first.priority;
            }
        );
        cache.insert(pos, std::pair(Info(id, priority), s));
    }

    virtual void remove(ElementId id)
    {
        if (idToElement.find(id) == idToElement.end()) { return; }

        idToElement.erase(id);

        auto pos = std::find_if
        (
            cache.begin(),
            cache.end(),
            [id](std::pair<Info, T> v)
            {
                return v.first.id == id;
            }
        );
        if (pos != cache.end())
        {
            cache.erase(pos);
        }
    }

    void updatePriority(ElementId id, Priority newPriority)
    {
        if (idToElement.find(id) == idToElement.end()){ return; }

        auto element = idToElement[id];
        remove(id);
        add(element.first, id, newPriority);
    }

    /**
     * @brief Return a vector from overriding priorities.
     *
     * @param oids overriding priorities.
     * @return std::vector<std::pair<Info, T>> cached values.
     * @remark Overriding is expensive, but useful for drawing partially with
     * modest element counts.
     */
    std::vector<std::pair<Info, T>> vectorise
    (
        std::multimap<Priority, ElementId> & oids
    )
    {
        std::vector<std::pair<Info, T>> s;
        s.reserve(oids.size());
        for (const auto & id : oids)
        {
            s.push_back(std::pair(Info(id.second, id.first), idToElement[id.second].first));
        }
        return s;
    }

    T & operator [](ElementId id) { return idToElement[id].first; }

    typename std::vector<std::pair<Info, T>>::const_iterator begin() const { return cache.cbegin(); }
    typename std::vector<std::pair<Info, T>>::const_iterator end() const { return cache.cend(); }

    uint64_t size() const { return cache.size(); }

    bool hasId(const ElementId id) const { return idToElement.find(id) != idToElement.end(); }

protected:

    // fast lookup of ids
    std::unordered_map<ElementId, std::pair<T, Priority>> idToElement;
    // contiguous for efficient drawing/iteration
    std::vector<std::pair<Info, T>> cache;
};

#endif /* PRIORITYSTORE_H */
