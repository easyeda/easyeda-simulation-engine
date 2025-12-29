/***************************************************************************
 *   Modified (C) 2025 by EasyEDA & JLC Technology Group                      *
 *   chensiyu@sz-jlc.com                                                   *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 *   GNU Affero General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Affero General Public      *
 *   License along with this program. If not, see                          *
 *   <http://www.gnu.org/licenses/>.                                       *
 ***************************************************************************/

#pragma once

#include "pch.h"

template <typename T>
class  CircularBuffer
{
private:
    std::vector<T> buffer;
    size_t capacity;
    size_t head = 0;
    size_t tail = 0;
    bool is_full = false;
    mutable std::mutex mtx;

public:
    explicit CircularBuffer(size_t size)
        : capacity(size), head(0), tail(0), is_full(false) // 显式初始化所有成员
    {
        if(size == 0){
            throw std::invalid_argument("data capacity must be > 0");
        }
        buffer.resize(capacity);
    }


    void push(const T& value ){
        std::lock_guard<std::mutex> lock(mtx);

        buffer[head] = value;
        head = (head + 1) % capacity;
        if(is_full){
            tail = (tail + 1) % capacity;
        }
        is_full = (head == tail);
    }

    T pop(){
        std::lock_guard<std::mutex> lock(mtx);

        if (isEmpty()) {
            throw std::runtime_error("Buffer is empty!");
        }
        
        T value = buffer[tail];
        tail = (tail + 1) %capacity;
        is_full = false;
        return value;

    }

    bool isEmpty() const {
        return !is_full && (head == tail);
    }

    bool isFull() const{
        return is_full;
    }

    size_t size() const{
        if(is_full) return capacity;
        if(head >= tail) return head - tail;
        return capacity -tail + head;    
    }

    size_t maxSize() const{
        return capacity;
    }

    std::vector<T> getAll() const{
        std::lock_guard<std::mutex> lock(mtx);
        std::vector<T> result;

        if (isEmpty()) return result;

        if(head > tail){
            result.insert(result.end(),buffer.begin() + tail ,buffer.begin() + head);
        } else{
            result.insert(result.end(),buffer.begin() + tail,buffer.end() );
            result.insert(result.end(),buffer.begin(),buffer.begin() + head);
        }

        return result;
    }


    std::vector<T> getDelta(size_t& lastPos) const {
        std::lock_guard<std::mutex> lock(mtx);

        std::vector<T> result;
        size_t currentHead = head;

        if (lastPos == currentHead || isEmpty()) {
            return result; // 没有新数据
        }

        if (lastPos < currentHead) {
            result.insert(result.end(), buffer.begin() + lastPos, buffer.begin() + currentHead);
        } else {
            result.insert(result.end(), buffer.begin() + lastPos, buffer.end());
            result.insert(result.end(), buffer.begin(), buffer.begin() + currentHead);
        }

        lastPos = currentHead;
        return result;
    }
    void clear(){
        std::lock_guard<std::mutex> lock(mtx);

        head = tail = 0;
        is_full = 0;
    }

    void back() const {
        std::lock_guard<std::mutex> lock(mtx); // 线程安全
        if (isEmpty()) {
            throw std::runtime_error("Buffer is empty!");
        }

        return buffer [(head == 0 ? capacity : head) - 1];
    }

    size_t getCurrentHead(){
        return head;
    }
};

