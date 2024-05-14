#pragma once

#include <iostream>
#include <utility>
#include <vector>

template <typename It>
class Paginator {
public:
    Paginator(It begin, It end, size_t page_size);
    
    auto begin() const {
        return pages_.begin();
    }
    
    auto end() const {
        return pages_.end();
    }
    
    
private:
    std::vector<std::pair<It, It>> pages_;
};

template <typename It>
Paginator<It>::Paginator(It begin, It end, size_t page_size) {
    for (It current = begin; current != end;) {
        size_t dist = distance(current, end);
        if (page_size < dist) {
            dist = page_size;
        }
        It next = current + dist;
        pages_.push_back(make_pair(current, next));
        current = next;
    }
}

template <typename It>
std::ostream& operator<< (std::ostream& os, const std::pair<It, It>& page) {
    for (It it = page.first; it != page.second; ++it) {
        os << "{ document_id = " << it->id << ", relevance = " << it->relevance << ", rating = " << it->rating << " }";
    }
    return os;
}

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}
