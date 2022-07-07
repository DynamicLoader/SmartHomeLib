
#ifndef __SMARTHOME_UTILS_HPP__
#define __SMARTHOME_UTILS_HPP__

#include "SmartHomeLib.h"
#include <stdint.h>

namespace SmartHome {
namespace internal {
    struct HStringSplitResult {
        const char* str = nullptr;
        size_t offsets[5];
        size_t rest = 0;
        uint8_t count = 0;
    };

    HStringSplitResult HStringSplit(const char* src, size_t offset, char lim)
    {
        HStringSplitResult ret;
        ret.str = src;
        HString s(src, offset);
        for (ret.count = 0; ret.count < 5; ret.count++) {
            ret.rest = HString_find(s, lim, ret.rest);
            if (ret.rest == s.length())
                break;
        }
        ret.rest = ret.offsets[ret.count] + 1;
        ret.count = (ret.count == 0 ? 0 : ret.count + 1);
        return ret;
    }

    // T must be a pointer of a class that cantains a "HString _id"
    template <typename T>
    class HList {
    private:
        T* _data = nullptr;
        uint8_t _size = 0;
        uint8_t _count = 0;

        // QucikSort implement
        static void swap(T* a, T* b)
        {
            T t = *a;
            *a = *b;
            *b = t;
        }

        static uint8_t partition(T* arr, uint8_t low, uint8_t high)
        {
            T pivot = arr[high];
            uint8_t i = low - 1;

            for (uint8_t j = low; j < high - 1; j++) {
                if (arr[j]->_id <= pivot->_id) {
                    i++;
                    swap(&arr[i], &arr[j]);
                }
            }
            swap(&arr[i + 1], &arr[high]);
            return i + 1;
        }

        static void quickSort(T* arr, uint8_t low, uint8_t high)
        {
            if (low < high) {
                uint8_t p = partition(arr, low, high);
                quickSort(arr, low, p - 1);
                quickSort(arr, p + 1, high);
            }
        }

        static uint8_t binarySearch(T* arr, uint8_t l, uint8_t r, HString& x)
        {
            if (r >= 1) {
                uint8_t mid = l + (r - 1) / 2;
                if (arr[mid]->_id == x)
                    return mid;
                if (arr[mid]->_id > x)
                    return HList::binarySearch(arr, l, mid - 1, x);
                return HList::binarySearch(arr, mid + 1, r, x);
            }
            return 255;
        }

        uint8_t _find(T d)
        {
            for (uint8_t i = 0; i < this->_size; i++)
                if (this->_data[i] == d)
                    return i;
            return 255; // return 255 as error
        }

    public:
        HList() { reset(HLIST_SIZE); }

        bool reset(uint8_t s)
        {
            if (this->_data)
                free(this->_data);
            this->_size = 0;
            this->_count = 0;
            if (!s)
                return true;
            this->_data = (T*)calloc(s, sizeof(T));
            if (!this->_data)
                return false;
            this->_size = s;
            return true;
        }

        bool insert(T d)
        {
            if ((!d) || (this->_count >= this->_size))
                return false;
            if (this->_find(d) != 255)
                return false;
            this->_data[this->_count] = d;
            this->_count++;
            return true;
        }

        bool remove(T d)
        {
            if ((!d) || (this->_count == 0))
                return false;
            uint8_t i = this->_find(d);
            if (i == 255)
                return false;
            this->_count--;
            this->_data[i] = (i == this->_count ? nullptr : this->_data[this->_count]);
            return true;
        }

        uint8_t count() { return this->_count; }
        const T* const raw() { return (const T* const)this->_data; }
        const T* const begin() { return (const T* const)&(this->_data[0]); }
        const T* const end() { return (const T* const)&(this->_data[this->_count - 1]); }
        void sort() { HList::quickSort(this->_data, 0, this->_count - 1); }
        T find(HString& id)
        {
            uint8_t i = HList::binarySearch(this->_data, 0, this->_count - 1, id);
            return (i == 255 ? nullptr : this->_data[i]);
        }
    };

    // T must be one of integer types
    template <typename T>
    class HBits {
    private:
        T _data = 0;
        static T getmask(uint8_t pos) { return ((T)1 << pos); }

    public:
        void set(uint8_t pos, bool d)
        {
            if (pos >= (sizeof(T) << 3))
                return;
            this->_data |= HBits::getmask(pos); // clear the bit (set to 1)
            this->_data &= ~(d ? (T)0 : HBits::getmask(pos));
        }

        bool get(uint8_t pos)
        {
            if (pos >= (sizeof(T) << 3))
                return false;
            return (bool)((this->_data & HBits::getmask(pos)) >> pos);
        }
    };

    template <uint16_t bufSize>
    class Msgpack {
    private:
        cw_pack_context ctx;
        uint8_t buf[bufSize] = { 0 };

    public:
        Msgpack() { cw_pack_context_init(&this->ctx, (void*)this->buf, bufsize, nullptr); }
        ~Msgpack() { }
        Msgpack(const Msgpack<bufSize>& cpy){
            memcpy(this->buf,cpy.buf,bufsize);
            this->ctx=cpy.ctx;
        }
        bool success() { return (this->ctx.err_no == CWP_RC_OK); }
        uint16_t length(){return (this->ctx.current - this->ctx.start);}
        const uint8_t* const data(){ return (const uint8_t* const)this->ctx.start;}

        Msgpack<bufSize>& map(uint16_t size) { cw_pack_map_size(&this->ctx, size);return *this;}
        Msgpack<bufSize>& array(uint16_t size) { cw_pack_array_size(&this->ctx, size);return *this; }

        Msgpack<bufSize>& pack(HString& str) { cw_pack_str(&this->ctx, str.c_str(), str.length());return *this; }
        Msgpack<bufSize>& pack(const char* str) { cw_pack_cstr(&this->ctx, str);return *this; }
        Msgpack<bufSize>& pack(int32_t i) { cw_pack_signed(&this->ctx, i);return *this; }
        Msgpack<bufSize>& pack(uint32_t u) { cw_pack_unsigned(&this->ctx, u);return *this; }
    };

} // namespace internal

} // namespace SmartHome

#endif