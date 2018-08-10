#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template<class T>

class Singleton
{
    public:
        static inline T& GetInstance() {
            static T instance;
            return instance;
        }

    protected:
        Singleton() {}
        
    private:
        void operator=(const Singleton& obj) {}
        Singleton(const Singleton &obj) {}
};

#endif
