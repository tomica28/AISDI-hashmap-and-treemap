#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include<vector>
#include<stack>

namespace aisdi {

    template<typename KeyType, typename ValueType>
    class HashMap {
    public:
        using key_type = KeyType;
        using mapped_type = ValueType;
        using value_type = std::pair<const key_type, mapped_type>;
        using size_type = std::size_t;
        using reference = value_type &;
        using const_reference = const value_type &;
        using Vector = std::vector<std::pair<const key_type, mapped_type>>;
        class ConstIterator;
        class Iterator;
        using iterator = Iterator;
        using const_iterator = ConstIterator;
        friend class ConstIterator;
        friend class Iterator;
    private:
        size_type lenght;
        size_type max_lenght;
        Vector * tab;
        std::hash<key_type> make_hash;

        void Remove(int a, int b)
        {
            std::stack<std::pair<const key_type, mapped_type>> stc;
            int temp = tab[b].size()-1;
            while(a < temp)
            {
                stc.push(tab[b].back());
                tab[b].pop_back();
                temp--;
            }
            tab[b].pop_back();
            while(stc.size())
            {
                tab[b].push_back(stc.top());
                stc.pop();
            }
        }

    public:

        HashMap()
        {
            lenght = 0;
            max_lenght = 100000;
            tab = new Vector[max_lenght];
        }

        ~HashMap(){
            delete [] tab;
            lenght = 0;
            max_lenght = 0;
        }

        HashMap(std::initializer_list<value_type> list): HashMap() {
            for(auto i = list.begin(); i != list.end(); i++)
                this->operator[](i->first) = i->second;
        }

        HashMap(const HashMap &other): HashMap() {
            for(auto i = other.begin(); i != other.end(); i++)
                this->operator[](i->first) = i->second;
        }

        HashMap(HashMap &&other):HashMap() {
            std::swap(lenght, other.lenght);
            std::swap(max_lenght, other.max_lenght);
            std::swap(tab, other.tab);
        }

        HashMap& operator=(const HashMap& other) {
            if(other == *this)
                return *this;
            lenght = 0;
            delete [] tab;
            tab = new Vector[max_lenght];
            for(auto i = other.begin(); i != other.end(); i++)
                this->operator[](i->first) = i->second;
            return *this;
        }

        HashMap& operator=(HashMap&& other){
            if(other == *this)
                return *this;
            lenght = 0;
            delete [] tab;
            tab = new Vector[max_lenght];
            std::swap(lenght, other.lenght);
            std::swap(max_lenght, other.max_lenght);
            std::swap(tab, other.tab);
            return *this;
        }

        bool isEmpty() const {
            return lenght == 0;
        }


        mapped_type &operator[](const key_type &key) {
            int temp = make_hash(key) % max_lenght;
            for(size_type i = 0; i < tab[temp].size(); i++)
            {
                if(tab[temp][i].first == key)
                    return tab[temp][i].second;
            }
            tab[temp].push_back(value_type(key, mapped_type{}));
            ++lenght;
            return tab[temp].back().second;
        }

        const mapped_type &valueOf(const key_type &key) const {
            int temp = make_hash(key) % max_lenght;
            if(tab[temp].empty())
                throw std::out_of_range("No item with this key");
            for(size_type i = 0; i < tab[temp].size(); i++)
            {
                if(tab[temp][i].first == key)
                    return tab[temp][i].second;
            }
            throw std::out_of_range("No item with this key");
        }

        mapped_type &valueOf(const key_type &key) {
            int temp = make_hash(key) % max_lenght;
            if(tab[temp].empty())
                throw std::out_of_range("No item with this key");
            for(size_type i = 0; i < tab[temp].size(); i++)
            {
                if(tab[temp][i].first == key)
                    return tab[temp][i].second;
            }
            throw std::out_of_range("No item with this key");
        }

        const_iterator find(const key_type &key) const {
            int temp = make_hash(key) % max_lenght;
            if(lenght == 0 || tab[temp].empty())
                return this->end();
            ConstIterator it;
            it.tab = tab;
            it.num = temp;
            it.pos = tab[temp].size();
            for(size_type i = 0; i < tab[temp].size(); i++)
            {
                if(tab[temp][i].first == key)
                {
                    it.pos = i;
                    break;
                }
            }
            return it;
        }

        iterator find(const key_type &key) {
            return Iterator(this->find(key));
        }

        void remove(const key_type &key) {
            int temp = make_hash(key) % lenght;
            if(lenght == 0 || tab[temp].empty())
                throw std::out_of_range("Removing non-existing element");
            int k = 0;
            for(auto i = tab[temp].begin(); i != tab[temp].end(); i++, k++)
            {
                if(i->first == key)
                {
                    Remove(k, temp);
                    lenght--;
                    break;
                }
            }
        }

        void remove(const const_iterator &it) {
            if(tab[it.num].empty() || it.pos >= (int)tab[it.num].size() || lenght == 0)
                throw std::out_of_range("Removing non-existing element");
            Remove(it.pos, it.num);
            lenght--;
        }

        int getSize() const {
            return lenght;
        }

        bool operator==(const HashMap &other) const {
            if(lenght != other.lenght)
                return false;
            for(size_type i = 0; i < max_lenght; i++)
            {
                if(tab[i] != other.tab[i])
                    return false;
            }
            return true;
        }

        bool operator!=(const HashMap &other) const {
            return !(*this == other);
        }

        iterator begin() {
            return Iterator(ConstIterator(this->cbegin()));
        }

        iterator end() {
            return Iterator(ConstIterator(this->cend()));
        }

        const_iterator cbegin() const {
            ConstIterator it;
            it.tab = tab;
            it.pos = 0;
            if(lenght == 0)
                it.num = 0;
            else
            {
                size_type i = 0;
                while(i < max_lenght && tab[i].size() == 0)
                    i++;
                it.num = i;
            }
            return it;
        }

        const_iterator cend() const {
            ConstIterator it;
            it.tab = tab;
            it.pos = 0;
            if(lenght == 0)
                it.num = 0;
            else
            {
                it.num = max_lenght - 1;
                while(it.num >=0 && tab[it.num].size() == 0)
                    it.num--;
                it.num++;
            }
            return it;
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator end() const {
            return cend();
        }
    };

    template<typename KeyType, typename ValueType>
    class HashMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename HashMap::const_reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename HashMap::value_type;
        using pointer = const typename HashMap::value_type *;
        using Vector = std::vector<std::pair<const key_type, mapped_type>>;
        friend HashMap;
    private:
        int max_lenght = 100000;
        Vector * tab;
        int num;
        int pos;
    public:
        explicit ConstIterator():tab(nullptr), num(0), pos(0)
        {}

        ConstIterator(Vector * vec, int n, int p)
        {
            tab = vec;
            num = n;
            pos = p;
        }

        ConstIterator(const ConstIterator &other) {
            tab = other.tab;
            num = other.num;
            pos = other.pos;
        }

        ConstIterator &operator++() {
            int temp = num;
            if(tab[num].empty())
                throw std::out_of_range("Iterator operator++");
            if((int)tab[num].size() - 1 > pos)
            {
                pos++;
                return *this;
            }
            else
            {
                num++;
                pos = 0;
                while(num < max_lenght - 1 && tab[num].empty())
                    num++;
                if(num == max_lenght - 1)
                    num = temp;
                return *this;
            }
        }

        ConstIterator operator++(int) {
            ConstIterator temp(tab, num, pos);
            this->operator++();
            return temp;
        }

        ConstIterator &operator--() {
            if(tab[num].size() == 0 && num == 0)
                throw std::out_of_range("Iterator operator--");
            if(pos > 0)
            {
                pos--;
                return *this;
            }
            else
            {
                num--;
                while(num >= 0 && tab[num].empty())
                    num--;
                if(num == -1)
                    throw std::out_of_range("Iterator operator--");
                pos = tab[num].size() - 1;
            }
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator temp(tab, num, pos);
            this->operator--();
            return temp;
        }

        reference operator*() const {
            if(tab[num].empty())
                throw std::out_of_range("Iterator operator*");
            return tab[num][pos];
        }

        pointer operator->() const {
            return &this->operator*();
        }

        bool operator==(const ConstIterator &other) const {
            if(num == other.num && pos == other.pos && tab == other.tab)
                return true;
            return false;
        }

        bool operator!=(const ConstIterator &other) const {
            return !(*this == other);
        }
    };

    template<typename KeyType, typename ValueType>
    class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename HashMap::reference;
        using pointer = typename HashMap::value_type *;

        explicit Iterator() {}

        Iterator(const ConstIterator &other)
                : ConstIterator(other) {}

        Iterator &operator++() {
            ConstIterator::operator++();
            return *this;
        }

        Iterator operator++(int) {
            auto result = *this;
            ConstIterator::operator++();
            return result;
        }

        Iterator &operator--() {
            ConstIterator::operator--();
            return *this;
        }

        Iterator operator--(int) {
            auto result = *this;
            ConstIterator::operator--();
            return result;
        }

        pointer operator->() const {
            return &this->operator*();
        }

        reference operator*() const {
            // ugly cast, yet reduces code duplication.
            return const_cast<reference>(ConstIterator::operator*());
        }
    };


}

#endif /* AISDI_MAPS_HASHMAP_H */
