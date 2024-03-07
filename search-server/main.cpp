// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь: 271

// Закомитьте изменения и отправьте их в свой репозиторий.

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }

    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
   public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const map<string, double> words_tf =
            SplitIntoWordsNoStopWithTF(document);
        for (const auto& [word, tf] : words_tf) {
            documents_[word][document_count_] = tf;
        }

        document_count_ += 1;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words);
        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;
    }

   private:
    map<string, map<int, double>> documents_;
    set<string> stop_words_;
    int document_count_ = 0;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }

        return words;
    }

    map<string, double> SplitIntoWordsNoStopWithTF(const string& text) const {
        map<string, double> words_tf;
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }

        for (const string& word : words) {
            double tf =
                static_cast<double>(count(words.begin(), words.end(), word)) /
                words.size();
            words_tf[word] = tf;
        }

        return words_tf;
    }

    struct Query {
        set<string> query_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            query.query_words.insert(word);
            if (word[0] == '-') {
                query.minus_words.insert(word.substr(1));
            }
        }

        return query;
    }

    vector<Document> FindAllDocuments(const Query& query) const {
        vector<Document> matched_documents;
        map<int, double> document_relevance;
        for (const auto& word : query.query_words) {
            if (documents_.count(word)) {
                const map<int, double>& id_tf = documents_.at(word);
                for (const auto& [id, tf] : id_tf) {
                    double idf = log(static_cast<double>(document_count_) /
                                     id_tf.size());
                    document_relevance[id] += tf * idf;
                }
            }
        }

        for (const auto& m_word : query.minus_words) {
            if (documents_.count(m_word)) {
                const map<int, double>& id_tf = documents_.at(m_word);
                for (const auto& [id, tf] : id_tf) {
                    document_relevance.erase(id);
                }
            }
        }

        for (const auto& [id, relevance] : document_relevance) {
            matched_documents.push_back({id, relevance});
        }

        return matched_documents;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();
    const string query = ReadLine();
    for (const auto& [document_id, relevance] :
         search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", " << "relevance = "s
             << relevance << " }"s << endl;
    }
}