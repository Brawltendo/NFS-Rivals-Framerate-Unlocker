// DO NOT EDIT. This file is auto-generated by `amalgamate.py`.

#define NOMINMAX

#include <safetyhook.hpp>


//
// Source file: allocator.cpp
//

#include <algorithm>
#include <functional>
#include <limits>

#define NOMINMAX
#include <Windows.h>


namespace safetyhook {
template <typename T> constexpr T align_up(T address, size_t align) {
    const auto unaligned_address = (uintptr_t)address;
    const auto aligned_address = (unaligned_address + align - 1) & ~(align - 1);
    return (T)aligned_address;
}

template <typename T> constexpr T align_down(T address, size_t align) {
    const auto unaligned_address = (uintptr_t)address;
    const auto aligned_address = unaligned_address & ~(align - 1);
    return (T)aligned_address;
}

Allocation::Allocation(Allocation&& other) noexcept {
    *this = std::move(other);
}

Allocation& Allocation::operator=(Allocation&& other) noexcept {
    if (this != &other) {
        free();

        m_allocator = std::move(other.m_allocator);
        m_address = other.m_address;
        m_size = other.m_size;

        other.m_address = nullptr;
        other.m_size = 0;
    }

    return *this;
}

Allocation::~Allocation() {
    free();
}

void Allocation::free() {
    if (m_allocator && m_address != nullptr && m_size != 0) {
        m_allocator->free(m_address, m_size);
        m_address = nullptr;
        m_size = 0;
        m_allocator.reset();
    }
}

Allocation::Allocation(std::shared_ptr<Allocator> allocator, uint8_t* address, size_t size) noexcept
    : m_allocator{std::move(allocator)}, m_address{address}, m_size{size} {
}

std::shared_ptr<Allocator> Allocator::global() {
    static std::weak_ptr<Allocator> global_allocator{};
    static std::mutex global_allocator_mutex{};

    std::scoped_lock lock{global_allocator_mutex};

    if (auto allocator = global_allocator.lock()) {
        return allocator;
    }

    auto allocator = Allocator::create();

    global_allocator = allocator;

    return allocator;
}

std::shared_ptr<Allocator> Allocator::create() {
    return std::shared_ptr<Allocator>{new Allocator{}};
}

std::expected<Allocation, Allocator::Error> Allocator::allocate(size_t size) {
    return allocate_near({}, size, std::numeric_limits<size_t>::max());
}

std::expected<Allocation, Allocator::Error> Allocator::allocate_near(
    const std::vector<uint8_t*>& desired_addresses, size_t size, size_t max_distance) {
    std::scoped_lock lock{m_mutex};
    return internal_allocate_near(desired_addresses, size, max_distance);
}

void Allocator::free(uint8_t* address, size_t size) {
    std::scoped_lock lock{m_mutex};
    return internal_free(address, size);
}

std::expected<Allocation, Allocator::Error> Allocator::internal_allocate_near(
    const std::vector<uint8_t*>& desired_addresses, size_t size, size_t max_distance) {
    // First search through our list of allocations for a free block that is large
    // enough.
    for (const auto& allocation : m_memory) {
        if (allocation->size < size) {
            continue;
        }

        for (auto node = allocation->freelist.get(); node != nullptr; node = node->next.get()) {
            // Enough room?
            if (static_cast<size_t>(node->end - node->start) < size) {
                continue;
            }

            const auto address = node->start;

            // Close enough?
            if (!in_range(address, desired_addresses, max_distance)) {
                continue;
            }

            node->start += size;

            return Allocation{shared_from_this(), address, size};
        }
    }

    // If we didn't find a free block, we need to allocate a new one.
    SYSTEM_INFO si{};

    GetSystemInfo(&si);

    const auto allocation_size = align_up(size, si.dwAllocationGranularity);
    const auto allocation_address = allocate_nearby_memory(desired_addresses, allocation_size, max_distance);

    if (!allocation_address) {
        return std::unexpected{allocation_address.error()};
    }

    const auto& allocation = m_memory.emplace_back(new Memory);

    allocation->address = *allocation_address;
    allocation->size = allocation_size;
    allocation->freelist = std::make_unique<FreeNode>();
    allocation->freelist->start = *allocation_address + size;
    allocation->freelist->end = *allocation_address + allocation_size;

    return Allocation{shared_from_this(), *allocation_address, size};
}

void Allocator::internal_free(uint8_t* address, size_t size) {
    for (const auto& allocation : m_memory) {
        if (allocation->address > address || allocation->address + allocation->size < address) {
            continue;
        }

        // Find the right place for our new freenode.
        FreeNode* prev{};

        for (auto node = allocation->freelist.get(); node != nullptr; prev = node, node = node->next.get()) {
            if (node->start > address) {
                break;
            }
        }

        // Add new freenode.
        auto free_node = std::make_unique<FreeNode>();

        free_node->start = address;
        free_node->end = address + size;

        if (prev == nullptr) {
            free_node->next.swap(allocation->freelist);
            allocation->freelist.swap(free_node);
        } else {
            free_node->next.swap(prev->next);
            prev->next.swap(free_node);
        }

        combine_adjacent_freenodes(*allocation);
        break;
    }
}

void Allocator::combine_adjacent_freenodes(Memory& memory) {
    for (auto prev = memory.freelist.get(), node = prev; node != nullptr; node = node->next.get()) {
        if (prev->end == node->start) {
            prev->end = node->end;
            prev->next.swap(node->next);
            node->next.reset();
            node = prev;
        } else {
            prev = node;
        }
    }
}

std::expected<uint8_t*, Allocator::Error> Allocator::allocate_nearby_memory(
    const std::vector<uint8_t*>& desired_addresses, size_t size, size_t max_distance) {
    if (desired_addresses.empty()) {
        if (const auto result = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
            result != nullptr) {
            return static_cast<uint8_t*>(result);
        }

        return std::unexpected{Error::BAD_VIRTUAL_ALLOC};
    }

    auto attempt_allocation = [&](uint8_t* p) -> uint8_t* {
        if (!in_range(p, desired_addresses, max_distance)) {
            return nullptr;
        }

        return static_cast<uint8_t*>(VirtualAlloc(p, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    };

    SYSTEM_INFO si{};

    GetSystemInfo(&si);

    auto desired_address = desired_addresses[0];
    auto search_start = reinterpret_cast<uint8_t*>(std::numeric_limits<uintptr_t>::min());
    auto search_end = reinterpret_cast<uint8_t*>(std::numeric_limits<uintptr_t>::max());

    if (static_cast<size_t>(desired_address - search_start) > max_distance) {
        search_start = desired_address - max_distance;
    }

    if (static_cast<size_t>(search_end - desired_address) > max_distance) {
        search_end = desired_address + max_distance;
    }

    search_start = std::max(search_start, static_cast<uint8_t*>(si.lpMinimumApplicationAddress));
    search_end = std::min(search_end, static_cast<uint8_t*>(si.lpMaximumApplicationAddress));
    desired_address = align_up(desired_address, si.dwAllocationGranularity);
    MEMORY_BASIC_INFORMATION mbi{};

    // Search backwards from the desired_address.
    for (auto p = desired_address; p > search_start && in_range(p, desired_addresses, max_distance);
         p = align_down(static_cast<uint8_t*>(mbi.AllocationBase) - 1, si.dwAllocationGranularity)) {
        if (VirtualQuery(p, &mbi, sizeof(mbi)) == 0) {
            break;
        }

        if (mbi.State != MEM_FREE) {
            continue;
        }

        if (auto allocation_address = attempt_allocation(p); allocation_address != 0) {
            return allocation_address;
        }
    }

    // Search forwards from the desired_address.
    for (auto p = desired_address; p < search_end && in_range(p, desired_addresses, max_distance);
         p += mbi.RegionSize) {
        if (VirtualQuery(p, &mbi, sizeof(mbi)) == 0) {
            break;
        }

        if (mbi.State != MEM_FREE) {
            continue;
        }

        if (auto allocation_address = attempt_allocation(p); allocation_address != nullptr) {
            return allocation_address;
        }
    }

    return std::unexpected{Error::NO_MEMORY_IN_RANGE};
}

bool Allocator::in_range(uint8_t* address, const std::vector<uint8_t*>& desired_addresses, size_t max_distance) {
    return std::ranges::all_of(desired_addresses, [&](const auto& desired_address) {
        const size_t delta = (address > desired_address) ? address - desired_address : desired_address - address;
        return delta <= max_distance;
    });
}

Allocator::Memory::~Memory() {
    VirtualFree(address, 0, MEM_RELEASE);
}
} // namespace safetyhook

//
// Source file: easy.cpp
//


namespace safetyhook {
InlineHook create_inline(void* target, void* destination) {
    if (auto hook = InlineHook::create(target, destination)) {
        return std::move(*hook);
    } else {
        return {};
    }
}

MidHook create_mid(void* target, MidHookFn destination) {
    if (auto hook = MidHook::create(target, destination)) {
        return std::move(*hook);
    } else {
        return {};
    }
}

VmtHook create_vmt(void* object) {
    if (auto hook = VmtHook::create(object)) {
        return std::move(*hook);
    } else {
        return {};
    }
}
} // namespace safetyhook

//
// Source file: inline_hook.cpp
//

#include <iterator>

#include <Windows.h>

#if __has_include(<Zydis/Zydis.h>)
#include <Zydis/Zydis.h>
#elif __has_include(<Zydis.h>)
#include <Zydis.h>
#else
#error "Zydis not found"
#endif



namespace safetyhook {
class UnprotectMemory {
public:
    UnprotectMemory(uint8_t* address, size_t size) : m_address{address}, m_size{size} {
        VirtualProtect(m_address, m_size, PAGE_EXECUTE_READWRITE, &m_protect);
    }

    ~UnprotectMemory() { VirtualProtect(m_address, m_size, m_protect, &m_protect); }

private:
    uint8_t* m_address{};
    size_t m_size{};
    DWORD m_protect{};
};

#pragma pack(push, 1)
struct JmpE9 {
    uint8_t opcode{0xE9};
    uint32_t offset{0};
};

#if defined(_M_X64)
struct JmpFF {
    uint8_t opcode0{0xFF};
    uint8_t opcode1{0x25};
    uint32_t offset{0};
};

struct TrampolineEpilogueE9 {
    JmpE9 jmp_to_original{};
    JmpFF jmp_to_destination{};
    uint64_t destination_address{};
};

struct TrampolineEpilogueFF {
    JmpFF jmp_to_original{};
    uint64_t original_address{};
};
#elif defined(_M_IX86)
struct TrampolineEpilogueE9 {
    JmpE9 jmp_to_original{};
    JmpE9 jmp_to_destination{};
};
#endif
#pragma pack(pop)

#ifdef _M_X64
static auto make_jmp_ff(uint8_t* src, uint8_t* dst, uint8_t* data) {
    JmpFF jmp{};

    jmp.offset = static_cast<uint32_t>(data - src - sizeof(jmp));
    store(data, dst);

    return jmp;
}

static void emit_jmp_ff(uint8_t* src, uint8_t* dst, uint8_t* data, size_t size = sizeof(JmpFF)) {
    if (size < sizeof(JmpFF)) {
        return;
    }

    UnprotectMemory unprotect{src, size};

    if (size > sizeof(JmpFF)) {
        std::fill_n(src, size, static_cast<uint8_t>(0x90));
    }

    store(src, make_jmp_ff(src, dst, data));
}
#endif

constexpr auto make_jmp_e9(uint8_t* src, uint8_t* dst) {
    JmpE9 jmp{};

    jmp.offset = static_cast<uint32_t>(dst - src - sizeof(jmp));

    return jmp;
}

static void emit_jmp_e9(uint8_t* src, uint8_t* dst, size_t size = sizeof(JmpE9)) {
    if (size < sizeof(JmpE9)) {
        return;
    }

    UnprotectMemory unprotect{src, size};

    if (size > sizeof(JmpE9)) {
        std::fill_n(src, size, static_cast<uint8_t>(0x90));
    }

    store(src, make_jmp_e9(src, dst));
}

static bool decode(ZydisDecodedInstruction* ix, uint8_t* ip) {
    ZydisDecoder decoder{};
    ZyanStatus status;

#if defined(_M_X64)
    status = ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
#elif defined(_M_IX86)
    status = ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LEGACY_32, ZYDIS_STACK_WIDTH_32);
#else
#error "Unsupported architecture"
#endif

    if (!ZYAN_SUCCESS(status)) {
        return false;
    }

    return ZYAN_SUCCESS(ZydisDecoderDecodeInstruction(&decoder, nullptr, ip, 15, ix));
}

std::expected<InlineHook, InlineHook::Error> InlineHook::create(void* target, void* destination) {
    return create(Allocator::global(), target, destination);
}

std::expected<InlineHook, InlineHook::Error> InlineHook::create(
    const std::shared_ptr<Allocator>& allocator, void* target, void* destination) {
    InlineHook hook{};

    if (const auto setup_result =
            hook.setup(allocator, reinterpret_cast<uint8_t*>(target), reinterpret_cast<uint8_t*>(destination));
        !setup_result) {
        return std::unexpected{setup_result.error()};
    }

    return hook;
}

InlineHook::InlineHook(InlineHook&& other) noexcept {
    *this = std::move(other);
}

InlineHook& InlineHook::operator=(InlineHook&& other) noexcept {
    if (this != &other) {
        destroy();

        std::scoped_lock lock{m_mutex, other.m_mutex};

        m_target = other.m_target;
        m_destination = other.m_destination;
        m_trampoline = std::move(other.m_trampoline);
        m_trampoline_size = other.m_trampoline_size;
        m_original_bytes = std::move(other.m_original_bytes);

        other.m_target = 0;
        other.m_destination = 0;
        other.m_trampoline_size = 0;
    }

    return *this;
}

InlineHook::~InlineHook() {
    destroy();
}

void InlineHook::reset() {
    *this = {};
}

std::expected<void, InlineHook::Error> InlineHook::setup(
    const std::shared_ptr<Allocator>& allocator, uint8_t* target, uint8_t* destination) {
    m_target = target;
    m_destination = destination;

    if (auto e9_result = e9_hook(allocator); !e9_result) {
#ifdef _M_X64
        if (auto ff_result = ff_hook(allocator); !ff_result) {
            return ff_result;
        }
#else
        return e9_result;
#endif
    }

    return {};
}

std::expected<void, InlineHook::Error> InlineHook::e9_hook(const std::shared_ptr<Allocator>& allocator) {
    m_original_bytes.clear();
    m_trampoline_size = sizeof(TrampolineEpilogueE9);

    std::vector<uint8_t*> desired_addresses{m_target};
    ZydisDecodedInstruction ix{};

    for (auto ip = m_target; ip < m_target + sizeof(JmpE9); ip += ix.length) {
        if (!decode(&ix, ip)) {
            return std::unexpected{Error::failed_to_decode_instruction(ip)};
        }

        m_trampoline_size += ix.length;
        m_original_bytes.insert(m_original_bytes.end(), ip, ip + ix.length);

        const auto is_relative = (ix.attributes & ZYDIS_ATTRIB_IS_RELATIVE) != 0;

        if (is_relative) {
            if (ix.raw.disp.size == 32) {
                const auto target_address = ip + ix.length + static_cast<int32_t>(ix.raw.disp.value);
                desired_addresses.emplace_back(target_address);
            } else if (ix.raw.imm[0].size == 32) {
                const auto target_address = ip + ix.length + static_cast<int32_t>(ix.raw.imm[0].value.s);
                desired_addresses.emplace_back(target_address);
            } else if (ix.meta.category == ZYDIS_CATEGORY_COND_BR && ix.meta.branch_type == ZYDIS_BRANCH_TYPE_SHORT) {
                const auto target_address = ip + ix.length + static_cast<int32_t>(ix.raw.imm[0].value.s);
                desired_addresses.emplace_back(target_address);
                m_trampoline_size += 4; // near conditional branches are 4 bytes larger.
            } else if (ix.meta.category == ZYDIS_CATEGORY_UNCOND_BR && ix.meta.branch_type == ZYDIS_BRANCH_TYPE_SHORT) {
                const auto target_address = ip + ix.length + static_cast<int32_t>(ix.raw.imm[0].value.s);
                desired_addresses.emplace_back(target_address);
                m_trampoline_size += 3; // near unconditional branches are 3 bytes larger.
            } else {
                return std::unexpected{Error::unsupported_instruction_in_trampoline(ip)};
            }
        }
    }

    auto trampoline_allocation = allocator->allocate_near(desired_addresses, m_trampoline_size);

    if (!trampoline_allocation) {
        return std::unexpected{Error::bad_allocation(trampoline_allocation.error())};
    }

    m_trampoline = std::move(*trampoline_allocation);

    for (auto ip = m_target, tramp_ip = m_trampoline.data(); ip < m_target + m_original_bytes.size(); ip += ix.length) {
        if (!decode(&ix, ip)) {
            m_trampoline.free();
            return std::unexpected{Error::failed_to_decode_instruction(ip)};
        }

        const auto is_relative = (ix.attributes & ZYDIS_ATTRIB_IS_RELATIVE) != 0;

        if (is_relative && ix.raw.disp.size == 32) {
            std::copy_n(ip, ix.length, tramp_ip);
            const auto target_address = ip + ix.length + ix.raw.disp.value;
            const auto new_disp = target_address - (tramp_ip + ix.length);
            store(tramp_ip + ix.raw.disp.offset, static_cast<int32_t>(new_disp));
            tramp_ip += ix.length;
        } else if (is_relative && ix.raw.imm[0].size == 32) {
            std::copy_n(ip, ix.length, tramp_ip);
            const auto target_address = ip + ix.length + ix.raw.imm[0].value.s;
            const auto new_disp = target_address - (tramp_ip + ix.length);
            store(tramp_ip + ix.raw.imm[0].offset, static_cast<int32_t>(new_disp));
            tramp_ip += ix.length;
        } else if (ix.meta.category == ZYDIS_CATEGORY_COND_BR && ix.meta.branch_type == ZYDIS_BRANCH_TYPE_SHORT) {
            const auto target_address = ip + ix.length + ix.raw.imm[0].value.s;
            auto new_disp = target_address - (tramp_ip + 6);

            // Handle the case where the target is now in the trampoline.
            if (target_address < m_target + m_original_bytes.size()) {
                new_disp = static_cast<ptrdiff_t>(ix.raw.imm[0].value.s);
            }

            *tramp_ip = 0x0F;
            *(tramp_ip + 1) = 0x10 + ix.opcode;
            store(tramp_ip + 2, static_cast<int32_t>(new_disp));
            tramp_ip += 6;
        } else if (ix.meta.category == ZYDIS_CATEGORY_UNCOND_BR && ix.meta.branch_type == ZYDIS_BRANCH_TYPE_SHORT) {
            const auto target_address = ip + ix.length + ix.raw.imm[0].value.s;
            auto new_disp = target_address - (tramp_ip + 5);

            // Handle the case where the target is now in the trampoline.
            if (target_address < m_target + m_original_bytes.size()) {
                new_disp = static_cast<ptrdiff_t>(ix.raw.imm[0].value.s);
            }

            *tramp_ip = 0xE9;
            store(tramp_ip + 1, static_cast<int32_t>(new_disp));
            tramp_ip += 5;
        } else {
            std::copy_n(ip, ix.length, tramp_ip);
            tramp_ip += ix.length;
        }
    }

    auto trampoline_epilogue = reinterpret_cast<TrampolineEpilogueE9*>(
        m_trampoline.address() + m_trampoline_size - sizeof(TrampolineEpilogueE9));

    // jmp from trampoline to original.
    auto src = reinterpret_cast<uint8_t*>(&trampoline_epilogue->jmp_to_original);
    auto dst = m_target + m_original_bytes.size();
    emit_jmp_e9(src, dst);

    // jmp from trampoline to destination.
    src = reinterpret_cast<uint8_t*>(&trampoline_epilogue->jmp_to_destination);
    dst = m_destination;

#ifdef _M_X64
    auto data = reinterpret_cast<uint8_t*>(&trampoline_epilogue->destination_address);
    emit_jmp_ff(src, dst, data);
#else
    emit_jmp_e9(src, dst);
#endif

    // jmp from original to trampoline.
    execute_while_frozen(
        [this, &trampoline_epilogue] {
            const auto src = m_target;
            const auto dst = reinterpret_cast<uint8_t*>(&trampoline_epilogue->jmp_to_destination);
            emit_jmp_e9(src, dst, m_original_bytes.size());
        },
        [this](uint32_t, HANDLE, CONTEXT& ctx) {
            for (size_t i = 0; i < m_original_bytes.size(); ++i) {
                fix_ip(ctx, m_target + i, m_trampoline.data() + i);
            }
        });

    return {};
}

#ifdef _M_X64
std::expected<void, InlineHook::Error> InlineHook::ff_hook(const std::shared_ptr<Allocator>& allocator) {
    m_original_bytes.clear();
    m_trampoline_size = sizeof(TrampolineEpilogueFF);
    ZydisDecodedInstruction ix{};

    for (auto ip = m_target; ip < m_target + sizeof(JmpFF) + sizeof(uintptr_t); ip += ix.length) {
        if (!decode(&ix, ip)) {
            return std::unexpected{Error::failed_to_decode_instruction(ip)};
        }

        // We can't support any instruction that is IP relative here because
        // ff_hook should only be called if e9_hook failed indicating that
        // we're likely outside the +- 2GB range.
        if (ix.attributes & ZYDIS_ATTRIB_IS_RELATIVE) {
            return std::unexpected{Error::ip_relative_instruction_out_of_range(ip)};
        }

        m_original_bytes.insert(m_original_bytes.end(), ip, ip + ix.length);
        m_trampoline_size += ix.length;
    }

    auto trampoline_allocation = allocator->allocate(m_trampoline_size);

    if (!trampoline_allocation) {
        return std::unexpected{Error::bad_allocation(trampoline_allocation.error())};
    }

    m_trampoline = std::move(*trampoline_allocation);

    std::copy(m_original_bytes.begin(), m_original_bytes.end(), m_trampoline.data());

    const auto trampoline_epilogue =
        reinterpret_cast<TrampolineEpilogueFF*>(m_trampoline.data() + m_trampoline_size - sizeof(TrampolineEpilogueFF));

    // jmp from trampoline to original.
    auto src = reinterpret_cast<uint8_t*>(&trampoline_epilogue->jmp_to_original);
    auto dst = m_target + m_original_bytes.size();
    auto data = reinterpret_cast<uint8_t*>(&trampoline_epilogue->original_address);
    emit_jmp_ff(src, dst, data);

    // jmp from original to trampoline.
    execute_while_frozen(
        [this] {
            const auto src = m_target;
            const auto dst = m_destination;
            const auto data = src + sizeof(JmpFF);
            emit_jmp_ff(src, dst, data, m_original_bytes.size());
        },
        [this](uint32_t, HANDLE, CONTEXT& ctx) {
            for (size_t i = 0; i < m_original_bytes.size(); ++i) {
                fix_ip(ctx, m_target + i, m_trampoline.data() + i);
            }
        });

    return {};
}
#endif

void InlineHook::destroy() {
    std::scoped_lock lock{m_mutex};

    if (!m_trampoline) {
        return;
    }

    execute_while_frozen(
        [this] {
            UnprotectMemory unprotect{m_target, m_original_bytes.size()};
            std::copy(m_original_bytes.begin(), m_original_bytes.end(), m_target);
        },
        [this](uint32_t, HANDLE, CONTEXT& ctx) {
            for (size_t i = 0; i < m_original_bytes.size(); ++i) {
                fix_ip(ctx, m_trampoline.data() + i, m_target + i);
            }
        });

    m_trampoline.free();
}
} // namespace safetyhook

//
// Source file: mid_hook.cpp
//

#include <algorithm>
#include <array>



namespace safetyhook {

#ifdef _M_X64
constexpr auto asm_data = std::to_array<uint8_t>(
    {
        0xff, 0x35, 0x5f, 0x01, 0x00, 0x00, 
        0x54, 
        0x55,
        0x50, 
        0x53, 
        0x51,
        0x52, 
        0x56, 
        0x57, 
        0x41, 0x50, 
        0x41, 0x51, 
        0x41, 0x52, 
        0x41, 0x53, 
        0x41, 0x54, 
        0x41, 0x55, 
        0x41, 0x56, 
        0x41, 0x57,
        0x9c, 
        0x48, 0x81, 0xec, 0x00, 0x01, 0x00, 0x00, 

        /*0xf3, 0x44, 0x0f, 0x7f, 0xbc, 0x24, 0x10, 0xff, 0xff, 0xff,
        0xf3, 0x44, 0x0f, 0x7f, 0xb4, 0x24, 0x20, 0xff, 0xff, 0xff,
        0xf3, 0x44, 0x0f, 0x7f, 0xac, 0x24, 0x30, 0xff, 0xff, 0xff,
        0xf3, 0x44, 0x0f, 0x7f, 0xa4, 0x24, 0x40, 0xff, 0xff, 0xff,
        0xf3, 0x44, 0x0f, 0x7f, 0x9c, 0x24, 0x50, 0xff, 0xff, 0xff,
        0xf3, 0x44, 0x0f, 0x7f, 0x94, 0x24, 0x60, 0xff, 0xff, 0xff,
        0xf3, 0x44, 0x0f, 0x7f, 0x8c, 0x24, 0x70, 0xff, 0xff, 0xff,
        0xf3, 0x44, 0x0f, 0x7f, 0x44, 0x24, 0x80,
        0xf3, 0x0f, 0x7f, 0x7c, 0x24, 0x90,
        0xf3, 0x0f, 0x7f, 0x74, 0x24, 0xa0,
        0xf3, 0x0f, 0x7f, 0x6c, 0x24, 0xb0,
        0xf3, 0x0f, 0x7f, 0x64, 0x24, 0xc0,
        0xf3, 0x0f, 0x7f, 0x5c, 0x24, 0xd0,
        0xf3, 0x0f, 0x7f, 0x54, 0x24, 0xe0,
        0xf3, 0x0f, 0x7f, 0x4c, 0x24, 0xf0,
        0xf3, 0x0f, 0x7f, 0x04, 0x24,*/

        0xf3, 0x44, 0x0f, 0x7f, 0xbc, 0x24, 0xf0, 0x00, 0x00, 0x00,
        0xf3, 0x44, 0x0f, 0x7f, 0xb4, 0x24, 0xe0, 0x00, 0x00, 0x00,
        0xf3, 0x44, 0x0f, 0x7f, 0xac, 0x24, 0xd0, 0x00, 0x00, 0x00,
        0xf3, 0x44, 0x0f, 0x7f, 0xa4, 0x24, 0xc0, 0x00, 0x00, 0x00,
        0xf3, 0x44, 0x0f, 0x7f, 0x9c, 0x24, 0xb0, 0x00, 0x00, 0x00,
        0xf3, 0x44, 0x0f, 0x7f, 0x94, 0x24, 0xa0, 0x00, 0x00, 0x00,
        0xf3, 0x44, 0x0f, 0x7f, 0x8c, 0x24, 0x90, 0x00, 0x00, 0x00,
        0xf3, 0x44, 0x0f, 0x7f, 0x84, 0x24, 0x80, 0x00, 0x00, 0x00,
        0xf3, 0x0f, 0x7f, 0x7c, 0x24, 0x70, 
        0xf3, 0x0f, 0x7f, 0x74, 0x24, 0x60, 
        0xf3, 0x0f, 0x7f, 0x6c, 0x24, 0x50, 
        0xf3, 0x0f, 0x7f, 0x64, 0x24, 0x40, 
        0xf3, 0x0f, 0x7f, 0x5c, 0x24, 0x30, 
        0xf3, 0x0f, 0x7f, 0x54, 0x24, 0x20, 
        0xf3, 0x0f, 0x7f, 0x4c, 0x24, 0x10, 
        0xf3, 0x0f, 0x7f, 0x04, 0x24, 

        0x48, 0x8d, 0x0c, 0x24, 
        0x48, 0x89, 0xe3, 
        0x48, 0x83, 0xec, 0x30, 
        0x48, 0x83, 0xe4, 0xf0, 
        0xff, 0x15, 0xa3, 0x00, 0x00, 0x00, 
        0x48, 0x89, 0xdc, 

        0xf3, 0x0f, 0x6f, 0x04, 0x24, 
        0xf3, 0x0f, 0x6f, 0x4c, 0x24, 0x10, 
        0xf3, 0x0f, 0x6f, 0x54, 0x24, 0x20, 
        0xf3, 0x0f, 0x6f, 0x5c, 0x24, 0x30, 
        0xf3, 0x0f, 0x6f, 0x64, 0x24, 0x40, 
        0xf3, 0x0f, 0x6f, 0x6c, 0x24, 0x50, 
        0xf3, 0x0f, 0x6f, 0x74, 0x24, 0x60, 
        0xf3, 0x0f, 0x6f, 0x7c, 0x24, 0x70, 
        0xf3, 0x44, 0x0f, 0x6f, 0x84, 0x24, 0x80, 0x00, 0x00, 0x00, 
        0xf3, 0x44, 0x0f, 0x6f, 0x8c, 0x24, 0x90, 0x00, 0x00, 0x00, 
        0xf3, 0x44, 0x0f, 0x6f, 0x94, 0x24, 0xa0, 0x00, 0x00, 0x00, 
        0xf3, 0x44, 0x0f, 0x6f, 0x9c, 0x24, 0xb0, 0x00, 0x00, 0x00, 
        0xf3, 0x44, 0x0f, 0x6f, 0xa4, 0x24, 0xc0, 0x00, 0x00, 0x00, 
        0xf3, 0x44, 0x0f, 0x6f, 0xac, 0x24, 0xd0, 0x00, 0x00, 0x00, 
        0xf3, 0x44, 0x0f, 0x6f, 0xb4, 0x24, 0xe0, 0x00, 0x00, 0x00, 
        0xf3, 0x44, 0x0f, 0x6f, 0xbc, 0x24, 0xf0, 0x00, 0x00, 0x00, 

        0x48, 0x81, 0xc4, 0x00, 0x01, 0x00, 0x00, 
        0x9d,
        0x41, 0x5f, 
        0x41, 0x5e, 
        0x41, 0x5d, 
        0x41, 0x5c, 
        0x41, 0x5b, 
        0x41, 0x5a, 
        0x41, 0x59, 
        0x41, 0x58, 
        0x5f, 
        0x5e, 
        0x5a, 
        0x59, 
        0x5b, 
        0x58, 
        0x5d, 
        0x5c, 
        0xc3, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    });
#else
constexpr auto asm_data = std::to_array<uint8_t>({0xff, 0x35, 0x91, 0x00, 0x00, 0x00, 0x54, 0x55, 0x50, 0x53, 0x51,
    0x52, 0x56, 0x57, 0x9c, 0x81, 0xec, 0x80, 0x00, 0x00, 0x00, 0xf3, 0x0f, 0x7f, 0x7c, 0x24, 0x90, 0xf3, 0x0f, 0x7f,
    0x74, 0x24, 0xa0, 0xf3, 0x0f, 0x7f, 0x6c, 0x24, 0xb0, 0xf3, 0x0f, 0x7f, 0x64, 0x24, 0xc0, 0xf3, 0x0f, 0x7f, 0x5c,
    0x24, 0xd0, 0xf3, 0x0f, 0x7f, 0x54, 0x24, 0xe0, 0xf3, 0x0f, 0x7f, 0x4c, 0x24, 0xf0, 0xf3, 0x0f, 0x7f, 0x04, 0x24,
    0x54, 0xff, 0x15, 0x8d, 0x00, 0x00, 0x00, 0x83, 0xc4, 0x04, 0xf3, 0x0f, 0x6f, 0x04, 0x24, 0xf3, 0x0f, 0x6f, 0x4c,
    0x24, 0x10, 0xf3, 0x0f, 0x6f, 0x54, 0x24, 0x20, 0xf3, 0x0f, 0x6f, 0x5c, 0x24, 0x30, 0xf3, 0x0f, 0x6f, 0x64, 0x24,
    0x40, 0xf3, 0x0f, 0x6f, 0x6c, 0x24, 0x50, 0xf3, 0x0f, 0x6f, 0x74, 0x24, 0x60, 0xf3, 0x0f, 0x6f, 0x7c, 0x24, 0x70,
    0x81, 0xc4, 0x80, 0x00, 0x00, 0x00, 0x9d, 0x5f, 0x5e, 0x5a, 0x59, 0x5b, 0x58, 0x5d, 0x5c, 0xc3, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00});
#endif

std::expected<MidHook, MidHook::Error> MidHook::create(void* target, MidHookFn destination) {
    return create(Allocator::global(), target, destination);
}

std::expected<MidHook, MidHook::Error> MidHook::create(
    const std::shared_ptr<Allocator>& allocator, void* target, MidHookFn destination) {
    MidHook hook{};

    if (const auto setup_result = hook.setup(allocator, reinterpret_cast<uint8_t*>(target), destination);
        !setup_result) {
        return std::unexpected{setup_result.error()};
    }

    return hook;
}

MidHook::MidHook(MidHook&& other) noexcept {
    *this = std::move(other);
}

MidHook& MidHook::operator=(MidHook&& other) noexcept {
    if (this != &other) {
        m_hook = std::move(other.m_hook);
        m_target = other.m_target;
        m_stub = std::move(other.m_stub);
        m_destination = other.m_destination;

        other.m_target = 0;
        other.m_destination = nullptr;
    }

    return *this;
}

void MidHook::reset() {
    *this = {};
}

std::expected<void, MidHook::Error> MidHook::setup(
    const std::shared_ptr<Allocator>& allocator, uint8_t* target, MidHookFn destination) {
    m_target = target;
    m_destination = destination;

    auto stub_allocation = allocator->allocate(asm_data.size());

    if (!stub_allocation) {
        return std::unexpected{Error::bad_allocation(stub_allocation.error())};
    }

    m_stub = std::move(*stub_allocation);

    std::copy(asm_data.begin(), asm_data.end(), m_stub.data());

#ifdef _M_X64
    store(m_stub.data() + sizeof(asm_data) - 16, m_destination);
#else
    store(m_stub.data() + sizeof(asm_data) - 8, m_destination);

    // 32-bit has some relocations we need to fix up as well.
    store(m_stub.data() + 0x02, m_stub.data() + m_stub.size() - 4);
    store(m_stub.data() + 0x47, m_stub.data() + m_stub.size() - 8);
#endif

    auto hook_result = InlineHook::create(allocator, m_target, m_stub.data());

    if (!hook_result) {
        m_stub.free();
        return std::unexpected{Error::bad_inline_hook(hook_result.error())};
    }

    m_hook = std::move(*hook_result);

#ifdef _M_X64
    store(m_stub.data() + sizeof(asm_data) - 8, m_hook.trampoline().data());
#else
    store(m_stub.data() + sizeof(asm_data) - 4, m_hook.trampoline().data());
#endif

    return {};
}
} // namespace safetyhook

//
// Source file: thread_freezer.cpp
//

#include <Windows.h>
#include <winternl.h>


#pragma comment(lib, "ntdll")

extern "C" {
NTSTATUS
NTAPI
NtGetNextThread(HANDLE ProcessHandle, HANDLE ThreadHandle, ACCESS_MASK DesiredAccess, ULONG HandleAttributes,
    ULONG Flags, PHANDLE NewThreadHandle);
}

namespace safetyhook {
void execute_while_frozen(
    const std::function<void()>& run_fn, const std::function<void(uint32_t, HANDLE, CONTEXT&)>& visit_fn) {
    // Freeze all threads.
    int num_threads_frozen;
    auto first_run = true;

    do {
        num_threads_frozen = 0;
        HANDLE thread{};

        while (true) {
            HANDLE next_thread{};
            const auto status = NtGetNextThread(GetCurrentProcess(), thread,
                THREAD_QUERY_LIMITED_INFORMATION | THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, 0,
                0, &next_thread);

            if (thread != nullptr) {
                CloseHandle(thread);
            }

            if (!NT_SUCCESS(status)) {
                break;
            }

            thread = next_thread;

            const auto thread_id = GetThreadId(thread);

            if (thread_id == 0 || thread_id == GetCurrentThreadId()) {
                continue;
            }

            const auto suspend_count = SuspendThread(thread);

            if (suspend_count == static_cast<DWORD>(-1)) {
                continue;
            }

            // Check if the thread was already frozen. Only resume if the thread was already frozen, and it wasn't the
            // first run of this freeze loop to account for threads that may have already been frozen for other reasons.
            if (suspend_count != 0 && !first_run) {
                ResumeThread(thread);
                continue;
            }

            CONTEXT thread_ctx{};

            thread_ctx.ContextFlags = CONTEXT_FULL;

            if (GetThreadContext(thread, &thread_ctx) == FALSE) {
                continue;
            }

            if (visit_fn) {
                visit_fn(thread_id, thread, thread_ctx);
            }

            ++num_threads_frozen;
        }

        first_run = false;
    } while (num_threads_frozen != 0);

    // Run the function.
    if (run_fn) {
        run_fn();
    }

    // Resume all threads.
    HANDLE thread{};

    while (true) {
        HANDLE next_thread{};
        const auto status = NtGetNextThread(GetCurrentProcess(), thread,
            THREAD_QUERY_LIMITED_INFORMATION | THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, 0, 0,
            &next_thread);

        if (thread != nullptr) {
            CloseHandle(thread);
        }

        if (!NT_SUCCESS(status)) {
            break;
        }

        thread = next_thread;

        const auto thread_id = GetThreadId(thread);

        if (thread_id == 0 || thread_id == GetCurrentThreadId()) {
            continue;
        }

        ResumeThread(thread);
    }
}

void fix_ip(CONTEXT& ctx, uint8_t* old_ip, uint8_t* new_ip) {
#ifdef _M_X64
    auto ip = ctx.Rip;
#else
    auto ip = ctx.Eip;
#endif

    if (ip == reinterpret_cast<uintptr_t>(old_ip)) {
        ip = reinterpret_cast<uintptr_t>(new_ip);
    }

#ifdef _M_X64
    ctx.Rip = ip;
#else
    ctx.Eip = ip;
#endif
}
} // namespace safetyhook

//
// Source file: utility.cpp
//

#include <Windows.h>


namespace safetyhook {
bool is_page_executable(uint8_t* address) {
    MEMORY_BASIC_INFORMATION mbi;

    if (VirtualQuery(address, &mbi, sizeof(mbi)) == 0) {
        return false;
    }

    const auto executable_protect = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

    return (mbi.Protect & executable_protect) != 0;
}

bool is_executable(uint8_t* address) {
    LPVOID image_base_ptr;

    if (RtlPcToFileHeader(address, &image_base_ptr) == nullptr) {
        return is_page_executable(address);
    }

    // Just check if the section is executable.
    const auto* image_base = reinterpret_cast<uint8_t*>(image_base_ptr);
    const auto* dos_hdr = reinterpret_cast<const IMAGE_DOS_HEADER*>(image_base);

    if (dos_hdr->e_magic != IMAGE_DOS_SIGNATURE) {
        return is_page_executable(address);
    }

    const auto* nt_hdr = reinterpret_cast<const IMAGE_NT_HEADERS*>(image_base + dos_hdr->e_lfanew);

    if (nt_hdr->Signature != IMAGE_NT_SIGNATURE) {
        return is_page_executable(address);
    }

    const auto* section = IMAGE_FIRST_SECTION(nt_hdr);

    for (auto i = 0; i < nt_hdr->FileHeader.NumberOfSections; ++i, ++section) {
        if (address >= image_base + section->VirtualAddress &&
            address < image_base + section->VirtualAddress + section->Misc.VirtualSize) {
            return (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
        }
    }

    return is_page_executable(address);
}
} // namespace safetyhook

//
// Source file: vmt_hook.cpp
//

#include <Windows.h>



namespace safetyhook {
VmHook::VmHook(VmHook&& other) noexcept {
    *this = std::move(other);
}

VmHook& VmHook::operator=(VmHook&& other) noexcept {
    destroy();
    m_original_vm = other.m_original_vm;
    m_new_vm = other.m_new_vm;
    m_vmt_entry = other.m_vmt_entry;
    m_new_vmt_allocation = std::move(other.m_new_vmt_allocation);
    other.m_original_vm = nullptr;
    other.m_new_vm = nullptr;
    other.m_vmt_entry = nullptr;
    return *this;
}

VmHook::~VmHook() {
    destroy();
}

void VmHook::reset() {
    *this = {};
}

void VmHook::destroy() {
    if (m_original_vm != nullptr) {
        *m_vmt_entry = m_original_vm;
        m_original_vm = nullptr;
        m_new_vm = nullptr;
        m_vmt_entry = nullptr;
        m_new_vmt_allocation.reset();
    }
}

std::expected<VmtHook, VmtHook::Error> VmtHook::create(void* object) {
    VmtHook hook{};

    const auto original_vmt = *reinterpret_cast<uint8_t***>(object);
    hook.m_objects.emplace(object, original_vmt);

    // Count the number of virtual method pointers. We start at one to account for the RTTI pointer.
    auto num_vmt_entries = 1;

    for (auto vm = original_vmt; is_executable(*vm); ++vm) {
        ++num_vmt_entries;
    }

    // Allocate memory for the new VMT.
    auto allocation = Allocator::global()->allocate(num_vmt_entries * sizeof(uint8_t*));

    if (!allocation) {
        return std::unexpected{Error::bad_allocation(allocation.error())};
    }

    hook.m_new_vmt_allocation = std::make_shared<Allocation>(std::move(*allocation));
    hook.m_new_vmt = reinterpret_cast<uint8_t**>(hook.m_new_vmt_allocation->data());

    // Copy pointer to RTTI.
    hook.m_new_vmt[0] = original_vmt[-1];

    // Copy virtual method pointers.
    for (auto i = 0; i < num_vmt_entries - 1; ++i) {
        hook.m_new_vmt[i + 1] = original_vmt[i];
    }

    *reinterpret_cast<uint8_t***>(object) = &hook.m_new_vmt[1];

    return hook;
}

VmtHook::VmtHook(VmtHook&& other) noexcept {
    *this = std::move(other);
}

VmtHook& VmtHook::operator=(VmtHook&& other) noexcept {
    destroy();
    m_objects = std::move(other.m_objects);
    m_new_vmt_allocation = std::move(other.m_new_vmt_allocation);
    m_new_vmt = other.m_new_vmt;
    other.m_new_vmt = nullptr;
    return *this;
}

VmtHook::~VmtHook() {
    destroy();
}

void VmtHook::apply(void* object) {
    m_objects.emplace(object, *reinterpret_cast<uint8_t***>(object));
    *reinterpret_cast<uint8_t***>(object) = &m_new_vmt[1];
}

void VmtHook::remove(void* object) {
    const auto search = m_objects.find(object);

    if (search == m_objects.end()) {
        return;
    }

    const auto original_vmt = search->second;

    execute_while_frozen([&] {
        if (IsBadWritePtr(object, sizeof(void*))) {
            return;
        }

        if (*reinterpret_cast<uint8_t***>(object) != &m_new_vmt[1]) {
            return;
        }

        *reinterpret_cast<uint8_t***>(object) = original_vmt;
    });

    m_objects.erase(search);
}

void VmtHook::reset() {
    *this = {};
}

void VmtHook::destroy() {
    execute_while_frozen([this] {
        for (const auto [object, original_vmt] : m_objects) {
            if (IsBadWritePtr(object, sizeof(void*))) {
                return;
            }

            if (*reinterpret_cast<uint8_t***>(object) != &m_new_vmt[1]) {
                return;
            }

            *reinterpret_cast<uint8_t***>(object) = original_vmt;
        }
    });

    m_objects.clear();
    m_new_vmt_allocation.reset();
    m_new_vmt = nullptr;
}
} // namespace safetyhook