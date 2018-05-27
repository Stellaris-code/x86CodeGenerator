/*
bf_peephole.cpp

Copyright (c) 22 Yann BOUCHER (yann)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "bf_peephole.hpp"

#include "bf_generator.hpp"

#include <functional>
#include <iostream>
#include <algorithm>
#include <map>

namespace bfgen
{

int movement_offset(std::string_view s)
{
    return std::count(s.begin(), s.end(), '>') - std::count(s.begin(), s.end(), '<');
}

bool is_cell_op(char c)
{
    return c == '+' || c == '-';
}

int get_cell_op_val(char c)
{
    return c == '+' ? +1 : -1;
}

bool is_ptr_move(char c)
{
    return c == '<' || c == '>';
}

int get_ptr_move_val(char c)
{
    return c == '>' ? +1 : -1;
}

std::map<int, int> get_action_list(std::string_view s)
{
    std::map<int, int> actions; // idx / offset
    int action_index { 0 };
    for (size_t i { 0 }; i < s.size(); ++i)
    {
        if (is_cell_op(s[i]))
        {
            actions[action_index] += get_cell_op_val(s[i]);
        }
        else
        {
            action_index += get_ptr_move_val(s[i]);
        }
    }

    return actions;
}

struct PeepholeFilter
{
    std::function<bool(BrainfuckGenerator&, std::string_view)> check;
    std::function<unsigned(BrainfuckGenerator&, std::string_view)> apply;
};

PeepholeFilter clear_filter =
{
    [](BrainfuckGenerator&, std::string_view s) { return s == "[-]" || s == "[+]"; },
    [](BrainfuckGenerator& gen, std::string_view)
    {
        gen.set(+0, 0);
        return 3;
    }
};

PeepholeFilter scan_filter =
{
    [](BrainfuckGenerator&, std::string_view s)
    {
        return s.size() >= 3 && s.front() == '[' &&
        s.substr(1, s.size()-2).find_first_not_of("><") == std::string::npos
        && s.back() == ']';
    },
    [](BrainfuckGenerator& gen, std::string_view s)
    {
        const int gran = movement_offset(s);

        gen.scan(gran);

        return s.size();
    }
};

PeepholeFilter operation_chain_filter =
{
    [](BrainfuckGenerator& gen, std::string_view s)
    {
        if (s.back() != '[' && s.back() != ']')
        return false;
        if (s.size() < 2)
        return false;
        if (s.substr(0, s.size()-1).find_first_not_of("><+-") != std::string_view::npos)
        return false;

        auto actions = get_action_list(s.substr(0, s.size()-1));
        const int pointer_increment = movement_offset(s.substr(0, s.size()-1));

        // If the offset cannot fit in an int8_t
        if (std::any_of(actions.begin(), actions.end(), [pointer_increment, &gen](const std::pair<int, int>& pair)
        {
            return (pair.first - pointer_increment)*gen.flags.cell_width < gen.min_offset() ||
            (pair.first - pointer_increment)*gen.flags.cell_width > gen.max_offset();
        }))
        return false;

        return true;
    },
    [](BrainfuckGenerator& gen, std::string_view s)
    {
        auto actions = get_action_list(s);

        // Move the pointer first in order to avoid cmp calls
        const int pointer_increment = movement_offset(s.substr(0, s.size()-1));

        if      (pointer_increment > 0) gen.move_forwards(pointer_increment);
        else if (pointer_increment < 0) gen.move_backwards(-pointer_increment);

        for (auto pair : actions)
        {
            if (pair.second)
            {
                gen.add(pair.first - pointer_increment, pair.second % (gen.max_value()+1));
            }
        }

        return s.size() - 1; // minus the end character
    }
};

PeepholeFilter copy_loop_filter =
{
    [](BrainfuckGenerator& gen, std::string_view s)
    {
        if (s.size() <= 3 || s.front() != '[' || s.back() != ']')
        return false;
        if (s.substr(1, s.size()-2).find_first_not_of("><+-") != std::string_view::npos)
        return false;

        auto actions = get_action_list(s.substr(1, s.size()-2));
        if (!actions.count(0) || actions.at(0) != -1)
        return false;

        // If the offset cannot fit in an int8_t
        // TODO : function fits_in<T>()
        if (std::any_of(actions.begin(), actions.end(), [&gen](const std::pair<int, int>& pair)
        {
            return pair.first*gen.flags.cell_width <= gen.min_offset() ||
            pair.first*gen.flags.cell_width >= gen.max_offset() ||
            (pair.second <= INT8_MIN || pair.second >= INT8_MAX);
        }))
        return false;

        if (movement_offset(s.substr(1, s.size()-2)) != 0)
        return false;

        return true;
    },
    [](BrainfuckGenerator& gen, std::string_view s)
    {
        auto actions = get_action_list(s.substr(1, s.size()-2));

        for (auto pair : actions)
        {
            if (pair.first != 0)
            {
                if (pair.second >= 0) gen.add_copy(pair.first, pair.second);
                else                  gen.sub_copy(pair.first, -pair.second);

            }
        }

        assert(actions.at(0) == -1);

        gen.set(0, 0);

        return s.size();
    }
};


int apply_peephole_optimization(BrainfuckGenerator &gen, std::string_view atom)
{
    PeepholeFilter filters[] = { clear_filter, scan_filter, copy_loop_filter, operation_chain_filter };

    for (const auto& filter : filters)
    {
        if (filter.check(gen, atom))
        {
            return filter.apply(gen, atom);
        }
    }

    return -1;
}

}
