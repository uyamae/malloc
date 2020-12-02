#include "allocator.h"
#include <cstdint>
#include <cassert>

alignas(32) uint8_t buffer[1024];

int main(int argc, char ** argv) {
    // メモリを確保してすべて解放した時に
    // Allocator のデストラクタでassert に停止しない
    {
        Allocator allocator(buffer, sizeof(buffer));
        void * a = allocator.Allocate(480);
        void * b = allocator.Allocate(224);
        void * c = allocator.Allocate(224);
        allocator.Deallocate(a);
        allocator.Deallocate(b);
        allocator.Deallocate(c);
    }
    {
        Allocator allocator(buffer, sizeof(buffer));
        // 最大サイズを確保して解放する
        void * p = allocator.Allocate(992);
        allocator.Deallocate(p);
        // いくつかのメモリを確保してすべて解放する
        void * a = allocator.Allocate(480);
        void * b = allocator.Allocate(224);
        void * c = allocator.Allocate(224);
        allocator.Deallocate(a);
        allocator.Deallocate(b);
        allocator.Deallocate(c);
        // 再度最大サイズを確保しようとすると失敗する
        void * d = allocator.Allocate(992);
        allocator.Deallocate(d);
    }

    return 0;
}
