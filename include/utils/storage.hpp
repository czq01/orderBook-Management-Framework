#ifndef __CZQ01_FILE_STORAGE__
#define __CZQ01_FILE_STORAGE__
#include "../base.hpp"


bool check_create_folder() {
    // TODO use a folder to store data.
    return false;
}

static int _file_count = 0;

//store acsending array on disk, and fast find the index of a specific value.
template <typename T, uint _ChunkSize=0x400>
class DiskSearchArray {
    FILE * m_file;                      // store by chunks
    std::vector<T> m_header;            // the first value of each chunk (for binary search)
    std::array<T, _ChunkSize> m_arr;    // current chunk (may not fullfilled)
    int m_pointer = 0;                  // the size of current chunk.

    // write current chunk to file, start new chunk
    void _start_new_line() {
        fwrite((void *)&m_arr, sizeof(m_arr), 1, m_file);
        m_pointer = 0;
        m_header.emplace_back(m_arr[0]);
    }

public:
    int size;
    char file_name[15]{0};

    // TODO:  DiskSearchArray(config) load local data after interuption
    // DiskSearchArray(int _file_idx) {}

    DiskSearchArray(): size(0) {
        sprintf(file_name, "%05d_arr_val", _file_count);
        m_file = fopen(file_name, "w+");
        _file_count++;
    }

    // append new data to this storage class
    // return 1 if discarded due to duplicate
    bool add_data(const T& val) {
        if (m_arr[m_pointer]==val) return true;
        m_arr[m_pointer] = val;
        m_pointer++;
        if (m_pointer==_ChunkSize) _start_new_line();
        size++;
        return false;
    }

    // To get highest idx of which stored value<=val.
    // this function use binary search
    unsigned int get_idx(T val) const {
        // perform binary search in m_header
        unsigned int size = m_header.size();
        if (m_pointer) size++;
        unsigned int s=0, e=size-1;
        while (e-s>1U) {
            unsigned int m = (s+e)/2U;
            if (m_header[m]<=val) {s=m;}
            else {e=m;}
        }
        unsigned int shift;
        if (m_header[e]<=val) shift = e;
        else shift = s;

        // perform binary search in the data line find above
        std::array<T, _ChunkSize> tmp_arr;
        if (shift<m_header.size()) {
            fseek(m_file, shift*sizeof(tmp_arr), SEEK_SET);
            fread(static_cast<void*>(&tmp_arr), sizeof(tmp_arr), 1, m_file);
            fseek(m_file, 0, SEEK_END);
            s=0U; e=_ChunkSize-1U;
        } else {
            tmp_arr = m_arr;
            s=0U; e=m_pointer-1U;
        }
        while (e-s>1) {
            unsigned int m = (s+e)/2U;
            if (tmp_arr[m]<=val) {s=m;}
            else {e=m;}
        }
        if (tmp_arr[e]<=val) return shift*_ChunkSize+e;
        else return shift*_ChunkSize+s;
    }

    // clear all data
    void clear() {
        fclose(m_file);
        m_file = fopen(file_name, 'w');
        m_header.clear();
        size = 0;
        m_pointer = 0;
    }

    // save the data and close file
    ~DiskSearchArray() {
        fwrite(static_cast<void*>(&m_arr), sizeof(T), m_pointer, m_file);
        fclose(m_file);
    }
};


// Use to Store Data Array in Several Files.
// File split by _SplitSize.  Each File will not be too large.
template <typename _T, uint _SplitSize=0x100000>
class DiskArray {
    __uint32_t m_curr_size;  // current file records number
    __uint32_t m_next_idx;   // idx of next file to be created
    __uint64_t total_size;   // total records stored;
    std::vector<FILE*> m_files;

    // create new file
    FILE * _create_file() {
        char name[16];
        sprintf(name, "%s_%05d", symbol, m_next_idx);
        m_next_idx++;
        return fopen(name, "w+");
    }

public:
    char symbol[8]{0};

    // TODO load current disk data
    // DiskArray(file_prefix)

    DiskArray(const char * symbol):
        m_curr_size(0), m_next_idx(0), total_size(0) {
            std::memcpy(this->symbol, symbol, std::strlen(symbol));
            m_files.emplace_back(_create_file());
    }

    // get record data according to idx.
    void fetchRecord(void * dst, u_int64_t offset) const {
        FILE * f = m_files[offset/_SplitSize];
        offset = offset%_SplitSize;
        fseek(f, offset*sizeof(_T), SEEK_SET);
        fread(dst, sizeof(_T), 1, f);
        fseek(f, 0, SEEK_END);
    }

    // append record at the back of array
    void insertRecord(const _T& obj) {
        fwrite(static_cast<const void*>(&obj), sizeof(_T), 1, m_files.back());
        total_size++; m_curr_size++;
        if (m_curr_size==_SplitSize) {m_files.emplace_back(_create_file());}
    }

    ~DiskArray() {
        for (FILE * p: m_files) {fclose(p);}
    }
};



class DiskOrderManager {
    DiskArray<OrderBook> m_books;
    DiskSearchArray<__uint64_t> m_idx_arr;

public:
    char symbol[8];

    DiskOrderManager(const char * symbol):
        m_books(symbol) {

    }

    const OrderBook get_snapshot(__uint64_t time) const {
        OrderBook book;
        unsigned long idx = m_idx_arr.get_idx(time);
        m_books.fetchRecord(static_cast<void*>(&book), idx);
        return book;
    }

    void add_new_data(const OrderBook& ob) {
        m_idx_arr.add_data(ob.epoch);
        m_books.insertRecord(ob);
    }

};



#endif