#pragma once

#include <karm-base/std.h>

namespace x86_64 {

inline void cli(void) { asm volatile("cli"); }

inline void sti(void) { asm volatile("sti"); }

inline void hlt(void) { asm volatile("hlt"); }

inline void pause(void) { asm volatile("pause"); }

inline void invlpg(size_t addr) {
    asm volatile("invlpg (%0)" ::"r"(addr)
                 : "memory");
}

/* --- CRs ------------------------------------------------------------------ */

enum cr0_bit {
    CR0_PROTECTED_MODE_ENABLE = (1 << 0),
    CR0_MONITOR_CO_PROCESSOR = (1 << 1),
    CR0_EMULATION = (1 << 2),
    CR0_TASK_SWITCHED = (1 << 3),
    CR0_EXTENSION_TYPE = (1 << 4),
    CR0_NUMERIC_ERROR_ENABLE = (1 << 5),
    CR0_WRITE_PROTECT_ENABLE = (1 << 16),
    CR0_ALIGNMENT_MASK = (1 << 18),
    CR0_NOT_WRITE_THROUGH_ENABLE = (1 << 29),
    CR0_CACHE_DISABLE = (1 << 30),
    CR0_PAGING_ENABLE = (1 << 31),
};

enum cr4_bit {
    CR4_VIRTUAL_8086_MODE_EXT = (1 << 0),
    CR4_PROTECTED_MODE_VIRTUAL_INT = (1 << 1),
    CR4_TIME_STAMP_DISABLE = (1 << 2), // disable it only for ring != 0 for RDTSC instruction
    CR4_DEBUGGING_EXT = (1 << 3),      // enable debug register break on io space
    CR4_PAGE_SIZE_EXT = (1 << 4),
    CR4_PHYSICAL_ADDRESS_EXT = (1 << 5),
    CR4_MACHINE_CHECK_EXCEPTION = (1 << 6),
    CR4_PAGE_GLOBAL_ENABLE = (1 << 7),
    CR4_PERFORMANCE_COUNTER_ENABLE = (1 << 8),
    CR4_FXSR_ENABLE = (1 << 9),
    CR4_SIMD_EXCEPTION_SUPPORT = (1 << 10), // Operating System Support for Unmasked SIMD Floating-Point Exceptions
    CR4_USER_MODE_INSTRUCTION_PREVENTION = (1 << 11),
    CR4_5_LEVEL_PAGING_ENABLE = (1 << 12),
    CR4_VIRTUAL_MACHINE_EXT_ENABLE = (1 << 13),
    CR4_SAFER_MODE_EXT_ENABLE = (1 << 14),
    CR4_FS_GS_BASE_ENABLE = (1 << 16),
    CR4_PCID_ENABLE = (1 << 17),
    CR4_XSAVE_ENABLE = (1 << 18),
    CR4_SUPERVISOR_EXE_PROTECTION_ENABLE = (1 << 20),
    CR4_SUPERVISOR_ACCESS_PROTECTION_ENABLE = (1 << 21),
    CR4_KEY_PROTECTION_ENABLE = (1 << 22),
    CR4_CONTROL_FLOW_ENABLE = (1 << 23),
    CR4_SUPERVISOR_KEY_PROTECTION_ENABLE = (1 << 24),
};

#define CR(N)                                         \
    inline uint64_t rdcr##N(void) {                   \
        uint64_t value = 0;                           \
        asm volatile("mov %%cr" #N ", %0"             \
                     : "=r"(value));                  \
        return value;                                 \
    }                                                 \
                                                      \
    inline void wrcr##N(uint64_t value) {             \
        asm volatile("mov %0, %%cr" #N ::"a"(value)); \
    }

CR(0)
CR(1)
CR(2)
CR(3)
CR(4)

#undef CR

/* --- AVX/SSSE ------------------------------------------------------------- */

enum xcr0_bit {
    XCR0_XSAVE_SAVE_X87 = (1 << 0),
    XCR0_XSAVE_SAVE_SSE = (1 << 1),
    XCR0_AVX_ENABLE = (1 << 2),
    XCR0_BNDREG_ENABLE = (1 << 3),
    XCR0_BNDCSR_ENABLE = (1 << 4),
    XCR0_AVX512_ENABLE = (1 << 5),
    XCR0_ZMM0_15_ENABLE = (1 << 6),
    XCR0_ZMM16_32_ENABLE = (1 << 7),
    XCR0_PKRU_ENABLE = (1 << 9),
};

inline uint64_t rdxcr(uint32_t i) {
    uint32_t eax, edx;
    asm volatile("xgetbv"

                 : "=a"(eax), "=d"(edx)
                 : "c"(i)
                 : "memory");

    return eax | ((uint64_t)edx << 32);
}

inline void wrxcr(uint32_t i, uint64_t value) {
    uint32_t edx = value >> 32;
    uint32_t eax = (uint32_t)value;
    asm volatile("xsetbv"
                 :
                 : "a"(eax), "d"(edx), "c"(i)
                 : "memory");
}

inline void xsave(void *region) {
    asm volatile("xsave (%0)" ::"a"(region));
}

inline void xrstor(void const *region) {
    asm volatile("xrstor (%0)" ::"a"(region));
}

inline void fninit(void) {
    asm volatile("fninit");
}

inline void fxsave(void *region) {
    asm volatile("fxsave (%0)" ::"a"(region));
}

inline void fxrstor(void const *region) {
    asm volatile("fxrstor (%0)" ::"a"(region));
}

/* --- Msrs ----------------------------------------------------------------- */

enum struct Msrs : uint64_t {
    APIC = 0x1B,
    EFER = 0xC0000080,
    STAR = 0xC0000081,
    LSTAR = 0xC0000082,
    COMPAT_STAR = 0xC0000083,
    SYSCALL_FLAG_MASK = 0xC0000084,
    FS_BASE = 0xC0000100,
    GS_BASE = 0xC0000101,
    KERN_GS_BASE = 0xc0000102,
};

inline uint64_t rdmsr(Msrs msr) {

    uint32_t low, high;
    asm volatile("rdmsr"
                 : "=a"(low), "=d"(high)
                 : "c"((uint64_t)msr));
    return ((uint64_t)high << 32) | low;
}

inline void wrmsr(Msrs msr, uint64_t value) {

    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    asm volatile("wrmsr"
                 :
                 : "c"((uint64_t)msr), "a"(low), "d"(high));
}

} // namespace x86_64
