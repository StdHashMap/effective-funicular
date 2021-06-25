#include <iterator>
#include <stdexcept>
#include <vector>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
	using _value_type = std::pair<const KeyType, ValueType>;

	/*
		Stores elements with equal hash
	*/
	class DataUnit {
	public:
		std::vector<std::pair<KeyType, ValueType>> values;
	};

	/* Hash Function */
	Hash hasher;

	/*
		Stores several *DataUnits*
		Each *DataUnit* can store several elements
		if there are several elements with equal hash
	*/
	std::vector<DataUnit> data;

	/*
		iterator class 
		supports forward pass over all elements stored in the hash table
	*/
	class iterator {
	public:
		using value_type = _value_type;
		using difference_type = size_t;
		using iterator_category = typename std::forward_iterator_tag;
		using pointer = void;
		using reference = std::pair<const KeyType, ValueType>&;
		std::vector<DataUnit> *data;
		std::pair<size_t, size_t> it;

		iterator() {}
		iterator(std::vector<DataUnit> *_data) : data(_data) {}

		/*
			Finds the next element in the hash map
		*/
		void move_to_first() {
			while (it.first < data->size() && it.second == ((*data)[it.first]).values.size()) {
				it.first++;
				it.second = 0;
			}
		}

		iterator(std::vector<DataUnit> *_data, std::pair<size_t, size_t> _it) : data(_data), it(_it) {
			move_to_first();
		}

		bool operator==(const iterator& sub) const {
			return it == sub.it;
		}

		bool operator!=(const iterator& sub) const {
			return !(it == sub.it);
		}

		iterator& operator++() {
			it.second++;
			move_to_first();
			return *this;
		}

		iterator operator++(int) {
			std::pair<size_t, size_t> ret = it;
			operator++();
			return iterator(data, ret);
		}

		std::pair<const KeyType, ValueType>* operator->() {
			return reinterpret_cast<std::pair<const KeyType, ValueType>*>(
				&(this->operator*()));
		}

		std::pair<KeyType, ValueType>& operator*() {
			return ((*data))[it.first].values[it.second];
		}
	};

	/*
		const_iterator class 
		supports forward pass over all elements stored in the hash table 
	*/
	class const_iterator {
	public:
		using value_type = _value_type;
		using difference_type = size_t;
		using iterator_category = typename std::forward_iterator_tag;
		using pointer = void;
		using reference = std::pair<const KeyType, ValueType>&;
		const std::vector<DataUnit> *data;
		std::pair<size_t, size_t> it;

		/*
			Finds the next element in the hash map
		*/
		void move_to_first() {
			while (it.first < data->size() && it.second == ((*data)[it.first]).values.size()) {
				it.first++;
				it.second = 0;
			}
		}

		const_iterator() {}
		const_iterator(const std::vector<DataUnit> *_data) : data(_data) {}
		const_iterator(const std::vector<DataUnit> *_data, std::pair<size_t, size_t> _it) : data(_data), it(_it) {
			move_to_first();
		}

		bool operator==(const const_iterator& sub) const {
			return it == sub.it;
		}

		bool operator!=(const const_iterator& sub) const {
			return !(it == sub.it);
		}

		const_iterator& operator++() {
			it.second++;
			move_to_first();
			return *this;
		}

		const_iterator operator++(int) {
			std::pair<size_t, size_t> ret = it;
			operator++();
			return const_iterator(data, ret);
		}

		const std::pair<KeyType, ValueType>* operator->() {
			return &(this->operator*());
		}

		const std::pair<KeyType, ValueType>& operator*() {
			return ((*data))[it.first].values[it.second];
		}
	};

	/*
		Stores the number of elements in the hash table
	*/
	size_t sz = 0;

	/*
		Initializes the hash table with size 5 times larger than
		the number of elements
	*/
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

	/*
		Initializes empty hash table
	*/
	HashMap(Hash _hasher=Hash()) : hasher(_hasher) {}	

	/*
		Calls *init* to initialize the hash table from the iterators
	*/
	template<class T> 
	HashMap(T begin, T end, Hash _hasher=Hash()) : hasher(_hasher) {
		init(begin, end);
	}

	/*
		Calls *init* to initialize the hash table from the given initializer list
	*/
	HashMap(const std::initializer_list<std::pair<KeyType, ValueType>>& l, Hash _hasher=Hash()) : hasher(_hasher) {
		init(l.begin(), l.end());
	}

	/*
		Returns the number of elements in the hash table
	*/
	size_t size() const {
		return sz;
	}

	/*
		Returns true if the hash table is empty
	*/
	bool empty() const {
		return sz == 0;
	}

	/*
		Returns Hash Function that is used in the hash table
	*/
	const Hash& hash_function() const {
		return hasher;
	}

	/*
		Resizes hash table if the number of stored
		elements is greater than 1/3 of the hash table size

		Otherwise, adds element to an existing bucket
	*/
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
	}

	/*
		Finds and deletes the element *key* from a bucket
		And shrinks hash table if it is too large
	*/
	void erase(const KeyType& key) {
		if (empty()) return;
		size_t bucket = hasher(key) % data.size();
		for (size_t i = 0; i < data[bucket].values.size(); i++) {
			if (data[bucket].values[i].first == key) {
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

	/*
		Returns iterator on the begin
	*/
	iterator begin() {
		return iterator(&data, std::make_pair(0, 0));
	}

	/*
		Returns const iterator on the begin
	*/
	const_iterator begin() const {
		return const_iterator(&data, std::make_pair(0, 0));
	}

	/*
		Returns iterator on the end
	*/
	iterator end() {
		if (empty()) return iterator(&data, std::make_pair(0, 0));
		return iterator(&data, std::make_pair(data.size() - 1, data.back().values.size()));
	}

	/*
		Returns const iterator on the end
	*/
	const_iterator end() const {
		if (empty()) return const_iterator(&data, std::make_pair(0, 0));
		return const_iterator(&data, std::make_pair(data.size() - 1, data.back().values.size()));
	}

	/*
		Returns iterator on the element *key*
		Or end if it is not in the hash table
	*/
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

	/*
		Returns const_iterator on the element *key*
		Or end if it is not in the hash table
	*/
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

	/*
		Returns mutable link to the element *key*
	*/
	ValueType& operator[](const KeyType& key) {
		auto it = find(key);
		if (it == end()) {
			insert(std::make_pair(key, ValueType()));
		}
		auto who = find(key);
		return who->second;
	}

	/*
		Returns immutable link to the element *key*
	*/
	const ValueType& at(const KeyType& key) const {
		auto it = find(key);
		if (it == end())
			throw std::out_of_range("out of range exception");
		return it->second;
	}

	/*
		Removes all elements from the hash table
	*/
	void clear() {
		data.clear();
		sz = 0;
	}
};
