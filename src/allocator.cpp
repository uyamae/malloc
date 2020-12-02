/**
 * @file
 * @brief メモリアロケータ
 * @author ozaki-takayuki
 */
#include "allocator.h"
#include <cstdint>
#include <cassert>

/**
 * @brief Memory Control Block 構造体定義
 */
struct Allocator::MCB {
    MCB * prev;     ///< 前のメモリブロック
    MCB * next;     ///< 次のメモリブロック
    size_t size;    ///< 管理しているメモリサイズ(MCB サイズを含まない)
    uint8_t reserved[8];    ///< 予約領域
};
// コンストラクタ
Allocator::Allocator(void * addr, size_t size)
    : addr{ addr }
    , size{ size }
    // 管理メモリ全体をひとつの空きメモリブロックにする
    , freeList{ reinterpret_cast<MCB *>(addr) }
    // 使用リストは空にする
    , usedList{ nullptr }
{
    // 空きメモリブロックの初期化
    freeList->prev = nullptr;
    freeList->next = nullptr;
    // 使用できるサイズは管理サイズ全体からMCB サイズを引いたもの
    freeList->size = size - sizeof(MCB);
}
// デストラクタ
Allocator::~Allocator()
{
    // 使用中メモリがないかチェック
    assert(usedList == nullptr);
}
// メモリを確保する
void * Allocator::Allocate(size_t size)
{
    static constexpr size_t size_offset = sizeof(MCB) - 1;
    static constexpr size_t size_mask = ~size_offset;
    // 実際に確保するサイズ
    // MCB サイズの倍数になるようにする
    size_t alloc_size = (size + size_offset) & size_mask;

    // 空きメモリブロックリストから使用可能なものを探す
    auto current = freeList;
    while (current) {
        // 確保サイズに足りなければ次へ
        if (current->size < alloc_size) {
            current = current->next;
            continue;
        }
        // メモリが大きい場合分割する
        if (current->size >= (alloc_size + sizeof(MCB) * 2)) {
            // 分割したあとのMCB の先頭アドレス
            auto next = reinterpret_cast<uintptr_t>(current + 1) + alloc_size;
            // next が管理領域内であれば分割
            if (next < (reinterpret_cast<uintptr_t>(addr) + this->size)) {
                // アドレスをMCB のポインタにキャスト
                auto newMcb = reinterpret_cast<MCB *>(next);
                // 現在のMCB の後ろに連結する
                newMcb->next = current->next;
                newMcb->prev = current;
                current->next = newMcb;
                // サイズを更新
                // 新しいMCB の管理サイズは現在のMCB の管理領域から
                // 確保サイズ分と新しいMCB のサイズを引いたもの
                newMcb->size = current->size - alloc_size - sizeof(MCB);
                // 現在のMCB の管理サイズは確保サイズに合わせる
                current->size = alloc_size;
            }
        }
        // 現在のMCB を空きメモリブロックリストから切り離す
        if (current->prev) {
            current->prev->next = current->next;
        }
        if (current->next) {
            current->next->prev = current->prev;
        }
        // 現在のMCB が空きメモリブロックリストの先頭なら、先頭を交代する
        if (current == freeList) {
            freeList = current->next;
        }
        // 使用中リストに連結する
        if (usedList) {
            usedList->prev = current;
        }
        current->next = usedList;
        usedList = current;
        // 現在のMCB の管理領域のアドレスを返す
        return static_cast<void *>(current + 1);
    }
    // 確保できるMCB が見つからなければ失敗でnullptr を返す
    return nullptr;
}
// メモリを解放する
void Allocator::Deallocate(void * ptr)
{
    // nullptr の場合何もしない
    if (!ptr) {
        return;
    }
    // MCB は解放するアドレスの直前に配置されている
    auto mcb = static_cast<MCB *>(ptr) - 1;
    // MCB を使用中リストから切り離す
    if (mcb->next) {
        mcb->next->prev = mcb->prev;
    }
    if (mcb->prev) {
        mcb->prev->next = mcb->next;
    }
    if (mcb == usedList) {
        usedList = mcb->next;
    }
    // MCB を空きメモリブロックリストに連結する
    if (freeList) {
        freeList->prev = mcb;
    }
    mcb->next = freeList;
    freeList = mcb;
}
