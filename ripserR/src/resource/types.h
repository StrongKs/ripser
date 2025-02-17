//
// Created by Kent Phipps on 2/16/25.
//

#ifndef RIPSERR_TYPES_H
#define RIPSERR_TYPES_H

// Type definitions
typedef int64_t index_t;
typedef float value_t;
typedef int coefficient_t;
typedef std::pair<index_t, value_t> index_diameter_t;

typedef index_t entry_t;
const index_t get_index(const entry_t& i) { return i; }
index_t get_coefficient(const entry_t& i) { return 1; }
entry_t make_entry(index_t _index, coefficient_t _value) { return entry_t(_index); }
void set_coefficient(entry_t& e, const coefficient_t c) {}

const entry_t& get_entry(const entry_t& e) { return e; }

typedef std::pair<value_t, index_t> diameter_index_t;
value_t get_diameter(const diameter_index_t& i) { return i.first; }
index_t get_index(const diameter_index_t& i) { return i.second; }

typedef std::pair<index_t, value_t> index_diameter_t;
index_t get_index(const index_diameter_t& i) { return i.first; }
value_t get_diameter(const index_diameter_t& i) { return i.second; }

struct diameter_entry_t : std::pair<value_t, entry_t> {
    using std::pair<value_t, entry_t>::pair;
    diameter_entry_t(value_t _diameter, index_t _index, coefficient_t _coefficient)
            : diameter_entry_t(_diameter, make_entry(_index, _coefficient)) {}
    diameter_entry_t(const diameter_index_t& _diameter_index, coefficient_t _coefficient)
            : diameter_entry_t(get_diameter(_diameter_index),
                               make_entry(get_index(_diameter_index), _coefficient)) {}
    diameter_entry_t(const diameter_index_t& _diameter_index)
            : diameter_entry_t(get_diameter(_diameter_index),
                               make_entry(get_index(_diameter_index), 0)) {}
    diameter_entry_t(const index_t& _index) : diameter_entry_t(0, _index, 0) {}
};

const entry_t& get_entry(const diameter_entry_t& p) { return p.second; }
entry_t& get_entry(diameter_entry_t& p) { return p.second; }
const index_t get_index(const diameter_entry_t& p) { return get_index(get_entry(p)); }
const coefficient_t get_coefficient(const diameter_entry_t& p) {
    return get_coefficient(get_entry(p));
}
const value_t& get_diameter(const diameter_entry_t& p) { return p.first; }
void set_coefficient(diameter_entry_t& p, const coefficient_t c) {
    set_coefficient(get_entry(p), c);
}

// ripser<dis matrix> used in compute_parse function
template <typename Entry> struct greater_diameter_or_smaller_index_comp {
    bool operator()(const Entry& a, const Entry& b) {
        return greater_diameter_or_smaller_index(a, b);
    }
};

// ripser<dis matrix> used in compute_parse function
template <typename Entry> bool greater_diameter_or_smaller_index(const Entry& a, const Entry& b) {
    return (get_diameter(a) > get_diameter(b)) ||
           ((get_diameter(a) == get_diameter(b)) && (get_index(a) < get_index(b)));
}



#endif //RIPSERR_TYPES_H
