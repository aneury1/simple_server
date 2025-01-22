#ifndef SIMPLE_SERVER_THREAD_SAFE_CONTAINER
#define SIMPLE_SERVER_THREAD_SAFE_CONTAINER
#if 0
#include <vector>
#include <mutex>
#include <functional>

namespace simple_server{

template<typename T>
class Vector{
    std::vector<T> dataset;
    mutable std::mutex mu;

    public:

    void insert(T d){
        std::lock_guard<std::mutex>(mu);
        dataset.emplace_back(d);
    }

    T get(int idex){
        std::lock_guard<std::mutex>(mu);
        return dataset[idex];
    }
    T operator[](int idex){
        std::lock_guard<std::mutex>(mu);
        return dataset[idex];
    }

    int size(){
        std::lock_guard<std::mutex>(mu);
        return dataset.size();
    }
 
    template <typename Predicate>
    void remove_if(Predicate pred) {
        std::lock_guard<std::mutex> lock(mu); 
        data.erase(std::remove_if(data.begin(), data.end(), pred), data.end());
    }

    void apply(const std::function<void(T&)>& func) {
        std::lock_guard<std::mutex> lock(mu); 
        for (auto& item : data) {
            func(item);
        }
    }

    void print() const {
        std::lock_guard<std::mutex> lock(mu);
        for (const auto& item : data) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }

};



}

#endif
#endif