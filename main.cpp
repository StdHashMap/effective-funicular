#include <iterator>
#include <vector>
#include <stdexcept>


template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
	class DataUnit {
	public:
		std::vector<std::pair<KeyType, ValueType>> values;
	};
	Hash hasher;
	std::vector<DataUnit> data;

	class iterator {
	public:
		// using _iterator = typename std::unordered_map<KeyType, ValueType>::iterator;
		using difference_type = size_t;
		using value_type = std::pair<const KeyType, ValueType>;
		using iterator_category = typename std::forward_iterator_tag;
		using pointer = void;
		using reference = std::pair<const KeyType, ValueType>&;
		std::vector<DataUnit> *data;
		std::pair<size_t, size_t> it;
		iterator() {}
		iterator(std::vector<DataUnit> *_data) : data(_data) {}
		void go() {
			while (it.first < data->size() && it.second == ((*data)[it.first]).values.size()) {
				it.first++;
				it.second = 0;
			}
		}
		iterator(std::vector<DataUnit> *_data, std::pair<size_t, size_t> _it) : data(_data), it(_it) {
			go();
		}
		bool operator==(const iterator& sub) const {
			return it == sub.it;
		}
		bool operator!=(const iterator& sub) const {
			return !(it == sub.it);
		}
		iterator& operator++() {
			it.second++;
			go();
			return *this;
		}
		iterator operator++(int) {
			std::pair<size_t, size_t> ret = it;
			operator++();
			return iterator(data, ret);
		}
		std::pair<const KeyType, ValueType>* operator->() {
			return reinterpret_cast<std::pair<const KeyType, ValueType>*>(&(((*data)[it.first]).values[it.second]));
		}
		std::pair<KeyType, ValueType>& operator*() {
			return ((*data))[it.first].values[it.second];
		}
	};
	class const_iterator {
	public:
		// using _iterator = typename std::unordered_map<KeyType, ValueType>::iterator;
		using difference_type = size_t;
		using value_type = std::pair<const KeyType, ValueType>;
		using iterator_category = typename std::forward_iterator_tag;
		using pointer = void;
		using reference = std::pair<const KeyType, ValueType>&;
		const std::vector<DataUnit> *data;
		std::pair<size_t, size_t> it;
		void go() {
			while (it.first < data->size() && it.second == ((*data)[it.first]).values.size()) {
				it.first++;
				it.second = 0;
			}
		}
		const_iterator() {}
		const_iterator(const std::vector<DataUnit> *_data) : data(_data) {}
		const_iterator(const std::vector<DataUnit> *_data, std::pair<size_t, size_t> _it) : data(_data), it(_it) {
			go();
		}
		bool operator==(const const_iterator& sub) const {
			return it == sub.it;
		}
		bool operator!=(const const_iterator& sub) const {
			return !(it == sub.it);
		}
		const_iterator& operator++() {
			it.second++;
			go();
			return *this;
		}
		const_iterator operator++(int) {
			std::pair<size_t, size_t> ret = it;
			operator++();
			return const_iterator(data, ret);
		}
		const std::pair<KeyType, ValueType>* operator->() {
			return &(((*data)[it.first]).values[it.second]);
		}
		const std::pair<KeyType, ValueType>& operator*() {
			return ((*data))[it.first].values[it.second];
		}
	};

	size_t sz = 0;

	template<class T>
	void init(T _begin, T end) {
		T begin = _begin;
		data.clear();
		size_t d = std::distance(begin, end);
		sz = d;
		data.resize(5 * d);
		while (begin != end) {
			size_t bucket = hasher(begin->first) % data.size();
			data[bucket].values.push_back(*begin);
			begin++;
		}
	}

	// unordered_map<KeyType, ValueType> check;
	HashMap(Hash _hasher=Hash()) : hasher(_hasher) {}	

	template<class T> 
	HashMap(T begin, T end, Hash _hasher=Hash()) : hasher(_hasher) {
		init(begin, end);
	}

	HashMap(const std::initializer_list<std::pair<KeyType, ValueType>>& l, Hash _hasher=Hash()) : hasher(_hasher) {
		init(l.begin(), l.end());
	}

	size_t size() const {
		return sz;
	}
	bool empty() const {
		return sz == 0;
	}
	const Hash& hash_function() const {
		return hasher;
	}
	void insert(const std::pair<KeyType, ValueType>& pp) {
		if ((sz + 1) * 3 > data.size()) {
			std::vector<std::pair<KeyType, ValueType>> values(begin(), end());
			values.push_back(pp);
			clear();
			init(values.begin(), values.end());
		} else {
			size_t bucket = hasher(pp.first) % data.size();
			data[bucket].values.push_back(pp);
			sz++;
		}
		// check.insert(pp);
	}
	void erase(const KeyType& key) {
		if (empty()) return;
		size_t bucket = hasher(key) % data.size();
		for (size_t i = 0; i < data[bucket].values.size(); i++) {
			if (data[bucket].values[i].first == key) {
				// data[bucket].values.erase(data[bucket].values.begin() + i);
				swap(data[bucket].values[i], data[bucket].values.back());
				data[bucket].values.pop_back();
				sz--;
				if (sz * 20 < data.size()) {
					std::vector<std::pair<KeyType, ValueType>> values(begin(), end());
					clear();
					init(values.begin(), values.end());
				}
				return;
			}
		}
	}
	iterator begin() {
		return iterator(&data, std::make_pair(0, 0));
	}
	const_iterator begin() const {
		return const_iterator(&data, std::make_pair(0, 0));
	}
	iterator end() {
		if (empty()) return iterator(&data, std::make_pair(0, 0));
		return iterator(&data, std::make_pair(data.size() - 1, data.back().values.size()));
	}
	const_iterator end() const {
		if (empty()) return const_iterator(&data, std::make_pair(0, 0));
		return const_iterator(&data, std::make_pair(data.size() - 1, data.back().values.size()));
	}
	iterator find(const KeyType& key) {
		if (empty()) return end();
		size_t bucket = hasher(key) % data.size();
		for (size_t i = 0; i < data[bucket].values.size(); i++) {
			if (data[bucket].values[i].first == key) {
				return iterator(&data, std::make_pair(bucket, i));
			}
		}
		return end();
	}
	const_iterator find(const KeyType& key) const {
		if (empty()) return end();
		size_t bucket = hasher(key) % data.size();
		for (size_t i = 0; i < data[bucket].values.size(); i++) {
			if (data[bucket].values[i].first == key) {
				return const_iterator(&data, std::make_pair(bucket, i));
			}
		}
		return end();
	}
	ValueType& operator[](const KeyType& key) {
		auto it = find(key);
		if (it == end()) {
			insert(std::make_pair(key, ValueType()));
		}
		auto who = find(key);
		return who->second;
	}
	const ValueType& at(const KeyType& key) const {
		auto it = find(key);
		if (it == end())
			throw std::out_of_range("hello");
		return it->second;
	}
	void clear() {
		data.clear();
		sz = 0;
	}
};

