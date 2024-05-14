#include <algorithm>
#include <string>
#include <vector>

#include "request_queue.h"

using namespace std;

RequestQueue::RequestQueue(const SearchServer& search_server) : search_server_(search_server) {
    time_ = 0;
}

RequestQueue::QueryResult::QueryResult(vector<Document>& r, size_t t) : count(r.size()) {
    time = t;
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
    auto docs = search_server_.FindTopDocuments(raw_query, status);
    AddResults(docs);
    return docs;
}

vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
    auto docs = search_server_.FindTopDocuments(raw_query);
    AddResults(docs);
    return docs;
}

int RequestQueue::GetNoResultRequests() const {
    return static_cast<int>(count_if(requests_.begin(), requests_.end(), [](const QueryResult& item){
        return item.count == 0;
    }));
}

void RequestQueue::AddResults(vector<Document>& docs) {
    requests_.push_back(QueryResult(docs, time_));
    ++time_;
    erase_if(requests_, [&time_ = this->time_](const QueryResult& item) {
        return time_ - item.time > 24 * 60;
    });
}
