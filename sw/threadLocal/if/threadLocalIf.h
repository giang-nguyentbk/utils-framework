/*
*        ________________           ________                                                    ______  
* ____  ___  /___(_)__  /_______    ___  __/____________ _______ ___________      _________________  /__
* _  / / /  __/_  /__  /__  ___/    __  /_ __  ___/  __ `/_  __ `__ \  _ \_ | /| / /  __ \_  ___/_  //_/
* / /_/ // /_ _  / _  / _(__  )     _  __/ _  /   / /_/ /_  / / / / /  __/_ |/ |/ // /_/ /  /   _  ,<   
* \__,_/ \__/ /_/  /_/  /____/      /_/    /_/    \__,_/ /_/ /_/ /_/\___/____/|__/ \____//_/    /_/|_|  
*                                                                                                       
*/

#pragma once

#include <map>
#include <assert.h>
#include <pthread.h>

namespace UtilsFramework
{
namespace ThreadLocal
{
namespace V1
{
namespace implementation /* private namespace, do not use directly the classes inside this namespace.
                            Use UtilsFramework::ThreadLocal::V1::IThreadLocal instead */
{

/*! @brief Thread Local Storage (TLS) is the mechanism by which each thread in a given multithreaded process
* allocates storage for thread-specific data. In standard multithreaded programs, data is shared among
* all threads of a given process, whereas thread local storage is the mechanism for allocating per-thread data.
*
* By implementation based on pthread get/set_specific methods, we can allocate blocks of data which can only
* be accessed by the current local thread via associated unique tlsKeys. */
template <class T, int instanceNum = 1>
class ThreadLocalLinuxImpl
{
public:
    /*! @brief End users only need to know that this method is to allocate a new thread local data which has type of T.
    *   @return Return an address of the thread-specific data object (of type T) which is allocated in the heap and
    * is uniquely associated with its m_tlsKey. */
    static T& get()
    {
        static DefaultAllocator defaultAllocator;
        return get(defaultAllocator);
    }

    /*! @brief Creates the tlsKey and allocates T's thread-local object if not exists.
    *   @return Return an address of the thread-specific data object (of type T) which is allocated in the heap and
    * is uniquely associated with its m_tlsKey. */
    template <class Allocator>
    static T& get(Allocator& create)
    {
        createKey();
        T* instance = (T*)::pthread_getspecific(m_tlsKey);
        if(!instance)
        {
            instance = create();
            assert(instance);
            ::pthread_setspecific(m_tlsKey, instance);
        }
        return *instance;
    }

    /*! @brief You also can get a pointer to the thread-local data object instead through getPtr().
    *   @return Return a pointer to the thread-specific data object (of type T) which was allocated in the heap and
    * was uniquely associated with its m_tlsKey. Return NULL if get() request hasn't been initilized. */
    static T* getPtr()
    {
        createKey();
        return (T*)::pthread_getspecific(m_tlsKey);
    }

    /*! @brief Deletes the thread-local instance of class T and links to NULL */
    static void reset()
    {
        createKey();
        T* instance = (T*)::pthread_getspecific(m_tlsKey);
        if(instance)
        {
            delete instance;
            ::pthread_setspecific(m_tlsKey, NULL);
        }
    }

private:
    /*! @brief If key not created yet, create a TLS key with pthread_once() and pthread_key_create().
    * Key is created once and only once (only the first time).
    * Subsequent calls of createKey() will not run tlsMakeKey anymore. */
    static void createKey()
    {
        if(!m_isKeyCreated)
        {
            ::pthread_once(&m_tlsKeyOnce, ThreadLocalLinuxImpl::tlsMakeKey);
            m_isKeyCreated = true;
        }
    }

    /*! @brief Define how T will be created by default. DefaultAllocator will use "new" operator to allocate T's object
    * in the heap memory.
    *   @return Return a pointer to allocated memory with type of T. */
    class DefaultAllocator
    {
    public:
        T* operator()()
        {
            return new T;
        }
    };

    /*! @brief Create a TLS key through pthread_key_create() system call. */
    static void tlsMakeKey()
    {
        (void)::pthread_key_create(&ThreadLocalLinuxImpl::m_tlsKey, ThreadLocalLinuxImpl::tlsDestructor);
    }

    /*! @brief Destructor passed to pthread_key_create(). For any reason, thread exits before being able to call
    * reset(), this will help to deallocate thread-local data object to avoid memory leak (because thread-local data
    * maybe outside the manageable scope of thread cleanup handlers). See man 7 for more details */
    static void tlsDestructor(void* obj)
    {
        T* t = reinterpret_cast<T*>(obj);
        delete t;
    }

    static pthread_key_t m_tlsKey;
    static pthread_once_t m_tlsKeyOnce;
    static bool m_isKeyCreated;

}; // class ThreadLocalLinuxImpl

template <class T, int instanceNum>
pthread_key_t ThreadLocalLinuxImpl<T, instanceNum>::m_tlsKey = 0;

template <class T, int instanceNum>
pthread_once_t ThreadLocalLinuxImpl<T, instanceNum>::m_tlsKeyOnce = PTHREAD_ONCE_INIT;

template <class T, int instanceNum>
bool ThreadLocalLinuxImpl<T, instanceNum>::m_isKeyCreated = false;

} // namespace implementation

template <class T, int instanceNum = 1>
using IThreadLocal = implementation::ThreadLocalLinuxImpl<T, instanceNum>;

} // namespace V1

} // namespace ThreadLocal

} // namespace UtilsFramework