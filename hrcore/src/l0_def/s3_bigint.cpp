#include "pch.h"

#include "s3_bigint.hpp"


hr::def::bigint::bigint():data(), flags()
{
}

hr::def::bigint::bigint(const bigint& other): data(other.data), flags(other.flags)
{

}

hr::def::bigint::bigint(bigint&& other)  noexcept : data(std::move(other.data)), flags(other.flags)
{
}

hr::def::bigint::bigint(const char* formatted_bigint)
{
    set(formatted_bigint);
}

bool hr::def::bigint::is_positive() const
{
    return flags[0];
}

bool hr::def::bigint::is_infinity() const
{
    return flags[1];
}

bool hr::def::bigint::is_nan() const
{
    return flags[2];
}

void hr::def::bigint::set(const char* const& formatted_bigint)
{
    set_to_zero();
    const char* cur_ite = formatted_bigint;
    
}

void hr::def::bigint::set(const i64& val)
{
    if (val >= 0)
    {
        _data_set_ui64(static_cast<ui64>(val));
        _set_positive(true);
    }
    else
    {
        const ui64 pos_val = -val;
        _data_set_ui64(pos_val);
        _set_positive(false);
    }

}

void hr::def::bigint::set(const ui64& val)
{
    _data_set_ui64(val);
    _set_positive(true);
}

void hr::def::bigint::set_to_zero()
{
    data.clear();
    flags.reset();
}

hr::def::bigint hr::def::bigint::operator+(const bigint& right) const
{
    bigint res; // zero

    auto linf = this->is_infinity();
    auto rinf = right.is_infinity();

    if (linf || rinf)
    {
        if (linf && rinf)
        {
            if (is_positive() == right.is_positive())
            {
                res._set_infinity(is_positive() ? 1i8 : -1i8);
            }
            else
            {
                // nothing
            }
            return res;
        }
        else
        {
            if (linf) {

            }
        }
        
    }

    res = *this;

    if (this->is_positive() == right.is_positive())
    {
        res._data_add_from(right.data);
    }
    else
    {
        if (this->is_positive())
        {
            res._set_positive(res._data_sub_from(right.data));
        }
        else
        {
            res._set_positive(!res._data_sub_from(right.data));
        }
    }

    res._trim();
    return res;
}

void hr::def::bigint::operator=(const bigint& right)
{
    this->data = right.data;
    this->flags = right.flags;
}



hr::def::i8 hr::def::bigint::operator<=>(const bigint& right) const
{
    auto lpos = this->is_positive();
    auto rpos = right.is_positive();

    if (lpos == rpos)
    {
        return (lpos ? 1i8 : -1i8) * this->_data_bigger_than(right.data);
    }
    return lpos ? 1i8 : -1i8;
}

void hr::def::bigint::_trim()
{
    if (data.size() == 0) return;

    ui64 zero_start_index = 0;
    for (i64 i = data.size()-1; i >= 0; i--)
    {
        if (data[i] > 0)
        {
            zero_start_index = i + 1;
            break;
        }
    }
    data.resize(zero_start_index);
    if (data.size() == 0)
        _set_positive(true);
}

void hr::def::bigint::_data_set_ui64(const ui64& val)
{
    data.resize(sizeof(ui64) / sizeof(data_type));
    for (ui8 i = 0; i < sizeof(ui64) / sizeof(data_type); i++)
    {
        data[i] = static_cast<data_type>((val >> (i * data_type_bits)) & data_type_max_mask);
    }
    _trim();
}

void hr::def::bigint::_data_add_from(const data_container_type& other)
{
    data.resize(std::max(data.size(), other.size()) + 1);

    data_type next_adder = 0;

    for (ui64 i = 0; i < data.size(); i++)
    {
        double_sized_data_type res = data[i] + (i < other.size() ? other[i] : 0) + next_adder;
        data[i] = res & data_type_max_mask;
        next_adder = res >> data_type_bits;
    }
    _trim();
}

bool hr::def::bigint::_data_sub_from(const data_container_type& other)
{
    bool pos_sign = true;
    const data_container_type* bigger = &this->data;
    const data_container_type* smaller = &other;
    auto comp_res = this->_data_bigger_than(*smaller);
    if (comp_res < 0)
    {
        std::swap(bigger, smaller);
        pos_sign = false;
    }
    else if (comp_res == 0)
    {
        set_to_zero();
        return true;
    }
    data_container_type res(bigger->size());

    bool lent = false;

    for (ui64 i = 0; i < res.size(); i++)
    {
        double_sized_data_type a = bigger->at(i);
        data_type b = i>=smaller->size() ? 0 : smaller->at(i);
        if (lent)
        {
            if (a == 0)
            {
                a = data_type_max_mask;
                lent = true;
            }
            else
            {
                --a;
                lent = a < b;
            }
        }
        else
        {
            if (a < b)
            {
                lent = true;
                a += 1ui64 << data_type_bits;
            }
            else
            {
                lent = false;
            }
        }
        
        res[i] = a - b;
    }

    data = res;
    _trim();

    return pos_sign;
}

void hr::def::bigint::_set_positive(bool is_positive)
{
    flags[0] = is_positive;
}

void hr::def::bigint::_set_infinity(i8 val)
{
    flags[2] = false;
    if (val == 0) {
        flags[1] = false;
    }
    else
    {
        flags[1] = true;
        flags[0] = val > 0;
    }
}

void hr::def::bigint::_set_nan(bool is_nan)
{

}

hr::def::i8 hr::def::bigint::_data_bigger_than(const data_container_type& other) const
{
    if (data.size() != other.size())
    {
        return data.size() > other.size() ? 1i8 : -1i8;
    }

    if (data.size() == 0) return 0i8;

    for (i64 i = data.size() - 1; i >= 0; i-- )
    {
        if (data[i] != other[i]) return data[i] > other[i] ? 1i8 : -1i8;
    }
    return 0i8;
}
