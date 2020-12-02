/**
 * @file
 * @brief メモリアロケータ
 * @author ozaki-takayuki
 */
#pragma once

/**
 * @brief メモリアロケータ
 */
class Allocator {
public:
    /**
     * @brief コンストラクタ
     * @param[in] addr 管理するメモリの先頭アドレス
     * @param[in] size 管理するメモリのサイズ
     */
    Allocator(void * addr, size_t size);
    /// デストラクタ
    ~Allocator();
    /**
     * @brief メモリを確保する
     * @param[in] size 確保するサイズ
     * @return 確保したメモリアドレス。失敗したらnullptr
     */
    void * Allocate(size_t size);
    /**
     * @brief メモリを解放する
     * @param[in] ptr 解放するメモリアドレス
     */
    void Deallocate(void * ptr);
private:
    /// Memocy Control Block 構造体宣言
    struct MCB;
    
    void * addr;    ///< 管理しているメモリの先頭アドレス
    size_t size;    ///< 管理しているメモリのサイズ
    MCB * freeList; ///< 空きメモリブロックリストの先頭
    MCB * usedList; ///< 使用中メモリブロックリストの先頭
};
